#include "LevelInit.h"

type_str_164 unk_F42B0x[1136];
//MouseOnOff

//mouseturnoff
//CreateIndexes_6EB90
//ClearSettings_567C0
//LevelDecompress_533B0
//sub_54660_read_and_decompress_sky_and_blocks
//sub_54800_read_and_decompress_tables
//sub_49F30
//sub_49270_generate_level_features
//PrintTextMessage_70910
//PlayerAxis_EB398ar
//sub_49F90
//InitStages_58940
//InitStageVars_11EE0
//Init0x3664C_84790
//sub_4A1E0
//sub_53160
//sub_60F00
//sub_71A70_setTmaps

//----- (000533B0) --------------------------------------------------------
char LevelDecompress_533B0(int16 levelIndex, levelDataType_2FECE* levelData) //2343b0
{
	if (levelIndex < 1000) {
#ifdef REMC2_CODE
		std::string levelDataPath = GetSubDirectoryFile(gameFolder, "CLEVELS", "LEVELS.DAT");
		FILE* levelsdatfile = DataFileIO::CreateOrOpenFile(levelDataPath.c_str(), 512);
		if (levelsdatfile == NULL) {
			levelDataPath = GetSubDirectoryFile(cdFolder, "LEVELS", "LEVELS.DAT");
			levelsdatfile = DataFileIO::CreateOrOpenFile(levelDataPath.c_str(), 512);
			if (levelsdatfile == NULL)
				return 0;
		}
		levelDataPath = GetSubDirectoryFile(gameFolder, "CLEVELS", "LEVELS.TAB");
		FILE* levelstabfile = DataFileIO::CreateOrOpenFile(levelDataPath.c_str(), 512);
		if (levelstabfile == NULL) {
			levelDataPath = GetSubDirectoryFile(cdFolder, "LEVELS", "LEVELS.TAB");
			levelstabfile = DataFileIO::CreateOrOpenFile(levelDataPath.c_str(), 512);
			if (levelstabfile == NULL) {
				DataFileIO::Close(levelsdatfile);
				return 0;
			}
		}
		DataFileIO::Read(levelstabfile, x_DWORD_E9C38_smalltit, 4000);
		DataFileIO::Close(levelstabfile);
		if (DataFileIO::FileLengthBytes(levelsdatfile)) {
			DataFileIO::Seek(levelsdatfile, ((uint32*)x_DWORD_E9C38_smalltit)[levelIndex], 0);
			DataFileIO::Read(levelsdatfile, (uint8_t*)x_DWORD_E9C38_smalltit, ((uint32*)x_DWORD_E9C38_smalltit)[levelIndex + 1] - ((uint32*)x_DWORD_E9C38_smalltit)[levelIndex]);
			if (DataFileRNC::Decompress((uint8_t*)x_DWORD_E9C38_smalltit, (uint8_t*)x_DWORD_E9C38_smalltit) < 0) {
				myprintf("ERROR decompressing LEVELS.DAT\n");
				return 0;
			}
			shadow_levelDataType_2FECE shadow_levelData;
			qmemcpy(&shadow_levelData, (shadow_levelDataType_2FECE*)(const void*)x_DWORD_E9C38_smalltit, sizeof(shadow_levelDataType_2FECE)); //0x6604
			memset((shadow_levelDataType_2FECE*)x_DWORD_E9C38_smalltit, 0, sizeof(shadow_levelDataType_2FECE)); //0x6604
			Convert_from_shadow_str_2FECE(&shadow_levelData, levelData);
		}
		DataFileIO::Close(levelsdatfile);
		//if exist editor generated level
#if !defined(IS_EDITOR)
		if (CommandLineParams.DoLoadEditedLevel()) {
			if (config_LOAD_EDITED_LEVEL) {
				char path2[512];
				FixDir(path2, (char*)"../remc2/editor/Debug/testsave.sav");
				FILE* file = fopen(path2, "rb");
				if (file) {
					shadow_levelDataType_2FECE shadow_2FECE;
					fread(&shadow_2FECE, sizeof(shadow_levelDataType_2FECE), 1, file);
					Convert_from_shadow_str_2FECE(&shadow_2FECE, &D41A0_0.terrain_2FECE);
				}
				fclose(file);
			}
		}
#endif //!IS_EDITOR
		//if exist editor generated level
#endif //REMC2_CODE
		LevelInit_56C00(levelData);
		SetLevelId_53590(levelData);
	}
	return 1;
}

//----- (00049F30) --------------------------------------------------------
void sub_49F30()//22af30
{
	D41A0_0.struct_0x6E8E[0].dword_0xA4_164x = unk_F42B0x;
	D41A0_0.dword_0x35 = -1;
	D41A0_0.dword_0x11e6 = -1;
	int32_t index = 999;
	do
	{
		D41A0_0.dword_0x35++;
		D41A0_0.pointers_0x246[D41A0_0.dword_0x35] = x_DWORD_EA3E4[index];
		index--;
	} while (index > 0);
}

//----- (000567C0) --------------------------------------------------------
void ClearSettings_567C0()//2377c0 // clean level
{
	memset(D41A0_0.array_0x10, 0, 29);
	D41A0_0.dword_0x2d = 0;
	D41A0_0.word_0x31 = 0;
	D41A0_0.word_0x33 = 0;
	D41A0_0.dword_0x35 = 0;
	D41A0_0.byte_0x239 = 0;
	memset(&D41A0_0.str_0x364D2, 0, sizeof(D41A0_0.str_0x364D2));
	memset(&D41A0_0.array_0x2362, 0, sizeof(D41A0_0.array_0x2362));
	memset(D41A0_0.struct_0x6E8E, 0, sizeof(type_event_0x6E8E) * 0x3e8);
	memset((void*)(&x_D41A0_BYTEARRAY_4_struct.byteindex_51), 0, 1);
	memset((void*)(x_D41A0_BYTEARRAY_4_struct.byteindex_121), 0, 16);
	memset((void*)(&x_D41A0_BYTEARRAY_4_struct.speedIndex), 0, 1);
	memset((void*)(&x_D41A0_BYTEARRAY_4_struct.byteindex_180), 0, 1);
	memset((void*)(&x_D41A0_BYTEARRAY_4_struct.str_index_242ar), 0, 14);
	memset((void*)(x_D41A0_BYTEARRAY_4_struct.bytearray_38403x), 0, sizeof(type_event_0x6E8E*) * 29);
	memset((void*)(&x_D41A0_BYTEARRAY_4_struct.dword_38519), 0, sizeof(type_event_0x6E8E*));
	memset((void*)(&x_D41A0_BYTEARRAY_4_struct.dword_38523), 0, sizeof(type_event_0x6E8E*));
	memset((void*)(&x_D41A0_BYTEARRAY_4_struct.dword_38527), 0, sizeof(type_event_0x6E8E*));
	memset((void*)(&x_D41A0_BYTEARRAY_4_struct.dword_38531), 0, sizeof(type_event_0x6E8E*));
	memset((void*)(&x_D41A0_BYTEARRAY_4_struct.dword_38535), 0, sizeof(type_event_0x6E8E*));
	memset((void*)mapEntityIndex_15B4E0, 0, 0x20000);
	memset((void*)mapTerrainType_10B4E0, 0, 0x10000);
	memset((void*)mapHeightmap_11B4E0, 0, 0x10000);
	memset((void*)mapShading_12B4E0, 0, 0x10000);
	memset((void*)mapAngle_13B4E0, 0, 0x10000);
	memset((void*)x_DWORD_E9C38_smalltit, 0, 0xfa00);
	x_D41A0_BYTEARRAY_4_struct.setting_30 = 0;
	x_D41A0_BYTEARRAY_4_struct.byteindex_26 = 0;
	x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 &= 0xFFFE3FFF;
}
 
//----- (00056C00) --------------------------------------------------------
void LevelInit_56C00(levelDataType_2FECE* levelData)//237c00
{
	isCaveLevel_D41B6 = false;
	SPELLS_BEGIN_BUFFER_str[4].subspell[0].byte_0x1A = 19;
	SPELLS_BEGIN_BUFFER_str[4].subspell[0].word_0x16x = 0xc7;
	SPELLS_BEGIN_BUFFER_str[19].subspell[0].byte_0x1A = 19;
	SPELLS_BEGIN_BUFFER_str[19].subspell[0].word_0x16x = 0xf5;
	if (levelData->MapType == MapType_t::Day)
	{
		SPELLS_BEGIN_BUFFER_str[4].subspell[0].byte_0x1A = 2;
		SPELLS_BEGIN_BUFFER_str[4].subspell[0].word_0x16x = 0xc6;
		SPELLS_BEGIN_BUFFER_str[19].subspell[0].byte_0x1A = 2;
		SPELLS_BEGIN_BUFFER_str[19].subspell[0].word_0x16x = 0xf4;
		D41A0_0.m_GameSettings.str_0x2196.transparency_0x2198 = 0;
		LoadSound_84300(0);
		CursorGraphicsIndex_D419E = 1;
	}
	else if (levelData->MapType == MapType_t::Night)
	{
		D41A0_0.m_GameSettings.str_0x2196.transparency_0x2198 = 0;
		LoadSound_84300(1u);
		CursorGraphicsIndex_D419E = 9;
	}
	else if (levelData->MapType == MapType_t::Cave)
	{
		D41A0_0.m_GameSettings.str_0x2196.transparency_0x2198 = 1;
		isCaveLevel_D41B6 = 1;
		MapBasicHeight_D41B7 = levelData->byte_0x2FED3;
		LoadSound_84300(2u);
		CursorGraphicsIndex_D419E = 10;
	}
	SetDefaultSpells_5C0A0();
	D41A0_0.byte_0x36E02 = 1;
	D41A0_0.word_0x36DFE = 0;
	D41A0_0.word_0x36DFC = 0;
	D41A0_0.str_0x21AE.xxxx_0x21B1 = 0;
	D41A0_0.m_GameSettings.m_Display.xxxx_0x2191 = 0;
	D41A0_0.byte_0x36E0B = D41A0_0.byte_0x36E0B & 0xFC;
	x_D41A0_BYTEARRAY_4_struct.SelectedMenuItem_38546 = 0;
	if (x_WORD_180660_VGA_type_resolution == 1)
		FontType_D419D = 1;
	else
		FontType_D419D = 3;
}

//----- (00053590) --------------------------------------------------------
void SetLevelId_53590(levelDataType_2FECE* levelData)//234590
{
	D41A0_0.levelID_0x4 = levelData->levelID_2FED0;
}

//----- (00049270) --------------------------------------------------------
void sub_49270_generate_level_features(levelDataType_2FECE* terrain)//22a270
{
	SetStagetagForTermod_49830(terrain);
	//adress 22A27D
	if (CommandLineParams.DoDebugSequences()) {
		add_compare(0x22A280, CommandLineParams.DoDebugafterload());
	}
	GenerateEvents_49290(terrain, 1, 640, 480);
	if (CommandLineParams.DoDebugSequences()) {
		add_compare(0x22A288, CommandLineParams.DoDebugafterload());
	}
}

//----- (00056A30) --------------------------------------------------------
void LevelInitGame_56A30() //237a30
{
#ifdef REMC2_CODE
	if (CommandLineParams.DoMouseOff()) {
		mouseturnoff = true;
	}
	if (CommandLineParams.DoSetLevel()) {
		x_D41A0_BYTEARRAY_4_struct.levelnumber_43w = 1;
	}
	Logger->debug("sub_56A30_init_game_level:before sub_6EB90");
	//fixing
	CreateIndexes_6EB90(&filearray_2aa18c[filearrayindex_BUILD00DATTAB]); //24fb90 adress 0x23ca2e
	//fixing
	Logger->debug("sub_56A30_init_game_level:sub_6EB90 passed");
	char temp_x_BYTE_E3799_sound_card = soundActive_E3799;
	soundActive_E3799 = false;
	ClearSettings_567C0();
#endif //REMC2_CODE
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 8)) {
#ifdef REMC2_CODE
		PrintTextMessage_70910((char*)"Load Level\0");
		Logger->debug("sub_56A30_init_game_level:before sub_533B0_decompress_levels");
#endif //REMC2_CODE
		LevelDecompress_533B0(x_D41A0_BYTEARRAY_4_struct.levelnumber_43w, &D41A0_0.terrain_2FECE);
#ifdef REMC2_CODE
		Logger->debug("sub_56A30_init_game_level:sub_533B0_decompress_levels passed");
#endif //REMC2_CODE
	}
#ifdef REMC2_CODE
	sub_54660_read_and_decompress_sky_and_blocks(D41A0_0.terrain_2FECE.MapType, x_BYTE_D41B5_texture_size); //235660
	sub_54800_read_and_decompress_tables(D41A0_0.terrain_2FECE.MapType); //235800
	//237ab3
#endif //REMC2_CODE
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 0x10))
		D41A0_0.word_0xe = D41A0_0.terrain_2FECE.word_0x2FED7;
#ifdef REMC2_CODE
	PrintTextMessage_70910((char*)"Generate map\0");
#endif //REMC2_CODE
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 4))
		GenerateLevelMap_43830(&D41A0_0.terrain_2FECE);
#ifdef REMC2_CODE
	sub_49F30(); //prepare events pointers
	//237B05
	if (CommandLineParams.DoDebugSequences()) {
		add_compare(0x237B05, CommandLineParams.DoDebugafterload());
	}
	PrintTextMessage_70910((char*)"Generate features\0");
#endif //REMC2_CODE
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 4))
		sub_49270_generate_level_features(&D41A0_0.terrain_2FECE);
#ifdef REMC2_CODE
	PrintTextMessage_70910((char*)"Initialise Models\0");
	memset(&PlayerAxis_EB398ar, 0, 6);
#endif //REMC2_CODE
	sub_49F90();
#ifdef REMC2_CODE
	//adress 237B55
	if (CommandLineParams.DoDebugSequences()) {
		add_compare(0x237B55, CommandLineParams.DoDebugafterload());
	}
#endif //REMC2_CODE
	D41A0_0.dword_0x11e6 = -1;
#ifdef REMC2_CODE
	sub_71A70_setTmaps(D41A0_0.terrain_2FECE.MapType);
	//adress 237b75
	if (!(x_D41A0_BYTEARRAY_4_struct.setting_byte1_22 & 4)) {
		InitStages_58940();
		InitStageVars_11EE0();
		Init0x3664C_84790();
	}
	//adress 237BB0
	if (CommandLineParams.DoDebugSequences()) {
		add_compare(0x237BB0, CommandLineParams.DoDebugafterload());
	}
	if (CommandLineParams.DoSetObjective()) {
		// FIXME: D41A0_BYTESTR_0 undeclared
		// D41A0_BYTESTR_0.struct_0x3659C[0].substr_3659C.stage_0x3659F[0] = 2;
		// D41A0_BYTESTR_0.struct_0x3659C[0].substr_3659C.stage_0x3659F[1] = 2;
		// D41A0_BYTESTR_0.struct_0x3659C[0].substr_3659C.stage_0x3659F[2] = 2;
		// D41A0_BYTESTR_0.struct_0x3659C[0].substr_3659C.stage_0x3659F[3] = 2;
		// //D41A0_BYTESTR_0.struct_0x3659C[0].substr_3659C.stage_0x3659F[4] = 2;
		// D41A0_BYTESTR_0.struct_0x3659C[0].substr_3659C.array_0x3659C_byte[1] = 4;
		// //D41A0_BYTESTR_0.struct_0x3654C[0].str_3654E_word2 = 40;
		// //D41A0_BYTESTR_0.struct_0x3654C[0].str_36550_word4 = 40;
	}
#endif //REMC2_CODE
	sub_4A1E0(0, 1);
#ifdef REMC2_CODE
	//adress 237bb9
	if (CommandLineParams.DoDebugSequences()) {
		add_compare(0x237bb9, CommandLineParams.DoDebugafterload());
	}
	soundActive_E3799 = temp_x_BYTE_E3799_sound_card;
#endif //REMC2_CODE
	sub_53160();
#ifdef REMC2_CODE
	//adress 237bc7
	if (CommandLineParams.DoDebugSequences()) {
		add_compare(0x237BC7, CommandLineParams.DoDebugafterload());
	}
	//adress 237beb
#endif //REMC2_CODE
	sub_60F00();
#ifdef REMC2_CODE
	if (CommandLineParams.DoDebugSequences()) {
		add_compare(0x237BF0, CommandLineParams.DoDebugafterload());
	}
#endif //REMC2_CODE
}
