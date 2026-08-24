#ifndef _BS_MA_AUDIO_SYSTEM_H_
#define _BS_MA_AUDIO_SYSTEM_H_

#include "common.h"
#include "audio_system.h"

#include "miniaudio.h"

#define MAX_SOUND_INSTANCES 128
#define SOUND_INSTANCE_ID_BASE 100000
#define MAX_AUDIO_STREAMS 32
// This is the index space that the native runner uses
#define AUDIO_STREAM_INDEX_BASE 300000
#define MAX_LISTENERS 4

typedef struct {
    bool active;
    int32_t soundIndex; // SOND resource that spawned this
    int32_t instanceId; // unique ID returned to GML
    ma_sound maSound; // miniaudio sound object
    ma_decoder decoder; // decoder for memory-based audio
    bool ownsDecoder; // true if decoder needs uninit
    float targetGain;
    float currentGain;
    float fadeTimeRemaining;
    float fadeTotalTime;
    float startGain;
    int32_t priority;
} SoundInstance;

typedef struct {
    bool active;
    char* filePath; // resolved file path (owned, freed on destroy)
    float initialGain;
    float initialPitch;
} AudioStreamEntry;

typedef struct {
    AudioSystem base;
    ma_engine engine;
    ma_device device;
    SoundInstance instances[MAX_SOUND_INSTANCES];
    int32_t nextInstanceCounter;
    FileSystem* fileSystem;
    AudioStreamEntry streams[MAX_AUDIO_STREAMS];
    ma_sound_group listenerGroups[MAX_LISTENERS];
    float listenerGains[MAX_LISTENERS];
} MaAudioSystem;

MaAudioSystem* MaAudioSystem_create(DataWin* dataWin);

// Shared post-engine setup: resets instance slots and creates the listener sound
// groups. Requires an initialized engine. Used by init and by backends that overlay
// this one (e.g. the SDL 1.2 backend).
void MaAudioSystem_resetState(MaAudioSystem* ma);

// Returns this backend's vtable, for overlay backends that inherit it and override
// individual functions.
const AudioSystemVtable* MaAudioSystem_vtable(void);

#endif /* _BS_MA_AUDIO_SYSTEM_H_ */
