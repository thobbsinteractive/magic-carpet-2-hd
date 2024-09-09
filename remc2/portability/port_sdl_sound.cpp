#include "port_sdl_sound.h"

#ifdef __linux__
#include <limits>
#define MAX_PATH PATH_MAX
#endif

std::function<void(int)> channelFinishedCallBack;

port_sdl_sound::port_sdl_sound()
{

}

port_sdl_sound::port_sdl_sound(bool hqsound, 
	bool fixspeedsound, 
	bool oggmusic, 
	bool oggmusicalternative, 
	std::string oggmusicFolder, 
	std::string speech_folder):
	m_hqsound(hqsound),
	m_fixspeedsound(fixspeedsound),
	m_oggmusic(oggmusic),
	m_oggmusicalternative(oggmusicalternative),
	m_oggmusicFolder(oggmusicFolder),
	m_speech_folder(speech_folder)
{
	InitSound();
}

port_sdl_sound::~port_sdl_sound()
{
	CleanUpSound();
}

void port_sdl_sound::StartSequence(int32_t sequence_num)
{
    if (unitTests)
        return;
    //3 - menu
    //4 - intro

	m_last_sequence_num = sequence_num;
    //volume fix
    if (m_lastMusicVolume == -1) {
        SetSequenceVolume(0x64, 0);
    }
    if (m_lastMusicVolume != m_settingsMusicVolume) {
        SetSequenceVolume(m_settingsMusicVolume, 0);
    }
    //volume fix
    if (Mix_PlayingMusic() == 0) {
        if (Mix_PlayMusic(m_GAME_music[sequence_num], -1) == -1)
            if (Mix_PausedMusic() == 1) {
                Mix_ResumeMusic();
            } else {
                Mix_PauseMusic();
            }
    }
};

void port_sdl_sound::PauseSequence(int32_t /*sequence_num */ )
{
    if (unitTests)
        return;

    Mix_PauseMusic();
};

void port_sdl_sound::StopSequence(int32_t /*sequence_num */ )
{
    if (unitTests)
        return;

    Mix_HaltMusic();
};

void port_sdl_sound::ResumeSequence(int32_t /*sequence_num */ )
{
    if (unitTests)
        return;

    Mix_ResumeMusic();
};

void port_sdl_sound::SetSequenceVolume(int32_t volume, int32_t milliseconds)
{
    if (unitTests)
        return;
    Logger->trace("SetSequenceVolume  vol {}  ms {}", volume, milliseconds);

#ifndef __linux__
    if ((milliseconds > 0) && (volume == 0)) {
        if (m_GAME_music[m_last_sequence_num]) {
            double position = Mix_GetMusicPosition(m_GAME_music[m_last_sequence_num]);
            if (position != 0) {
                Mix_FadeOutMusic(milliseconds);
                Mix_SetMusicPosition(position);
            }
        }
    } else if ((milliseconds > 0) && (m_lastMusicVolume == 0)) {
        if (m_GAME_music[m_last_sequence_num]) {
            double position = Mix_GetMusicPosition(m_GAME_music[m_last_sequence_num]);
            if (position != 0) {
                Mix_FadeInMusicPos(m_GAME_music[m_last_sequence_num], 1, milliseconds, position);
            }
        }
    } else
#endif                          //__linux__
        Mix_VolumeMusic(volume);
	m_lastMusicVolume = volume;
    if (milliseconds == 0)
		m_settingsMusicVolume = volume;
}

void port_sdl_sound::InitMIDISequence(uint8_t * /*datax */ , type_E3808_music_header *headerx, int32_t track_number)
{
    Logger->trace("InitMIDISequence {}", track_number);
    if (unitTests)
        return;
    //uint8_t* acttrack = &header[32 + track_number * 32];
    uint8_t *acttrack = headerx->str_8.track_10[track_number].xmiData_0;
    //int testsize = *(uint32_t*)(&header[32 + (track_number + 1) * 32] + 18) - *(uint32_t*)(acttrack + 18);
    int testsize2 = *(uint32_t *) (acttrack + 26);

    //we can translate datax from xmi to mid and play(with bad quality or slow midi emulators), at now but we use ogg samples
    //unsigned char* TranscodeXmiToMid(const unsigned char* pXmiData,       size_t iXmiLength, size_t* pMidLength);
    size_t iXmiLength = testsize2; // FIXME this size is completely bogus - between 0.8 and 1.2TB!!!
    size_t pMidLength;
    dirsstruct helpdirsstruct;

    if (m_oggmusic) {

        std::string oggmusicPath = GetSubDirectoryPath(m_oggmusicFolder.c_str());
        char alternativeMusicPath[512] = "";
        char selectedTrackPath[512] = "";
        if (m_oggmusicalternative)
        {
            if (track_number == 0) {
                sprintf(alternativeMusicPath, "%s/alternative/day", oggmusicPath.c_str());
            } else if (track_number == 1) {
                sprintf(alternativeMusicPath, "%s/alternative/night", oggmusicPath.c_str());
            } else if (track_number == 2) {
                sprintf(alternativeMusicPath, "%s/alternative/cave", oggmusicPath.c_str());
            } else if (track_number == 3) {
                sprintf(alternativeMusicPath, "%s/alternative/cave", oggmusicPath.c_str());
            } else if (track_number == 4) {
                sprintf(alternativeMusicPath, "%s/alternative/cave", oggmusicPath.c_str());
            } else if (track_number == 5) {
                sprintf(alternativeMusicPath, "%s/alternative/cave", oggmusicPath.c_str());
            } else {
                sprintf(alternativeMusicPath, "%s/alternative/cave", oggmusicPath.c_str());
            }

            helpdirsstruct = getListDir(alternativeMusicPath);

            if (helpdirsstruct.number > 0) {
                int randtrack = rand() % (helpdirsstruct.number + 1);
                if (randtrack == 0)
                    sprintf(selectedTrackPath, "%s/music%d.ogg", oggmusicPath.c_str(), track_number);
                else
                    sprintf(selectedTrackPath, "%s/%s", alternativeMusicPath, helpdirsstruct.dir[randtrack - 1]);
            } else
                sprintf(selectedTrackPath, "%s/music%d.ogg", oggmusicPath.c_str(), track_number);
        } else
			sprintf(selectedTrackPath, "%s/music%d.ogg", oggmusicPath.c_str(), track_number);

		m_GAME_music[track_number] = Mix_LoadMUS(selectedTrackPath);
		const char* error = SDL_GetError();

    } else {
        uint8_t *outmidi = TranscodeXmiToMid( /*(const uint8_t*)*(uint32_t*)( */ acttrack /* + 18) */ , iXmiLength, &pMidLength);
        SDL_RWops *rwmidi = SDL_RWFromMem(outmidi, pMidLength);
        Logger->trace("InitMIDISequence  xmi {}  mid {}", iXmiLength, pMidLength);

		m_GAME_music[track_number] = Mix_LoadMUSType_RW(rwmidi, MUS_MID, SDL_TRUE);
    }
}

void port_sdl_sound::StartSpeech(const uint8_t track, const uint16_t offset, const uint16_t len)
{
	StartSpeech(track, offset, len, nullptr);
}

void port_sdl_sound::StartSpeech(const uint8_t track, const uint16_t offset, const uint16_t len, std::function<void(int16_t chunkId, uint16_t flags)> sampleEndedEventHandler)
{
	char trackNum[3];
    uint8_t *track_data = nullptr;
    uint32_t track_data_len;
    uint32_t track_offset;
    int fd;

	Logger->debug("StartSpeech track {}  offset {}  len {}", track, offset, len);

	snprintf(trackNum, sizeof(trackNum), "%02d", track);

	std::string speech_path = GetSubDirectoryPath(m_speech_folder.c_str());
	std::string track_filename = speech_path.c_str() + std::string("/track") + trackNum + ".cdr";

	if (!std::filesystem::exists(track_filename))
		track_filename = speech_path.c_str() + std::string("/track") + trackNum + ".wav";

	if (!std::filesystem::exists(track_filename))
	{
		return;
	}

    track_data_len = len * 2360;
    track_offset = offset * 2360;
    track_data = (uint8_t *) calloc(track_data_len, sizeof(uint8_t));
    if (!track_data) {
        return;
    }

    if ((fd = open(track_filename.c_str(), O_RDONLY)) < 0) {
        Logger->warn("unable to open speech file {}", track_filename);
        return;
    }

    if (lseek(fd, track_offset, SEEK_SET) != track_offset) {
        Logger->warn("wrong offset {} in speech file {}", track_offset, track_filename);
        goto cleanup;
    }

    if (read(fd, track_data, track_data_len) == -1) {
        Logger->warn("can't read speech file {}", track_filename);
        goto cleanup;
    }

	m_gamechunk[31].allocated = 1;
	m_gamechunk[31].alen = track_data_len;
	m_gamechunk[31].abuf = track_data;
	m_gamechunk[31].volume = 127;

    Mix_PlayChannel(31, &m_gamechunk[31], 0);

cleanup: 
    close(fd);
}

void port_sdl_sound::CleanUpSound()
{
    if (unitTests)
        return;

    for (int i = 0; i < 10; i++) {
        Mix_FreeMusic(m_GAME_music[i]);
    }

    Mix_CloseAudio();
}

int32_t port_sdl_sound::ac_sound_call_driver(AIL_DRIVER *drvr, int32_t fn, VDI_CALL *out)
{
    switch (fn) {
    case 0x300:{               //AIL_API_install_driver
            drvr->VHDR_4->VDI_HDR_var10 = (void *)&common_IO_configurations;
            drvr->VHDR_4->num_IO_configurations_14 = m_num_IO_configurations;
            drvr->VHDR_4->environment_string_16 = environment_string.a;
            drvr->VHDR_4->VDI_HDR_var46 = m_service_rate;
            break;
        }
    case 0x301:{               //AIL_API_install_DIG_driver_file/AIL_API_install_MDI_driver_file          
            out->AX = 1;        //offset
            out->BX = 2;        //offset
            out->SI = 0;
            out->DI = 0;
            break;
        }
    case 0x304:{               //AIL_API_install_DIG_driver_file/AIL_API_install_MDI_driver_file
            break;
        }
    case 0x305:{               //AIL_API_install_DIG_driver_file/AIL_API_install_MDI_driver_file
            break;
        }
    case 0x306:{               //AIL_API_uninstall_driver/AIL_API_uninstall_MDI_driver_file
            break;
        }
    case 0x401:{
            /*              mychunk.abuf=(uint8_t*)last_sample->start_2_3[0];
               mychunk.alen = last_sample->len_4_5[0];
               mychunk.volume = last_sample->volume_16;
               //mychunk.allocated = 0;

               #ifdef USE_SDL2
               Mix_PlayChannel(-1, &mychunk, 0);
               #else
               Mix_PlayChannel(-1, &mychunk, 0);
               #endif */
            break;
        }
    case 0x501:{               //AIL_API_install_MDI_INI
            break;
        }
    case 0x502:{               //AIL_API_install_MDI_INI
            break;
        }
    default:
        {
            break;
        }
    }
    return 1;
};

void port_sdl_sound::InitSample(HSAMPLE S) 
{
	//test mark
	bool same_mark = true;
	int comparesize = sample_mark;
	if (comparesize > S->len_4_5[0])comparesize = S->len_4_5[0];
	for (int i = 0; i < comparesize; i++)
	{
		if (S->mark44mark[i] != ((uint8_t*)S->start_2_3[0])[i])
		{
			same_mark = false;
			break;
		}
	}
	//test mark

	if (!same_mark)
	{
		for (int i = 0; i < comparesize; i++)
		{
			S->mark44mark[i] = ((uint8_t*)S->start_2_3[0])[i];
		}

		if (S->start_44mhz != nullptr) {
			free(S->start_44mhz);
			S->start_44mhz = nullptr;
		}
		if (m_fixspeedsound)
			S->start_44mhz = malloc(S->len_4_5[0] * 2 * 2 * 2 * 2);
		else
			S->start_44mhz = malloc(S->len_4_5[0] * 2 * 2 * 2);

		uint16_t lastval = ((uint8_t*)S->start_2_3[0])[0] * 256;
		uint16_t actval;
		int16_t val1, val3;
		for (int i = 0; i < S->len_4_5[0]; i++)
		{
			actval = ((uint8_t*)S->start_2_3[0])[i] * 256;
			val1 = lastval - 0x8000;
			val3 = (lastval * 0.5 + actval * 0.5) - 0x8000;
			if (m_fixspeedsound) {
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[0 + i * 16]) = val1;
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[2 + i * 16]) = val1;
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[4 + i * 16]) = val1;
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[6 + i * 16]) = val1;
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[8 + i * 16]) = val3;
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[10 + i * 16]) = val3;
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[12 + i * 16]) = val3;
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[14 + i * 16]) = val3;
			}
			else {
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[0 + i * 8]) = val1;
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[2 + i * 8]) = val1;
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[4 + i * 8]) = val3;
				(*(int16_t*)&((uint8_t*)S->start_44mhz)[6 + i * 8]) = val3;
			}
			if (i < S->len_4_5[0] + 1)lastval = actval;
		}
		lastval = actval;
	}
}

void port_sdl_sound::SetLocation(axis_3d* coord, axis_4d* orient)
{
	//Not implemented
}

void port_sdl_sound::EnableScheduling(void)
{
	//Not implemented
}

void port_sdl_sound::SetMasterVolume(int32_t volume)
{
	m_master_volume = volume;

    for (int i = 0; i < 32; i++)
        Mix_Volume(i, (int)((m_gamechunk[i].volume * m_master_volume) / 127));
}

void port_sdl_sound::SetSampleVolume(HSAMPLE S, int32_t volume)
{
    if (m_master_volume == -1)
		m_master_volume = 127;
	m_gamechunk[S->index_sample].volume = volume;
    Mix_Volume(S->index_sample, (int)((m_gamechunk[S->index_sample].volume * m_master_volume) / 127));
}

void port_sdl_sound::StartSample(HSAMPLE S)
{
    if (unitTests)
        return;

    if (m_hqsound) 
	{
		m_gamechunk[S->index_sample].abuf = /*sample->abuf;// */ (uint8_t *) S->start_44mhz;
        if (m_fixspeedsound)
			m_gamechunk[S->index_sample].alen = /*sample->alen;// */ S->len_4_5[0] * 16;
        else
			m_gamechunk[S->index_sample].alen = /*sample->alen;// */ S->len_4_5[0] * 8;
		Logger->trace("StartSample-hq:{}", S->start_44mhz);
    } 
	else 
	{
        Logger->trace("StartSample:{}", S->start_44mhz);
		m_gamechunk[S->index_sample].abuf = (uint8_t *) S->start_2_3[0];
		m_gamechunk[S->index_sample].alen = S->len_4_5[0];
    }

	m_gamechunk[S->index_sample].volume = S->volume_16;
	m_gamechunkHSAMPLE[S->index_sample] = S;

    Mix_PlayChannel(S->index_sample, &m_gamechunk[S->index_sample], 0);
};

uint32_t port_sdl_sound::SampleStatus(HSAMPLE S)
{
    if (unitTests)
        return 0;

    if (Mix_Playing(S->index_sample) == 0)
        return 2;

    return 0;
}

void port_sdl_sound::EndSample(HSAMPLE S)
{
    Mix_HaltChannel(-1);
}

bool port_sdl_sound::InitSound()
{
    //#define MUSIC_MID_FLUIDSYNTH
    srand(time(NULL));
    if (m_hqsound)
	{
        if (Mix_OpenAudio(44100, AUDIO_S16, 2, 4096) == -1)
        {
            return false;
        }
    } 
	else 
	{
        if (Mix_OpenAudio(22050, AUDIO_U8 /*MIX_DEFAULT_FORMAT */ , 2, 4096) == -1)
        {
            return false;
        }
    }

	channelFinishedCallBack = std::bind(&port_sdl_sound::ChannelFinished, this, std::placeholders::_1);

	Mix_ChannelFinished(&ChannelFinishedCallBack);

    return true;
}

void ChannelFinishedCallBack(int channel)
{
	channelFinishedCallBack(channel);
}

void port_sdl_sound::ChannelFinished(int channel)
{
	HSAMPLE S = m_gamechunkHSAMPLE[channel];
	if (S) {
		if (S->status_1 != 1) {
			if (S->status_1 != 2) {
				S->status_1 = 2;
			}
		}
		S->vol_scale_18[0][0] = 0;
		S->flags_14 = 0;
		S->vol_scale_18[0][2] = 0;
		S->vol_scale_18[0][3] = 0;
	}
}

void port_sdl_sound::ac_set_real_vect(uint32_t vectnum, uint16_t real_ptr)
{
	m_actvect[vectnum] = real_ptr;
    //66
};

uint16_t port_sdl_sound::ac_get_real_vect(uint32_t vectnum)
{
    return m_actvect[vectnum];
};

void port_sdl_sound::Update()
{
	//No implemented
};

void port_sdl_sound::DeleteSource(uint16_t channel)
{
	//No implemented
};