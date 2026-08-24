#ifndef _BS_SDL_AUDIO_SYSTEM_H_
#define _BS_SDL_AUDIO_SYSTEM_H_

#include "common.h"
#include "audio_system.h"

#include <SDL/SDL.h>
#include "ma_audio_system.h"

// SDL 1.2 audio backend for low-end devices (e.g. the Miyoo Mini family), where
// miniaudio's own device layer misbehaves (slow, low-pitched playback).
//
// It overlays the miniaudio backend: SdlAudioSystem IS-A MaAudioSystem and inherits
// its entire vtable (instances, decoding, gain/pitch/fades, streams). Only device
// output changes hands: instead of letting miniaudio open the sound card, an SDL 1.2
// audio callback drains a no-device ma_engine and converts f32 -> S16SYS.
typedef struct {
    MaAudioSystem ma;       // first member - pointer-compatible with AudioSystem*
    SDL_AudioSpec sdlSpec;  // format the callback actually receives
    bool audioOpen;
    volatile bool ready;    // false until the engine is live; callback outputs silence
} SdlAudioSystem;

SdlAudioSystem* SdlAudioSystem_create(DataWin* dataWin);

#endif /* _BS_SDL_AUDIO_SYSTEM_H_ */
