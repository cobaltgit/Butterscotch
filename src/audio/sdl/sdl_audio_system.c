#include "sdl_audio_system.h"
#include "utils.h"

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <string.h>
#include <stdio.h>

#include "stb_ds.h"

// ===[ SDL audio callback ]===

// Scratch buffer for float→S16 conversion. One global is fine — SDL only
// ever has one audio device open and the callback is serialised by SDL.
#define SDL_MA_MAX_FRAMES 4096
static float sdlMixScratch[SDL_MA_MAX_FRAMES * 2]; // stereo f32

static void sdlAudioCallback(void* userdata, Uint8* stream, int len) {
    SdlAudioSystem* sa = (SdlAudioSystem*) userdata;

    int frameCount = len / (2 * (int) sizeof(int16_t));
    if (frameCount > SDL_MA_MAX_FRAMES) frameCount = SDL_MA_MAX_FRAMES;

    ma_uint64 framesRead = 0;
    ma_engine_read_pcm_frames(&sa->ma.engine, sdlMixScratch,
                              (ma_uint64) frameCount, &framesRead);

    // Zero-fill any tail miniaudio didn't fill (silence on underrun)
    if ((int) framesRead < frameCount)
        memset(&sdlMixScratch[framesRead * 2], 0,
               (size_t)(frameCount - (int) framesRead) * 2 * sizeof(float));

    // Convert f32 stereo → S16SYS stereo in-place into SDL's buffer
    int16_t* out = (int16_t*) stream;
    for (int i = 0, n = frameCount * 2; i < n; i++) {
        float s = sdlMixScratch[i];
        if      (s >  1.0f) s =  1.0f;
        else if (s < -1.0f) s = -1.0f;
        out[i] = (int16_t)(s * 32767.0f);
    }
}

// ===[ Lifecycle overrides ]===

static void sdlInit(AudioSystem* audio, DataWin* dataWin, FileSystem* fileSystem) {
    SdlAudioSystem* sa = (SdlAudioSystem*) audio;

    // Manually replicate the non-device parts of maInit: set up the fields
    // that every shared vtable function (playSound, update, etc.) depends on.
    // We do NOT call maAudioSystemVtable.init because that would open an ALSA
    // device — SDL owns the hardware here instead.
    arrput(sa->ma.base.audioGroups, dataWin);
    sa->ma.fileSystem = fileSystem;
    memset(sa->ma.instances, 0, sizeof(sa->ma.instances));
    sa->ma.nextInstanceCounter = 0;

    // Open the SDL audio device first so we can match miniaudio's config to
    // whatever freq/channels SDL actually gives us.
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Audio: SDL_InitSubSystem(AUDIO) failed: %s\n", SDL_GetError());
        return;
    }

    SDL_AudioSpec desired;
    memset(&desired, 0, sizeof(desired));
    desired.freq     = 44100;
    desired.format   = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples  = 512;       // ~23 ms at 44100; tune if you hear dropouts
    desired.callback = sdlAudioCallback;
    desired.userdata = sa;         // sa is already fully allocated before init is called

    if (SDL_OpenAudio(&desired, &sa->sdlSpec) < 0) {
        fprintf(stderr, "Audio: SDL_OpenAudio failed: %s\n", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }

    // Initialise miniaudio in noDevice mode, sized to match what SDL gave us.
    ma_engine_config config = ma_engine_config_init();
    config.noDevice   = MA_TRUE;
    config.channels   = sa->sdlSpec.channels;
    config.sampleRate = (ma_uint32) sa->sdlSpec.freq;

    ma_result result = ma_engine_init(&config, &sa->ma.engine);
    if (result != MA_SUCCESS) {
        fprintf(stderr, "Audio: miniaudio noDevice init failed (error %d)\n", result);
        SDL_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return;
    }

    SDL_PauseAudio(0); // start the callback

    fprintf(stderr, "Audio: SDL audio opened (%d Hz, %d ch, %d frame buffer); "
                    "miniaudio engine initialized\n",
            sa->sdlSpec.freq, sa->sdlSpec.channels, sa->sdlSpec.samples);
}

static void sdlDestroy(AudioSystem* audio) {
    // Stop the callback before tearing down the engine it reads from.
    // SDL_CloseAudio blocks until any in-progress callback returns.
    SDL_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    // Delegate the rest of teardown (uninit instances, free audioGroups,
    // ma_engine_uninit, free(ma)) to the shared miniaudio destroy.
    //
    // The free(ma) inside maDestroy is safe: MaAudioSystem is the first
    // member of SdlAudioSystem, so (MaAudioSystem*) sa == (void*) sa and
    // free correctly releases the full SdlAudioSystem allocation.
    maAudioSystemVtable.destroy(audio);
}

// ===[ Vtable ]===

// Initialised at create-time by copying MaAudioSystem's vtable so we
// inherit every playSound/stopSound/update/gain/pitch/stream function for free,
// then patch the two functions that need SDL-aware behaviour.
static AudioSystemVtable sdlAudioSystemVtable;

SdlAudioSystem* SdlAudioSystem_create(void) {
    SdlAudioSystem* sa = safeCalloc(1, sizeof(SdlAudioSystem));

    sdlAudioSystemVtable         = maAudioSystemVtable; // inherit everything
    sdlAudioSystemVtable.init    = sdlInit;             // override: noDevice + SDL_OpenAudio
    sdlAudioSystemVtable.destroy = sdlDestroy;          // override: SDL_CloseAudio first

    sa->ma.base.vtable = &sdlAudioSystemVtable;
    return sa;
}
