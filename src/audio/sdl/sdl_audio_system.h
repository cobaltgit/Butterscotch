#pragma once

// Pull in MaAudioSystem, its vtable, SoundInstance, AudioStreamEntry, and
// the miniaudio types — everything the shared vtable functions operate on.
#include <SDL/SDL.h>
#include "ma_audio_system.h"

// SdlAudioSystem IS-A MaAudioSystem: MaAudioSystem must be the first member
// so that AudioSystem* casts chain correctly through the base pointer.
// The engine will be initialised in noDevice mode; the SDL callback drains it.
typedef struct {
    MaAudioSystem ma;       // first member — pointer-compatible with AudioSystem*
    SDL_AudioSpec sdlSpec;  // what SDL actually opened (may differ from desired)
} SdlAudioSystem;

SdlAudioSystem* SdlAudioSystem_create(void);
