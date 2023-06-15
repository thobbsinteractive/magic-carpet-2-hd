
// OpenAL 3D audio implementation
//
// includes reverb effect for added ambiance and the makings of a few positional elements
//
// resources:
// OpenAL official doc https://www.openal.org/documentation/
// OpenAL-soft doc     https://github.com/kcat/openal-soft/wiki/Programmer%27s-Guide
// nice guide          https://indiegamedev.net/2020/04/12/the-complete-guide-to-openal-with-c-part-3-positioning-sounds/
// EFX code based on   https://github.com/kcat/openal-soft/blob/master/examples/alreverb.c
//
// Authors:
//    2023 - Petre Rodan (complete rewrite)
//

#include <stdlib.h>
#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"
#include "AL/efx.h"
#include "AL/efx-presets.h"
#include "../engine/ail_sound.h"
#include "../engine/global_types.h"
#include "../engine/read_config.h"
#include "../engine/Sound.h"
#include "port_sound_lut.h"
#include "port_sdl_sound.h"
#include "port_openal.h"

///< al_chunk_cache_t flags
#define          OPENAL_FLG_LOADED  0x1     ///< if chunk was properly loaded via alBufferData()

#define   AL_DIST_REFRESH_INTERVAL  200     ///< after how many ms shoud the distance between creatures and the listener should be refreshed
#define           AL_DIST_MIN_PLAY  10000   ///< minimal distance to the player needed for creature to play it's sample

// Effect object functions
static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
static LPALEFFECTFV alEffectfv;
static LPALGETEFFECTI alGetEffecti;
static LPALGETEFFECTIV alGetEffectiv;
static LPALGETEFFECTF alGetEffectf;
static LPALGETEFFECTFV alGetEffectfv;

// Auxiliary Effect Slot object functions
static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

struct al_chunk {
    int16_t id;                 ///< chunk identifier
    ALint state;                ///< 0, AL_PLAYING or something in between
    ALuint alSource;            ///< openal source identifier
    ALsizei size;               ///< chunk size
    event_t *entity;            ///< what entity has created the sound source
};
typedef struct al_chunk al_chunk_t;     ///< element of the currently playing chunks array

struct al_chunk_cache {
    int16_t id;                 ///< chunk identifier
    uint16_t flags;             ///< 0 or OPENAL_FLG_LOADED
    ALuint bufferName;          ///< openal buffer identifier
    ALsizei size;               ///< chunk size
};
typedef struct al_chunk_cache al_chunk_cache_t; ///< element of the cached chunks array

struct al_env {
    uint8_t initialized;        ///< '1' if the OpenAL-soft library was properly initialized
    uint8_t efx_initialized;    ///< '1' if the ALC_EXT_EFX extension is usable
    uint8_t scheduling_enabled; ///< state of the chunk scheduling
    int8_t bank;                ///< current sound bank
    int8_t reverb_type;         ///< should match the current MapType
    uint32_t frame_cnt;         ///< frame counter
    axis_3d listener_c;         ///< the listener's coordinates in game space (x, y, z)
    axis_4d listener_o;         ///< the listener's orientation values (yaw, pitch, roll)
};
typedef struct al_env al_env_t; ///< random collection of global variables

struct al_next_vol {            ///< sometimes the volume of a chunk is received before the chunk itself, so keep that info here
    int16_t chunk_id;           ///< chunk identifier
    float gain;                 ///< volume (0-127) converted into gain (0-1.0)
};
typedef struct al_next_vol al_next_vol_t;

ALCcontext *context;
ALuint al_slot;                 ///< effect slot
ALuint al_effect;
al_chunk_t alc[OPENAL_C_SZ] = { };      ///< currently playing chunks array
al_chunk_cache_t alcc[OPENAL_CC_SZ] = { };      ///< cached chunks array
int8_t al_con[OPENAL_CC_SZ] = { }; ///< concurrency array (how many times is a particular chunk played currently)
al_env_t ale = { };             ///< the random collection of global variables

openal_config_t oac;            ///< subsystem configuration read from config.ini

// alsound_set_sample_volume for a chunk precedes alsound_play
// for that particular chunk. so store it's volume in alnv:
al_next_vol_t alnv = { };

const char *alsound_get_error_str(ALCenum error);
ALCenum alsound_error_check(const char *msg);
static ALuint alsound_load_effect(const EFXEAXREVERBPROPERTIES * reverb);

/// \brief find a chunk_id in the currently playing chunks array
/// \param chunk_id  identifier
/// \return location in the array or -1 if chunk not present
int16_t alsound_find_alc_sample(const int32_t id)
{
    int16_t i;
    //Logger->info("alsound_sample_status looking for {}", chunk_id);
    if (id > 128) {
        for (i = OPENAL_C_SZ; i > 0; i--) {
            if ((alc[i - 1].state == AL_PLAYING) && (alc[i - 1].size == id)) {
                return i - 1;
            }
        }
    } else {
        for (i = OPENAL_C_SZ; i > 0; i--) {
            if ((alc[i - 1].state == AL_PLAYING) && (alc[i - 1].id == id)) {
                return i - 1;
            }
        }
    }
    return -1;
}

/// \brief establish the presence of a chunk_id in the currently playing chunks array
/// \param chunk_id  identifier
/// \return  1 if sample is playing and 0 otherwise
uint8_t alsound_sample_status(const int32_t id)
{
    if (alsound_find_alc_sample(id) > -1) {
        return 1;
    }
    return 0;
}

/// \brief delete source based on play channel id
/// \param channel_id alc[] array index
void alsound_delete_source(const int16_t channel_id)
{
    //Logger->info("alsound_delete_source {}", channel_id);
    alDeleteSources(1, &alc[channel_id].alSource);
    alsound_error_check("alsound_delete_source alDeleteSources");
    if (alc[channel_id].entity) {
        //Logger->info("delete_source {}", channel_id);
        alc[channel_id].entity->play_ch = -1;
        alc[channel_id].entity = 0;
    }
    al_con[alc[channel_id].id]--;
    alc[channel_id].state = 0;
    alc[channel_id].size = 0;
    alc[channel_id].id = -1;
}

/// \brief stop chunk from playing based on chunk id
/// \param chunk_id  identifier
void alsound_end_sample(const int32_t chunk_id)
{
    int16_t ret;
    ret = alsound_find_alc_sample(chunk_id);
    if (ret > -1) {
        //Logger->info("alsound_end_sample id {}  ch {}", chunk_id, ret);
        alsound_delete_source(ret);
    }
}

/// \brief change the volume of a chunk that is currently playing
/// \param chunk_id  identifier
/// \param volume    0-127
void alsound_set_sample_volume(const int32_t chunk_id, const int32_t volume)
{
    int16_t ret;
    float gain = (float)volume / 127.0f;

    if (ale.bank < 3) {
        if (alct[ale.bank][chunk_id].flags & AL_IGNORE_RECODE) {
            return;
        }
    }

    ret = alsound_find_alc_sample(chunk_id);
    if (ret > -1) {
        alSourcef(alc[ret].alSource, AL_GAIN, gain);
        alsound_error_check("set_sample_volume alSourcef");
        //Logger->info("alsound_set_sample_volume {} {}", chunk_id, gain);
    }

    alnv.chunk_id = chunk_id;
    alnv.gain = gain;
}

/// \brief initialize OpenAL and it's EFX subsystem
void alsound_init()
{
    ALboolean enumeration;
    const ALCchar *defaultDeviceName;
    ALCdevice *device;
    EFXEAXREVERBPROPERTIES reverb = EFX_REVERB_PRESET_GENERIC;

    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE) {
        Logger->error("OpenAL: enumeration extension not available");
    }

    defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

    device = alcOpenDevice(defaultDeviceName);
    if (!device) {
        Logger->error("OpenAL: unable to open device");
        return;
    }

    Logger->info("OpenAL device: {}", alcGetString(device, ALC_DEVICE_SPECIFIER));
    alGetError();

    context = alcCreateContext(device, NULL);
    if (!alcMakeContextCurrent(context)) {
        Logger->error("OpenAL: unable to create default context");
        return;
    }

    if (alIsExtensionPresent("EAX-RAM") == AL_TRUE) {
        Logger->info("EAX-RAM was found!");
    }

    //alDistanceModel(AL_NONE);
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    alsound_error_check("alDistanceModel");
    ale.frame_cnt = 0;
    ale.initialized = 1;

    if (oac.efx_enabled) {
        if (!alcIsExtensionPresent(device, "ALC_EXT_EFX")) {
            Logger->error("OpenAL: EFX not supported");
        } else {
            alGenEffects = (LPALGENEFFECTS) alGetProcAddress("alGenEffects");
            alDeleteEffects = (LPALDELETEEFFECTS) alGetProcAddress("alDeleteEffects");
            alIsEffect = (LPALISEFFECT) alGetProcAddress("alIsEffect");
            alEffecti = (LPALEFFECTI) alGetProcAddress("alEffecti");
            alEffectiv = (LPALEFFECTIV) alGetProcAddress("alEffectiv");
            alEffectf = (LPALEFFECTF) alGetProcAddress("alEffectf");
            alEffectfv = (LPALEFFECTFV) alGetProcAddress("alEffectfv");
            alGetEffecti = (LPALGETEFFECTI) alGetProcAddress("alGetEffecti");
            alGetEffectiv = (LPALGETEFFECTIV) alGetProcAddress("alGetEffectiv");
            alGetEffectf = (LPALGETEFFECTF) alGetProcAddress("alGetEffectf");
            alGetEffectfv = (LPALGETEFFECTFV) alGetProcAddress("alGetEffectfv");

            alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS) alGetProcAddress("alGenAuxiliaryEffectSlots");
            alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS) alGetProcAddress("alDeleteAuxiliaryEffectSlots");
            alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT) alGetProcAddress("alIsAuxiliaryEffectSlot");
            alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI) alGetProcAddress("alAuxiliaryEffectSloti");
            alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV) alGetProcAddress("alAuxiliaryEffectSlotiv");
            alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF) alGetProcAddress("alAuxiliaryEffectSlotf");
            alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV) alGetProcAddress("alAuxiliaryEffectSlotfv");
            alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI) alGetProcAddress("alGetAuxiliaryEffectSloti");
            alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV) alGetProcAddress("alGetAuxiliaryEffectSlotiv");
            alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF) alGetProcAddress("alGetAuxiliaryEffectSlotf");
            alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV) alGetProcAddress("alGetAuxiliaryEffectSlotfv");

            al_effect = alsound_load_effect(&reverb);
            if (!al_effect) {
                Logger->error("OpenAL: cannot load effect");
            } else {
                al_slot = 0;
                alGenAuxiliaryEffectSlots(1, &al_slot);
                alAuxiliaryEffectSloti(al_slot, AL_EFFECTSLOT_EFFECT, (ALint) al_effect);
                if (alGetError() == AL_NO_ERROR) {
                    Logger->info("OpenAL: EFX init success");
                    ale.efx_initialized = 1;
                }
            }
        }
    }

    srand((unsigned int)time(NULL));
}

/// \brief set environmental variables
/// \param value  multiple-role input
/// \param flag   role-defining option: AL_SET_BANK 
void alsound_set_env(const int32_t value, const uint8_t flag)
{
    switch (flag) {
    case AL_SET_BANK:
        ale.bank = value;
        break;
    default:
        break;
    }
}

/// \brief update the listener's position once every frame
/// \param coord  cartesian coordinates
/// \param orient orientation
void alsound_set_location(axis_3d *coord, axis_4d *orient)
{
    ale.listener_c.x = coord->x;
    ale.listener_c.y = coord->y;
    ale.listener_c.z = coord->z;

    ale.listener_o.yaw = orient->yaw;
    //ale.listener_r.pitch = orient->pitch;
    //ale.listener_r.roll = orient->roll;
    //Logger->info("set location {} {} {}", ale.listener_c.x, ale.listener_c.y, ale.listener_c.z);
}

/// \brief cleanup currently playing chunk array, apply listener position called once every frame
void alsound_update(void)
{
    int16_t i;
    ALCenum ret;
    float angle;
    int16_t yaw_corrected;
    event_t *entity;
    uint8_t idx = 0;

    if (!ale.initialized) {
        return;
    }

    ale.frame_cnt++;
    alGetError();               // reset global error variable

    for (i = OPENAL_C_SZ; i > 0; i--) {
        if (alc[i - 1].state == AL_PLAYING) {
            alGetSourcei(alc[i - 1].alSource, AL_SOURCE_STATE, &alc[i - 1].state);
            ret = alGetError();
            if (ret != AL_NO_ERROR) {
                Logger->error("error during alGetSourcei: {} for i {}", alsound_get_error_str(ret), i - 1);
            }
        } else if (alc[i - 1].state != 0) {
            alsound_delete_source(i - 1);
        }
    }

    // update the position and orientation of the listener
    // based on it's movement in game space
    alListener3f(AL_POSITION, ale.listener_c.x, ale.listener_c.y, ale.listener_c.z);
    alsound_error_check("alListener3f AL_POSITION");

    // the yaw needs to be corrected by 90 degrees in the xy plane
    yaw_corrected = ale.listener_o.yaw + 512;
    if (yaw_corrected > 2048) {
        yaw_corrected -= 2048;
    }

    angle = 2.0f * M_PI * yaw_corrected / 2048.0f;
    ALfloat orientation[] = { cos(angle), sin(angle), 0.0f, 0.0f, 0.0f, 1.0f };
    alListenerfv(AL_ORIENTATION, orientation);
    alsound_error_check("alListenerfv AL_ORIENTATION");

    // ignoring velocity for now
    alListener3f(AL_VELOCITY, 0, 0, 0);
    alsound_error_check("alListener3f AL_VELOCITY");

    // get all creature positions
    idx = 0;
    do {
        for (entity = x_D41A0_BYTEARRAY_4_struct.bytearray_38403x[idx]; entity > x_DWORD_EA3E4[0]; entity = entity->next_0) {
            if ((entity->class_0x3F_63 == 5) && (alcrt[entity->model_0x40_64].chunk_id != -1) && (ale.scheduling_enabled)) {
                alsound_update_source(entity);
            }
        }
        idx++;
    } while (idx < 29);
}

/// \brief cache chunk into the chunk cache array
/// \param cache_ch  empty index to be used
/// \param chunk_id  identifier
/// \param mixchunk  SDL2_mixer compatible struct holding chunk data
void alsound_cache(const int16_t cache_ch, const int16_t chunk_id, const Mix_Chunk *mixchunk, const uint16_t flags)
{
    ALCenum ret;

    // save chunk to disk for debug
    //alsound_save_chunk(mixchunk->abuf, mixchunk->alen, NULL);

    alGetError();               // reset global error variable
    alGenBuffers(1, &alcc[cache_ch].bufferName);
    alsound_error_check("alGenBuffers");
    if (flags & AL_FORMAT_STEREO8_22050) {
        alBufferData(alcc[cache_ch].bufferName, AL_FORMAT_STEREO8, mixchunk->abuf, mixchunk->alen, 22050);
    } else if (flags & AL_FORMAT_STEREO16_44100) {
        alBufferData(alcc[cache_ch].bufferName, AL_FORMAT_STEREO16, mixchunk->abuf, mixchunk->alen, 44100);
    } else {
        alBufferData(alcc[cache_ch].bufferName, AL_FORMAT_MONO8, mixchunk->abuf, mixchunk->alen, 22050);
    }
    alsound_error_check("alBufferData");
    ret = alGetError();

    if (ret == AL_NO_ERROR) {
        //Logger->info("alsound_cache        id {}  sz {}  cache_ch {}  flags {}", chunk_id, mixchunk->alen, cache_ch, flags);
        alcc[cache_ch].size = mixchunk->alen;
        alcc[cache_ch].id = chunk_id;
        alcc[cache_ch].flags |= OPENAL_FLG_LOADED;
    } else {
        Logger->error("alBufferData() failed: {}", alsound_get_error_str(ret));
        alcc[cache_ch] = { };
    }
}

/// \brief clear all elements of the chunk cache array, used during scene changes
void alsound_clear_cache(void)
{
    int16_t i;

    if (!ale.initialized) {
        return;
    }

    //Logger->info("alsound_clear_cache");
    for (i = OPENAL_C_SZ; i > 0; i--) {
        if (alc[i - 1].state == AL_PLAYING) {
            alSourceStop(alc[i - 1].alSource);
            alsound_error_check("clear_cache alSourceStop");
            alDeleteSources(1, &alc[i - 1].alSource);
            alsound_error_check("clear_cache alDeleteSources");
        } else if (alc[i - 1].state != 0) {
            alDeleteSources(1, &alc[i - 1].alSource);
            alsound_error_check("clear_cache alDeleteSources");
            alc[i - 1].state = 0;
            //Logger->info("freed {}", i);
        }
    }

    for (i = OPENAL_CC_SZ; i > 0; i--) {
        if (alcc[i - 1].flags & OPENAL_FLG_LOADED) {
            alDeleteBuffers(1, &alcc[i - 1].bufferName);
            alsound_error_check("clear_cache alDeleteBuffers");
        }
    }

    memset(alc, 0, sizeof(alc));
    memset(alcc, 0, sizeof(alcc));
    memset(al_con, 0, sizeof(al_con));
    ale.scheduling_enabled = 0;
}

/// \brief place a sound source at the current coordinates
/// \param chunk_id  sound sample to be played on a continuous loop
/// \param ssp   optional openal parameters to apply to the source
/// \return play channel index
int16_t alsound_create_source(const int16_t chunk_id, al_ssp_t *ssp, event_t *entity)
{
    Mix_Chunk mixchunk = { };
    al_ssp_t ssp_l = { };
    uint8_t *chunk_data = NULL;
    int32_t chunk_len;

    if (chunk_id < 0) {
        return -1;
    }

    get_sample_ptr((uint8_t) chunk_id, &chunk_data, &chunk_len);

    if (chunk_len < 1000) {
        Logger->error("alsound_create_source received invalid data");
        return -1;
    }

    mixchunk.abuf = chunk_data;
    mixchunk.alen = chunk_len;
    mixchunk.volume = 127;

    if (ssp == NULL) {
        ssp_l.gain = 1.0;
        ssp_l.reference_distance = 2048.0;
        ssp_l.max_distance = 65535.0;
        ssp_l.rolloff_factor = 1.0;
        ssp_l.coord.x = ale.listener_c.x;
        ssp_l.coord.y = ale.listener_c.y;
        ssp_l.coord.z = ale.listener_c.z;
        return alsound_play(chunk_id, &mixchunk, entity, &ssp_l, AL_FORMAT_MONO8_22050 | AL_TYPE_POSITIONAL);
    } else {
        return alsound_play(chunk_id, &mixchunk, entity, ssp, AL_FORMAT_MONO8_22050 | AL_TYPE_POSITIONAL);
        Logger->info("alsound_create_source {}  at ({},{},{})", mixchunk.alen, ssp->coord.x, ssp->coord.y, ssp->coord.z);
    }
}

/// \brief update entity openal source position
/// \param entity  
/// \param position   
void alsound_update_source(event_t *entity)
{
    al_ssp_t ssp = { };
    double dx, dy, dist;
    uint64_t now = mygetthousandths();
    uint8_t create_new_source = 0;

    // spread out the scheduled time when the distance needs to be refreshed
    if (entity->dist_mark == UINT64_MAX) {
        entity->dist_mark = now + entity->id_0x1A_26;
    }

    // once every AL_DIST_REFRESH_INTERVAL re-calculate the distance to the listener
    if (now > entity->dist_mark) {
        dx = ale.listener_c.x - entity->axis_0x4C_76.x;
        dy = ale.listener_c.y - entity->axis_0x4C_76.y;
        dist = sqrt((dx * dx) + (dy * dy));
        entity->dist = (uint32_t) dist;
        entity->dist_mark = now + AL_DIST_REFRESH_INTERVAL;
    }

    // once a while some of the creatures create sounds
    if ((now > entity->play_mark) && (alcrt[entity->model_0x40_64].chunk_id != -1)) {
        create_new_source = 1;
        if (alcrt[entity->model_0x40_64].flags & AL_REPLAY_RARELY) {
#ifdef _POSIX_C_SOURCE
            entity->play_mark = now + 5000 + (random() % 32768);
#else
            entity->play_mark = now + 5000 + (rand() % 32768);
#endif
        } else if (alcrt[entity->model_0x40_64].flags & AL_REPLAY_FREQUENTLY) {
            entity->play_mark = now;
        } else {
#ifdef _POSIX_C_SOURCE
            entity->play_mark = now + 5000 + (random() % 4098);
#else
            entity->play_mark = now + 5000 + (rand() % 4098);
#endif
        }
        //Logger->info("sch     {} @{} in {} ms", entity->id_0x1A_26, entity->play_mark, entity->play_mark - now);
    }

    if ((entity->dist < AL_DIST_MIN_PLAY) && (alcrt[entity->model_0x40_64].chunk_id != -1)) {
        if ((entity->play_ch == -1) && create_new_source) {
            ssp.gain = 0.8f;
            ssp.reference_distance = 2048.0;
            if (alcrt[entity->model_0x40_64].flags & AL_POWERFUL_SHOUT) {
                ssp.gain = 1.0f;
                ssp.reference_distance = 4096.0;
            } else if (alcrt[entity->model_0x40_64].flags & AL_WHISPER) {
                ssp.gain = 0.7f;
                ssp.reference_distance = 1024.0;
            }
            ssp.max_distance = 65535.0;
            ssp.rolloff_factor = 1.0;
            ssp.coord.x = entity->axis_0x4C_76.x;
            ssp.coord.y = entity->axis_0x4C_76.y;
            ssp.coord.z = entity->axis_0x4C_76.z;
            //Logger->info("play    {} id {} @{}", creature_name[entity->model_0x40_64], entity->id_0x1A_26, now);
            entity->play_ch = alsound_create_source(alcrt[entity->model_0x40_64].chunk_id, &ssp, entity);
        } else if (entity->play_ch != -1) {
            //Logger->info("update  {} @{}", entity->id_0x1A_26, now);
            alSource3f(alc[entity->play_ch].alSource, AL_POSITION, entity->axis_0x4C_76.x, entity->axis_0x4C_76.y, entity->axis_0x4C_76.z);
            //alsound_error_check("alSource3f update_source AL_POSITION");
        }
    }

}

/// \brief primary entrypoint for chunks that need to be played
/// \param chunk_id  chunk identifier
/// \param mixchunk  SDL2_mixer compatible struct holding chunk data
/// \param loops 0 for no looping, 0xffff for infinite loop
/// \param ssp   optional openal parameters to apply to the source. not used by the recode
/// \return play channel index
int16_t alsound_play(const int16_t chunk_id, Mix_Chunk *mixchunk, event_t *entity, al_ssp_t *ssp, const uint16_t flags)
{
    int16_t i;
    int16_t cache_ch = -1;
    int16_t play_ch = -1;
    float gain = 0.8;           // testing: all sounds produced by recode are at lowered levels

    if (!ale.initialized) {
        return -1;
    }

    //Logger->info("alsound_play requested id {}  sz {}  fmt {}", chunk_id, mixchunk->alen, flags);

    // check if sample is already playing too many times atm
    if (al_con[chunk_id] > oac.same_chunk_concurrency - 1) {
        //Logger->info("alsound_play ignored id {}", chunk_id);
        return -1;
    }

    if (ale.bank < 3) {
        if ((alct[ale.bank][chunk_id].flags & AL_IGNORE_RECODE) && !(flags & AL_TYPE_POSITIONAL) && 
        !((ale.listener_c.x == 0) && (ale.listener_c.y == 0) && (ale.listener_c.z == 0))) {
            return -1;
        }
        if (alct[ale.bank][chunk_id].ff_fx > -1) {
            haptic_run_effect(alct[ale.bank][chunk_id].ff_fx);
        }
    }

    // check if chunk has already been cached
    for (i = OPENAL_CC_SZ; i > 0; i--) {
        if (alcc[i - 1].id == chunk_id) {
            cache_ch = i - 1;
            if (((uint32_t) alcc[i - 1].size != mixchunk->alen) || (chunk_id == OPENAL_CC_SZ - 1)) {
                //Logger->warn("cache miss!  new {} cached {}  cache_ch {}", mixchunk->alen, alcc[i - 1].size, i - 1);
                // replace invalid cache slot
                alsound_cache(cache_ch, chunk_id, mixchunk, flags);
            }
            break;
        }
    }

    if (cache_ch == -1) {
        // find an empty cache slot
        for (i = OPENAL_CC_SZ; i > 0; i--) {
            if (alcc[i - 1].size == 0) {
                cache_ch = i - 1;
                break;
            }
        }
        if (cache_ch == -1) {
            Logger->error("openal cache is full");
            return -1;
        } else {
            // load chunk into empty cache slot
            alsound_cache(cache_ch, chunk_id, mixchunk, flags);
        }
    }

    // find an empty play slot
    for (i = OPENAL_C_SZ; i > 0; i--) {
        if (alc[i - 1].state == 0) {
            play_ch = i - 1;
            break;
        }
    }

    if (play_ch == -1) {
        Logger->warn("all channels are used, ignoring chunk");
        return -1;
    }

    //Logger->info("alsound_play playing   id {}  sz {}  cache_ch {}  play_ch {}", chunk_id, alcc[cache_ch].size, cache_ch, play_ch);

    if ((alnv.chunk_id == chunk_id) || ((uint32_t) alnv.chunk_id == mixchunk->alen)) {
        gain = alnv.gain;
        alnv.chunk_id = -1;     // apply the volume only once
    }

    if (flags & AL_TYPE_ENV) {
        gain = (float) oac.env_volume / 127.0f;
    } else if (flags & AL_TYPE_SPEECH) {
        gain = (float) oac.speech_volume / 127.0f;
    }

    alGetError();               // reset global error variable
    alGenSources((ALuint) 1, &alc[play_ch].alSource);
    alsound_error_check("alGenSources");

    alSourcef(alc[play_ch].alSource, AL_PITCH, 1);
    alsound_error_check("alSourcef AL_PITCH");
    alSource3f(alc[play_ch].alSource, AL_VELOCITY, 0, 0, 0);
    alsound_error_check("alSource3f AL_VELOCITY");

    if (!ssp) {
        // no placement received (sound comes from recode)
        alSourcef(alc[play_ch].alSource, AL_GAIN, gain);
        alsound_error_check("alSourcef AL_GAIN");
        alSourcef(alc[play_ch].alSource, AL_REFERENCE_DISTANCE, 512);
        alsound_error_check("alSourcef AL_REFERENCE_DISTANCE");
        alSourcef(alc[play_ch].alSource, AL_MAX_DISTANCE, 65535);
        alsound_error_check("alSourcef AL_MAX_DISTANCE");
        alSourcef(alc[play_ch].alSource, AL_ROLLOFF_FACTOR, 1.0);
        alsound_error_check("alSourcef AL_ROLLOFF_FACTOR");
        alSource3f(alc[play_ch].alSource, AL_POSITION, ale.listener_c.x, ale.listener_c.y, ale.listener_c.z);
        alsound_error_check("alSource3f listener AL_POSITION");
        //Logger->info("alsound_play source @({},{},{})", ale.listener_c.x, ale.listener_c.y, ale.listener_c.z);
    } else {
        alSourcef(alc[play_ch].alSource, AL_GAIN, ssp->gain);
        alsound_error_check("alSourcef AL_GAIN");
        alSourcef(alc[play_ch].alSource, AL_REFERENCE_DISTANCE, ssp->reference_distance);
        alsound_error_check("alSourcef AL_REFERENCE_DISTANCE");
        alSourcef(alc[play_ch].alSource, AL_MAX_DISTANCE, ssp->max_distance);
        alsound_error_check("alSourcef AL_MAX_DISTANCE");
        alSourcef(alc[play_ch].alSource, AL_ROLLOFF_FACTOR, ssp->rolloff_factor);
        alsound_error_check("alSourcef AL_ROLLOFF_FACTOR");
        alSource3f(alc[play_ch].alSource, AL_POSITION, ssp->coord.x, ssp->coord.y, ssp->coord.z);
        alsound_error_check("alSource3f alSource AL_POSITION");
        //Logger->info("alsound_play source @({},{},{})", ssp->coord.x, ssp->coord.y, ssp->coord.z);
    }

//    if (loops == 0xffff) {
//        alSourcei(alc[play_ch].alSource, AL_LOOPING, AL_TRUE);
//    } else {
    alSourcei(alc[play_ch].alSource, AL_LOOPING, AL_FALSE);
//    }
    alsound_error_check("alSourcei AL_LOOPING");

    alSourcei(alc[play_ch].alSource, AL_BUFFER, alcc[cache_ch].bufferName);
    alsound_error_check("alSourcei AL_BUFFER");

    if (ale.efx_initialized) {
        // assign effect
        EFXEAXREVERBPROPERTIES reverb;

        if (ale.bank != ale.reverb_type) {
            // re-initialize the effect to match the current environment
            alDeleteAuxiliaryEffectSlots(1, &al_slot);
            alDeleteEffects(1, &al_effect);

            switch (ale.bank) {
            case AL_BANK_MAP_DAY:
                reverb = EFX_REVERB_PRESET_GENERIC;
                break;
            case AL_BANK_MAP_NIGHT:
                reverb = EFX_REVERB_PRESET_GENERIC;
                break;
            case AL_BANK_MAP_CAVE:
                reverb = EFX_REVERB_PRESET_CAVE;
                break;
            case AL_BANK_MENU:
            default:
                reverb = EFX_REVERB_PRESET_GENERIC;
                break;
            }

            al_effect = alsound_load_effect(&reverb);
            if (!al_effect) {
                Logger->error("OpenAL: cannot load effect");
            } else {
                al_slot = 0;
                alGenAuxiliaryEffectSlots(1, &al_slot);
                alAuxiliaryEffectSloti(al_slot, AL_EFFECTSLOT_EFFECT, (ALint) al_effect);
                if (alGetError() == AL_NO_ERROR) {
                    Logger->trace("alsound_play switched to bank {}", ale.bank);
                }
            }
            ale.reverb_type = ale.bank;
        }

        alSource3i(alc[play_ch].alSource, AL_AUXILIARY_SEND_FILTER, (ALint) al_slot, 0, AL_FILTER_NULL);
        alsound_error_check("alSource3i AL_AUXILIARY_SEND_FILTER");
    }

    alSourcePlay(alc[play_ch].alSource);
    alsound_error_check("alSourcePlay");

    alc[play_ch].id = chunk_id;
    alc[play_ch].size = mixchunk->alen;

    alGetSourcei(alc[play_ch].alSource, AL_SOURCE_STATE, &alc[play_ch].state);
    alsound_error_check("alGetSourcei init");

    alc[play_ch].entity = entity;
    al_con[chunk_id]++;

    return play_ch;
}

/// \brief function that determines if a chunk needs to be received as MONO8 22050 or STEREO8 22050
/// \param chunk_id  chunk identifier
/// \return AL_FORMAT_STEREO8_22050, AL_FORMAT_MONO8_22050, AL_TYPE_ENV 
uint16_t alsound_get_chunk_flags(const int16_t chunk_id)
{
    int16_t ret;

    if ((ale.bank > AL_BANK_MENU) || (chunk_id > 69) || (chunk_id < 1)) {
        // not covered
        ret = 0;
    } else if (ale.bank == AL_BANK_MENU) {
        // sound during intro, boulder menu
        ret = AL_FORMAT_MONO8_22050;
    } else if (ale.bank < 3) {
        // sound in day/night/cave locations
        ret = alct[ale.bank][chunk_id].flags;
    }

    if (chunk_id == OPENAL_CC_SZ - 1) {
        ret = AL_FORMAT_STEREO16_44100;
    }

    return ret;
}

/// \brief load the given reverb properties into a new OpenAL effect
/// \param reverb  struct containing reverb parameters
/// \return an effect identifier or NULL on error
static ALuint alsound_load_effect(const EFXEAXREVERBPROPERTIES *reverb)
{
    ALuint effect = 0;
    ALenum err;

    alGetError();

    // Create the effect object and check if we can do EAX reverb
    alGenEffects(1, &effect);
    alsound_error_check("alGenEffects");
    if (alGetEnumValue("AL_EFFECT_EAXREVERB") != 0) {
        Logger->trace("OpenAL: Using EAX Reverb");

        /* EAX Reverb is available. Set the EAX effect type then load the
         * reverb properties. */
        alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_EAXREVERB);
        alsound_error_check("alEffecti");

        alEffectf(effect, AL_EAXREVERB_DENSITY, reverb->flDensity);
        alEffectf(effect, AL_EAXREVERB_DIFFUSION, reverb->flDiffusion);
        alEffectf(effect, AL_EAXREVERB_GAIN, reverb->flGain);
        alEffectf(effect, AL_EAXREVERB_GAINHF, reverb->flGainHF);
        alEffectf(effect, AL_EAXREVERB_GAINLF, reverb->flGainLF);
        alEffectf(effect, AL_EAXREVERB_DECAY_TIME, reverb->flDecayTime);
        alEffectf(effect, AL_EAXREVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
        alEffectf(effect, AL_EAXREVERB_DECAY_LFRATIO, reverb->flDecayLFRatio);
        alEffectf(effect, AL_EAXREVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
        alEffectf(effect, AL_EAXREVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
        alEffectfv(effect, AL_EAXREVERB_REFLECTIONS_PAN, reverb->flReflectionsPan);
        alEffectf(effect, AL_EAXREVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
        alEffectf(effect, AL_EAXREVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
        alEffectfv(effect, AL_EAXREVERB_LATE_REVERB_PAN, reverb->flLateReverbPan);
        alEffectf(effect, AL_EAXREVERB_ECHO_TIME, reverb->flEchoTime);
        alEffectf(effect, AL_EAXREVERB_ECHO_DEPTH, reverb->flEchoDepth);
        alEffectf(effect, AL_EAXREVERB_MODULATION_TIME, reverb->flModulationTime);
        alEffectf(effect, AL_EAXREVERB_MODULATION_DEPTH, reverb->flModulationDepth);
        alEffectf(effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
        alEffectf(effect, AL_EAXREVERB_HFREFERENCE, reverb->flHFReference);
        alEffectf(effect, AL_EAXREVERB_LFREFERENCE, reverb->flLFReference);
        alEffectf(effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
        alEffecti(effect, AL_EAXREVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
    } else {
        Logger->trace("OpenAL: Using Standard Reverb");

        /* No EAX Reverb. Set the standard reverb effect type then load the
         * available reverb properties. */
        alEffecti(effect, AL_EFFECT_TYPE, AL_EFFECT_REVERB);
        alsound_error_check("alEffecti");

        alEffectf(effect, AL_REVERB_DENSITY, reverb->flDensity);
        alEffectf(effect, AL_REVERB_DIFFUSION, reverb->flDiffusion);
        alEffectf(effect, AL_REVERB_GAIN, reverb->flGain);
        alEffectf(effect, AL_REVERB_GAINHF, reverb->flGainHF);
        alEffectf(effect, AL_REVERB_DECAY_TIME, reverb->flDecayTime);
        alEffectf(effect, AL_REVERB_DECAY_HFRATIO, reverb->flDecayHFRatio);
        alEffectf(effect, AL_REVERB_REFLECTIONS_GAIN, reverb->flReflectionsGain);
        alEffectf(effect, AL_REVERB_REFLECTIONS_DELAY, reverb->flReflectionsDelay);
        alEffectf(effect, AL_REVERB_LATE_REVERB_GAIN, reverb->flLateReverbGain);
        alEffectf(effect, AL_REVERB_LATE_REVERB_DELAY, reverb->flLateReverbDelay);
        alEffectf(effect, AL_REVERB_AIR_ABSORPTION_GAINHF, reverb->flAirAbsorptionGainHF);
        alEffectf(effect, AL_REVERB_ROOM_ROLLOFF_FACTOR, reverb->flRoomRolloffFactor);
        alEffecti(effect, AL_REVERB_DECAY_HFLIMIT, reverb->iDecayHFLimit);
    }

    // Check if an error occured, and clean up if so
    err = alGetError();
    if (err != AL_NO_ERROR) {
        Logger->error("OpenAL error: {}", alGetString(err));
        if (alIsEffect(effect)) {
            alDeleteEffects(1, &effect);
        }
        return 0;
    }

    return effect;
}

/// \brief enable scheduling of chunks and thus playing positional audio 
void alsound_enable_scheduling(void)
{
    ale.scheduling_enabled = 1;
}

/// \brief close down OpenAL, to be used only on program exit
void alsound_close(void)
{
    int16_t i;
    ALCdevice *device;

    for (i = OPENAL_C_SZ; i > 0; i--) {
        alDeleteSources(1, &alc[i - 1].alSource);
    }

    for (i = OPENAL_CC_SZ; i > 0; i--) {
        if (alcc[i - 1].flags & OPENAL_FLG_LOADED) {
            alDeleteBuffers(1, &alcc[i - 1].bufferName);
        }
    }

    alDeleteAuxiliaryEffectSlots(1, &al_slot);
    alDeleteEffects(1, &al_effect);
    device = alcGetContextsDevice(context);

    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

#ifdef _POSIX_C_SOURCE
/// \brief save chunk data to a file for debug purposes
/// \param data  chunk's uint16 wav data
/// \param len   chunk length in bytes
uint8_t alsound_save_chunk(uint8_t *data, const uint32_t len, char *filename)
{
    char fname[] = "/tmp/remc_XXXXXX";
    int fd;

    umask(077);

    if (filename == NULL) {
        fd = mkstemp(fname);
    } else {
        fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    }

    if (fd < 0) {
        return EXIT_FAILURE;
    }

    if (write(fd, data, len) != len) {
        Logger->error("write error: {}", strerror(errno));
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}
#endif

/// \brief print human-readable OpenAL error codes
/// \param error  OpenAL error code
/// \return string containing the error type
const char *alsound_get_error_str(ALCenum error)
{
    switch (error) {
    case AL_NO_ERROR:
        return "AL_NO_ERROR";
    case AL_INVALID_NAME:
        return "AL_INVALID_NAME";
    case AL_INVALID_ENUM:
        return "AL_INVALID_ENUM";
    case AL_INVALID_VALUE:
        return "AL_INVALID_VALUE";
    case AL_INVALID_OPERATION:
        return "AL_INVALID_OPERATION";
    case AL_OUT_OF_MEMORY:
        return "AL_OUT_OF_MEMORY";
    }
    return "AL_INVALID_OPERATION";
}

/// \brief function that is run after an OpenAL function to pick up the error set in a special global variable
/// \param msg  string to be displayed in case of non-successful operation of the preceding function
/// \return the error code for further decision-making
ALCenum alsound_error_check(const char *msg)
{
    ALCenum ret;

    ret = alGetError();
    if (ret != AL_NO_ERROR) {
        Logger->error("error during {}: {}", msg, alsound_get_error_str(ret));
        return ret;
    }
    return AL_NO_ERROR;
}


