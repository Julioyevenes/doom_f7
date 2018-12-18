//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:  none
//

#include <stdio.h>
#include <stdlib.h>

#ifdef ORIGCODE
#include "SDL_mixer.h"
#endif

#include "config.h"
#include "doomfeatures.h"
#include "doomtype.h"

#ifdef ORIGCODE
#include "gusconf.h"
#endif
#include "i_sound.h"
#include "i_video.h"
#include "m_argv.h"
#include "m_config.h"

// Sound sample rate to use for digital output (Hz)

int snd_samplerate = 44100;

// Maximum number of bytes to dedicate to allocated sound effects.
// (Default: 64MB)

int snd_cachesize = 64 * 1024 * 1024;

// Config variable that controls the sound buffer size.
// We default to 28ms (1000 / 35fps = 1 buffer per tic).

int snd_maxslicetime_ms = 28;

// External command to invoke to play back music.

char *snd_musiccmd = "";

// Low-level sound and music modules we are using

static sound_module_t *sound_module;
static music_module_t *music_module;

int snd_musicdevice = SNDDEVICE_SB;
int snd_sfxdevice = SNDDEVICE_SB;

// Sound modules

extern void I_InitTimidityConfig(void);
extern sound_module_t sound_sdl_module;
extern sound_module_t sound_pcsound_module;
extern music_module_t music_sdl_module;
extern music_module_t music_opl_module;

// For OPL module:

extern int opl_io_port;

// For native music module:

extern char *timidity_cfg_path;

// DOS-specific options: These are unused but should be maintained
// so that the config file can be shared between chocolate
// doom and doom.exe

#if ORIGCODE
static int snd_sbport = 0;
static int snd_sbirq = 0;
static int snd_sbdma = 0;
static int snd_mport = 0;
#endif

// Compiled-in sound modules:

static sound_module_t *sound_modules[] = 
{
#ifdef FEATURE_SOUND
    &sound_sdl_module,
    &sound_pcsound_module,
#endif
    NULL,
};

// Compiled-in music modules:

static music_module_t *music_modules[] =
{
#ifdef FEATURE_SOUND
    &music_sdl_module,
    &music_opl_module,
#endif
    NULL,
};

#include "deh_str.h"
#include "z_zone.h"
#include "stm32f769i_discovery_audio.h"

typedef struct _ExpandedSoundType
{
	uint32_t *lump;
	uint32_t len;
	uint32_t pos;
} ExpandedSoundType;

#define NUM_CHANNELS 8
#define AUDIO_BUFFER_SIZE 1024

uint8_t audio_buffer[AUDIO_BUFFER_SIZE] = {0};

static ExpandedSoundType expanded_sound[NUM_CHANNELS];

static boolean s_initialized = false;
static boolean sfx_prefix;

static uint8_t *current_sound_lump = NULL;
static int current_sound_lump_num = -1;

void U8M_to_S16S_BuffConv(uint8_t *inbuff, uint16_t inbuffSize, uint32_t *outbuff)
{
	uint16_t i;

	for(i = 0; i < inbuffSize ; i++)
	{
		*outbuff = 	((*inbuff - 0x80) << 24) + \
					((*inbuff - 0x80) << 16) + \
					((*inbuff - 0x80) << 8)  + \
					(*inbuff - 0x80);

		outbuff++;
		inbuff++;
	}
}

void MixAudio(uint8_t *dst, const uint8_t *src, uint32_t len)
{
	int16_t src1, src2;
	int dst_sample;
	const int max_audioval = ((1<<(16-1))-1);
	const int min_audioval = -(1<<(16-1));

	len /= 2;
	while ( len-- )
	{
		src1 = ((src[1])<<8|src[0]);
		src2 = ((dst[1])<<8|dst[0]);
		src += 2;
		dst_sample = (src1 + src2) / 2;
		if ( dst_sample > max_audioval )
		{
			dst_sample = max_audioval;
		}
		else if ( dst_sample < min_audioval )
		{
			dst_sample = min_audioval;
		}
		dst[0] = dst_sample&0xFF;
		dst_sample >>= 8;
		dst[1] = dst_sample&0xFF;
		dst += 2;
	}
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	uint8_t i, channels_playing = 0;

	for(i = 0 ; i < NUM_CHANNELS ; i++)
	{
		if(expanded_sound[i].pos < expanded_sound[i].len)
		{
			MixAudio( &audio_buffer[AUDIO_BUFFER_SIZE / 2],
				  expanded_sound[i].lump + \
				  (expanded_sound[i].pos / 4),
				  AUDIO_BUFFER_SIZE / 2 );

			expanded_sound[i].pos += AUDIO_BUFFER_SIZE / 2;

			channels_playing++;
		}
	}

	if(channels_playing == 0)
	{
		memset(	&audio_buffer[AUDIO_BUFFER_SIZE / 2],
			0,
			AUDIO_BUFFER_SIZE / 2 );
	}
}

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{
	uint8_t i, channels_playing = 0;

	for(i = 0 ; i < NUM_CHANNELS ; i++)
	{
		if(expanded_sound[i].pos < expanded_sound[i].len)
		{
			MixAudio( &audio_buffer[0],
				  expanded_sound[i].lump + \
				  (expanded_sound[i].pos / 4),
				  AUDIO_BUFFER_SIZE / 2 );

			expanded_sound[i].pos += AUDIO_BUFFER_SIZE / 2;

			channels_playing++;
		}
	}

	if(channels_playing == 0)
	{
		memset(	&audio_buffer[0],
			0,
			AUDIO_BUFFER_SIZE / 2 );
	}
}

// Check if a sound device is in the given list of devices

static boolean SndDeviceInList(snddevice_t device, snddevice_t *list,
                               int len)
{
    int i;

    for (i=0; i<len; ++i)
    {
        if (device == list[i])
        {
            return true;
        }
    }

    return false;
}

// Find and initialize a sound_module_t appropriate for the setting
// in snd_sfxdevice.

static void InitSfxModule(boolean use_sfx_prefix)
{
    int i;

    sound_module = NULL;

    for (i=0; sound_modules[i] != NULL; ++i)
    {
        // Is the sfx device in the list of devices supported by
        // this module?

        if (SndDeviceInList(snd_sfxdevice, 
                            sound_modules[i]->sound_devices,
                            sound_modules[i]->num_sound_devices))
        {
            // Initialize the module

            if (sound_modules[i]->Init(use_sfx_prefix))
            {
                sound_module = sound_modules[i];
                return;
            }
        }
    }
}

// Initialize music according to snd_musicdevice.

static void InitMusicModule(void)
{
    int i;

    music_module = NULL;

    for (i=0; music_modules[i] != NULL; ++i)
    {
        // Is the music device in the list of devices supported
        // by this module?

        if (SndDeviceInList(snd_musicdevice, 
                            music_modules[i]->sound_devices,
                            music_modules[i]->num_sound_devices))
        {
            // Initialize the module

            if (music_modules[i]->Init())
            {
                music_module = music_modules[i];
                return;
            }
        }
    }
}

//
// Initializes sound stuff, including volume
// Sets channels, SFX and music volume,
//  allocates channel buffer, sets S_sfx lookup.
//

void I_InitSound(boolean use_sfx_prefix)
{
#if ORIGCODE
    boolean nosound, nosfx, nomusic;

    //!
    // @vanilla
    //
    // Disable all sound output.
    //

    nosound = M_CheckParm("-nosound") > 0;

    //!
    // @vanilla
    //
    // Disable sound effects. 
    //

    nosfx = M_CheckParm("-nosfx") > 0;

    //!
    // @vanilla
    //
    // Disable music.
    //

    nomusic = M_CheckParm("-nomusic") > 0;

    // Initialize the sound and music subsystems.

    if (!nosound && !screensaver_mode)
    {
        // This is kind of a hack. If native MIDI is enabled, set up
        // the TIMIDITY_CFG environment variable here before SDL_mixer
        // is opened.

        if (!nomusic
         && (snd_musicdevice == SNDDEVICE_GENMIDI
          || snd_musicdevice == SNDDEVICE_GUS))
        {
            I_InitTimidityConfig();
        }

        if (!nosfx)
        {
            InitSfxModule(use_sfx_prefix);
        }

        if (!nomusic)
        {
            InitMusicModule();
        }
    }
#endif
	sfx_prefix = use_sfx_prefix;

	if( BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, 20, 11025) == 0 )
	{
		BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);

		BSP_AUDIO_OUT_Play((uint16_t*)&audio_buffer, AUDIO_BUFFER_SIZE);

		s_initialized = true;
	}
	else
	{
		s_initialized = false;
	}
}

void I_ShutdownSound(void)
{
    if (sound_module != NULL)
    {
        sound_module->Shutdown();
    }

    if (music_module != NULL)
    {
        music_module->Shutdown();
    }
}

int I_GetSfxLumpNum(sfxinfo_t *sfxinfo)
{
#if ORIGCODE
    if (sound_module != NULL) 
    {
        return sound_module->GetSfxLumpNum(sfxinfo);
    }
    else
    {
        return 0;
    }
#endif
    char namebuf[9];

    if (sfxinfo->link != NULL)
    {
        sfxinfo = sfxinfo->link;
    }

    if (sfx_prefix)
    {
        M_snprintf(namebuf, sizeof(namebuf), "ds%s", DEH_String(sfxinfo->name));
    }
    else
    {
        M_StringCopy(namebuf, DEH_String(sfxinfo->name), sizeof(namebuf));
    }

    return W_GetNumForName(namebuf);
}

void I_UpdateSound(void)
{
    if (sound_module != NULL)
    {
        sound_module->Update();
    }

    if (music_module != NULL && music_module->Poll != NULL)
    {
        music_module->Poll();
    }
}

static void CheckVolumeSeparation(int *vol, int *sep)
{
    if (*sep < 0)
    {
        *sep = 0;
    }
    else if (*sep > 254)
    {
        *sep = 254;
    }

    if (*vol < 0)
    {
        *vol = 0;
    }
    else if (*vol > 127)
    {
        *vol = 127;
    }
}

void I_UpdateSoundParams(int channel, int vol, int sep)
{
    if (sound_module != NULL)
    {
        CheckVolumeSeparation(&vol, &sep);
        sound_module->UpdateSoundParams(channel, vol, sep);
    }
}

int I_StartSound(sfxinfo_t *sfxinfo, int channel, int vol, int sep)
{
#if ORIGCODE
    if (sound_module != NULL)
    {
        CheckVolumeSeparation(&vol, &sep);
        return sound_module->StartSound(sfxinfo, channel, vol, sep);
    }
    else
    {
        return 0;
    }
#endif
	unsigned int lumplen;
	int samplerate;
	unsigned int length;

	if (!s_initialized)
	{
		return -1;
	}

	if (channel > NUM_CHANNELS)
	{
		return -1;
	}

	current_sound_lump_num = sfxinfo->lumpnum;
	current_sound_lump = W_CacheLumpNum(sfxinfo->lumpnum, PU_STATIC);
	lumplen = W_LumpLength(sfxinfo->lumpnum);

	if (lumplen < 8 || current_sound_lump[0] != 0x03 || current_sound_lump[1] != 0x00)
	{
		return -1;
	}

	samplerate = (current_sound_lump[3] << 8) | current_sound_lump[2];
	length = (current_sound_lump[7] << 24) | (current_sound_lump[6] << 16) | (current_sound_lump[5] << 8) | current_sound_lump[4];

	if (length > lumplen - 8 || length <= 48)
	{
		return -1;
	}

	current_sound_lump += 16;
	length -= 32;

	expanded_sound[channel].lump = (uint32_t *) malloc(length * 4);

	U8M_to_S16S_BuffConv(current_sound_lump, length, expanded_sound[channel].lump);

	if (current_sound_lump != NULL)
	{
		W_ReleaseLumpNum(current_sound_lump_num);
		current_sound_lump = NULL;
	}

	expanded_sound[channel].len = length * 4;
	expanded_sound[channel].pos = 0;

	return channel;
}

void I_StopSound(int channel)
{
#if ORIGCODE
    if (sound_module != NULL)
    {
        sound_module->StopSound(channel);
    }
#endif
	if (!s_initialized)
	{
		return;
	}

	if (expanded_sound[channel].lump != NULL)
	{
		free(expanded_sound[channel].lump);
		expanded_sound[channel].lump = NULL;
		expanded_sound[channel].len = 0;
		expanded_sound[channel].pos = 0;
	}
}

boolean I_SoundIsPlaying(int channel)
{
#if ORIGCODE
    if (sound_module != NULL)
    {
        return sound_module->SoundIsPlaying(channel);
    }
    else
    {
        return false;
    }
#endif
	if (!s_initialized)
	{
    		return false;
	}

	return expanded_sound[channel].lump != NULL;
}

void I_PrecacheSounds(sfxinfo_t *sounds, int num_sounds)
{
    if (sound_module != NULL && sound_module->CacheSounds != NULL)
    {
	sound_module->CacheSounds(sounds, num_sounds);
    }
}

void I_InitMusic(void)
{
}

void I_ShutdownMusic(void)
{

}

void I_SetMusicVolume(int volume)
{
    if (music_module != NULL)
    {
        music_module->SetMusicVolume(volume);
    }
}

void I_PauseSong(void)
{
    if (music_module != NULL)
    {
        music_module->PauseMusic();
    }
}

void I_ResumeSong(void)
{
    if (music_module != NULL)
    {
        music_module->ResumeMusic();
    }
}

void *I_RegisterSong(void *data, int len)
{
    if (music_module != NULL)
    {
        return music_module->RegisterSong(data, len);
    }
    else
    {
        return NULL;
    }
}

void I_UnRegisterSong(void *handle)
{
    if (music_module != NULL)
    {
        music_module->UnRegisterSong(handle);
    }
}

void I_PlaySong(void *handle, boolean looping)
{
    if (music_module != NULL)
    {
        music_module->PlaySong(handle, looping);
    }
}

void I_StopSong(void)
{
    if (music_module != NULL)
    {
        music_module->StopSong();
    }
}

boolean I_MusicIsPlaying(void)
{
    if (music_module != NULL)
    {
        return music_module->MusicIsPlaying();
    }
    else
    {
        return false;
    }
}

void I_BindSoundVariables(void)
{
#ifdef ORIGCODE
    extern int use_libsamplerate;
    extern float libsamplerate_scale;

    M_BindVariable("snd_musicdevice",   &snd_musicdevice);
    M_BindVariable("snd_sfxdevice",     &snd_sfxdevice);
    M_BindVariable("snd_sbport",        &snd_sbport);
    M_BindVariable("snd_sbirq",         &snd_sbirq);
    M_BindVariable("snd_sbdma",         &snd_sbdma);
    M_BindVariable("snd_mport",         &snd_mport);
    M_BindVariable("snd_maxslicetime_ms", &snd_maxslicetime_ms);
    M_BindVariable("snd_musiccmd",      &snd_musiccmd);
    M_BindVariable("snd_samplerate",    &snd_samplerate);
    M_BindVariable("snd_cachesize",     &snd_cachesize);
    M_BindVariable("opl_io_port",       &opl_io_port);

    M_BindVariable("timidity_cfg_path", &timidity_cfg_path);
    M_BindVariable("gus_patch_path",    &gus_patch_path);
    M_BindVariable("gus_ram_kb",        &gus_ram_kb);

#ifdef FEATURE_SOUND
    M_BindVariable("use_libsamplerate",   &use_libsamplerate);
    M_BindVariable("libsamplerate_scale", &libsamplerate_scale);
#endif

    // Before SDL_mixer version 1.2.11, MIDI music caused the game
    // to crash when it looped.  If this is an old SDL_mixer version,
    // disable MIDI.

#ifdef __MACOSX__
    {
        const SDL_version *v = Mix_Linked_Version();

        if (SDL_VERSIONNUM(v->major, v->minor, v->patch)
          < SDL_VERSIONNUM(1, 2, 11))
        {
            snd_musicdevice = SNDDEVICE_NONE;
        }
    }
#endif
#endif
}

