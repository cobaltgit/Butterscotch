#include "sdl_audio_system.h"
#include "utils.h"

#include <SDL/SDL_audio.h>
#include <string.h>
#include "stb_ds.h"

// ===[ SDL audio callback ]===

// Scratch buffer for float->S16 conversion. One static is fine: SDL serialises
// callbacks for the single device we open.
#define SDL_MIX_MAX_FRAMES 512
static float sdlMixScratch[SDL_MIX_MAX_FRAMES * 2]; // stereo f32

static void sdlAudioCallback(void* userdata, Uint8* stream, int len) {
    SdlAudioSystem* sa = (SdlAudioSystem*) userdata;

    // SDL 1.2 starts the audio thread the moment the device opens, which can
    // beat ma_engine_init. Stay silent until sdlInit has fully brought the
    // engine up (SDL_PauseAudio provides the happens-before for `ready`).
    if (!sa->ready) {
        memset(stream, 0, (size_t) len);
        return;
    }

    int channels = sa->sdlSpec.channels > 0 ? sa->sdlSpec.channels : 2;
    int16_t* out = (int16_t*) stream;
    int framesLeft = len / (int) (channels * sizeof(int16_t));

    while (framesLeft > 0) {
        int chunk = framesLeft < SDL_MIX_MAX_FRAMES ? framesLeft : SDL_MIX_MAX_FRAMES;

        ma_uint64 framesRead = 0;
        ma_engine_read_pcm_frames(&sa->ma.engine, sdlMixScratch, (ma_uint64) chunk, &framesRead);

        // Zero-fill any tail miniaudio didn't fill (silence on underrun)
        if ((int) framesRead < chunk) {
            memset(&sdlMixScratch[framesRead * 2], 0,
                   (size_t) (chunk - (int) framesRead) * 2 * sizeof(float));
        }

        for (int i = 0, n = chunk * channels; i < n; i++) {
            float s = sdlMixScratch[i];
            if      (s >  1.0f) s =  1.0f;
            else if (s < -1.0f) s = -1.0f;
            out[i] = (int16_t) (s * 32767.0f);
        }

        out += chunk * channels;
        framesLeft -= chunk;
    }
}

// ===[ Lifecycle overrides ]===

static void sdlInit(AudioSystem* audio, DataWin* dataWin, FileSystem* fileSystem) {
    SdlAudioSystem* sa = (SdlAudioSystem*) audio;

    arrput(sa->ma.base.audioGroups, dataWin);
    sa->ma.fileSystem = fileSystem;

    // Open the SDL device first so miniaudio can be sized to whatever it gives us.
    // Failure is not fatal: the engine still comes up below so the game runs
    // silently instead of crashing on the first playSound.
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        logError("Audio: SDL_InitSubSystem(SDL_INIT_AUDIO) failed: %s\n", SDL_GetError());
    } else {
        SDL_AudioSpec desired;
        ZERO_STRUCT(desired);
        desired.freq     = 44100;
        desired.format   = AUDIO_S16SYS;
        desired.channels = 2;
        desired.samples  = 1024; // ~23 ms at 44100 Hz
        desired.callback = sdlAudioCallback;
        desired.userdata = sa;

        if (SDL_OpenAudio(&desired, &sa->sdlSpec) < 0) {
            logError("Audio: SDL_OpenAudio failed: %s\n", SDL_GetError());
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
        } else {
            sa->audioOpen = true;
            // The callback thread is already running at this point; hold it
            // (and keep the !ready gate in the callback as a belt-and-braces).
            SDL_PauseAudio(1);

            // With a non-NULL obtained spec, SDL hands us the hardware format
            // as-is. If that's something other than S16 stereo, reopen with
            // obtained=NULL so SDL transparently converts to what our mixer expects.
            if (sa->sdlSpec.format != AUDIO_S16SYS || sa->sdlSpec.channels > 2) {
                SDL_CloseAudio();
                if (SDL_OpenAudio(&desired, NULL) < 0) {
                    logError("Audio: SDL_OpenAudio (conversion retry) failed: %s\n", SDL_GetError());
                    SDL_QuitSubSystem(SDL_INIT_AUDIO);
                    sa->audioOpen = false;
                    ZERO_STRUCT(sa->sdlSpec);
                } else {
                    sa->sdlSpec = desired; // the callback now receives exactly this format
                    SDL_PauseAudio(1);
                }
            }
        }
    }

    ma_engine_config config = ma_engine_config_init();
    config.noDevice   = MA_TRUE;
    config.channels   = sa->audioOpen && sa->sdlSpec.channels == 1 ? 1 : 2;
    config.sampleRate = (ma_uint32) (sa->audioOpen ? sa->sdlSpec.freq : 44100);

    ma_result result = ma_engine_init(&config, &sa->ma.engine);
    if (result != MA_SUCCESS) {
        logError("Audio: Failed to initialize miniaudio no-device engine (error %d)\n", result);
        if (sa->audioOpen) {
            SDL_CloseAudio();
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
            sa->audioOpen = false;
        }
        return;
    }

    // Shared post-engine setup: instance slots + listener sound groups
    // (maPlaySound routes every sound through listener group 0).
    MaAudioSystem_resetState(&sa->ma);

    if (sa->audioOpen) {
        sa->ready = true;
        SDL_PauseAudio(0); // start the callback
    }

    logInfo("Audio: SDL1 overlay initialized (%u Hz, %d ch, %u-frame buffer)\n",
            (unsigned) config.sampleRate, (int) config.channels,
            (unsigned) (sa->audioOpen ? sa->sdlSpec.samples : 0));
}

static void sdlDestroy(AudioSystem* audio) {
    SdlAudioSystem* sa = (SdlAudioSystem*) audio;

    // Stop the callback before tearing down the engine it drains.
    // SDL_CloseAudio blocks until any in-flight callback has returned.
    if (sa->audioOpen) {
        SDL_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        sa->audioOpen = false;
    }

    // Delegate the rest of teardown (instances, streams, audio groups,
    // ma_engine_uninit) to the shared miniaudio destroy. Its trailing free(ma)
    // releases our full allocation because MaAudioSystem is the first member
    // of SdlAudioSystem.
    MaAudioSystem_vtable()->destroy(audio);
}

static void sdlSuspend(AudioSystem* audio) {
    SdlAudioSystem* sa = (SdlAudioSystem*) audio;
    if (sa->audioOpen) SDL_PauseAudio(1);
}

static void sdlResume(AudioSystem* audio) {
    SdlAudioSystem* sa = (SdlAudioSystem*) audio;
    if (sa->audioOpen) SDL_PauseAudio(0);
}

// ===[ Vtable ]===

// Inherit every playSound/stopSound/update/gain/pitch/stream function from the
// miniaudio backend, then patch the few that need SDL-aware behaviour.
static AudioSystemVtable sdlAudioSystemVtable;

SdlAudioSystem* SdlAudioSystem_create(DataWin* dataWin) {
    SdlAudioSystem* sa = safeCalloc(1, sizeof(SdlAudioSystem));

    sdlAudioSystemVtable         = *MaAudioSystem_vtable(); // inherit everything
    sdlAudioSystemVtable.init    = sdlInit;                 // override: noDevice engine + SDL_OpenAudio
    sdlAudioSystemVtable.destroy = sdlDestroy;              // override: close SDL device first
    sdlAudioSystemVtable.suspend = sdlSuspend;              // override: inherited ones target a NULL device
    sdlAudioSystemVtable.resume  = sdlResume;

    sa->ma.base.vtable = &sdlAudioSystemVtable;
    sa->ma.base.dw = dataWin;
    return sa;
}
