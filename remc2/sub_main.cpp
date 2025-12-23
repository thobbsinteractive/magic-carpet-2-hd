#include "sub_main.h"

#include "engine/CommandLineParser.h"
#include "engine/GameUI.h"
#include "engine/Graphics.h"
#include "engine/MenusAndIntros.h"
#include "engine/Network.h"
#include "engine/Player.h"
#include "engine/PlayerInput.h"
#include "engine/engine_support.h"
#include "engine/CompareMaps.h"
#include "engine/Events.h"
#include "engine/Level.h"
#include "utilities/StateMonitor.h"
#include "utilities/RendererTests.h"
#include "engine/ConvertMapInfo.h"
#include "engine/LevelInit.h"
#include "engine/DatTabIndexes.h"
#include "engine/MouseOnOff.h"
#include "engine/PrintMessages.h"
#include "engine/ReadAndDecompress.h"

/*

//2541e7
mainConnection_E12AA 2b22aa->326f0 7f0300 .. 03
after NetworkTestCall_72FBB 350000 .. 00
after NetworkCancel_748F7 not changed

connection_E12AE[] 2b22ae->34340 b0160002 .. 16
after NetworkTestCall_72FBB 900b73 .. 0b
after NetworkCancel_748F7 not changed

*/

#ifdef __linux__
#include <strings.h>
#include <cstdlib>
#include <cstring>
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define strnicmp strncasecmp
#define __cdecl
#include <ctype.h>
#include "../findfirst/findfirst.h"
#include <iostream>
#include <functional>
#include <type_traits>
#else
#endif //__linux__

#include <filesystem>

#include "./engine/EventsFunctions.h"

//int test_regression_level = 1;
//first multi is 50(51) 10
//first hide level is 30(31) 5

//adress 2285ff

//int save_debugcounter = 0;

#define FIX_sub_48B90// - when set game have error, but compactible with original

//(set in bool sub_558E0_InGameLoad(uint8_t fileindex)//2368e0)
//int count_begin = 1;//1
//int stage__4A190_0x6E8E = 0;
//int debugnextlevel = 0;

//bool config_EDITOR = false;

void preconvert() {
	/*
	int16_t word_0;
	int16_t word_2;
	int16_t word_4;
	int16_t word_6;
	int16_t word_8;
	int16_t word_10;
	int16_t word_12;
	int16_t word_14;
	int16_t word_16;
	int16_t word_18;
	int16_t word_20;
}
type_D404C;
#pragma pack (16)

type_D404C str_D404C[5];

uint8_t ar_D404C[22 * 5]//particles table*/
/*
//type_E24BCx str_E24BCx[32];
memcpy((type_E17CC_0*)str_E1328, array_E1328 , 0x100*sizeof(type_E17CC_0));
char outname[512];
char buffer[512];
sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
FILE* file = fopen(outname, "wt");
sprintf(buffer, "type_E17CC_0 str_E17CC_0[48] = {\n");
fwrite(buffer, strlen(buffer), 1, file);
for (int i = 0; i < 0x100; i++)
{
	sprintf(buffer, "{0x%04X,0x%02X,0x%08X},\n",
		str_E1328[i].word_0, str_E1328[i].byte_2, str_E1328[i].dword_3
		);

	fwrite(buffer, strlen(buffer), 1, file);
}
sprintf(buffer, "};\n");
fwrite(buffer, strlen(buffer), 1, file);
fclose(file);
*/


/*
memcpy((uint8_t*)str_x_DWORD_D4C52ar_0x2F22, x_DWORD_D4C52ar + 0x2F22, 4000);
char outname[512];
char buffer[512];
sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
FILE* file = fopen(outname, "wt");
sprintf(buffer, "type_mapScreenPortals_E17CC mapScreenPortals_E17CC[168] = {\n");
fwrite(buffer, strlen(buffer), 1, file);
for (int i = 0; i < 168; i++)
{
	sprintf(buffer, "{0x%04X,0x%02X,0x%04X,0x%04X},\n",
		str_x_DWORD_D4C52ar_0x2F22[i].dword_0, str_x_DWORD_D4C52ar_0x2F22[i].word_4, str_x_DWORD_D4C52ar_0x2F22[i].adress_6,
		str_x_DWORD_D4C52ar_0x2F22[i].dword_10);
	fwrite(buffer, strlen(buffer), 1, file);
}
sprintf(buffer, "};\n");
fwrite(buffer, strlen(buffer), 1, file);
fclose(file);*/
/*
#pragma pack (1)
typedef struct {//lenght 22
	int16_t word_0;
	int16_t word_2;
	int16_t word_4;
	int16_t word_6;
	int16_t word_8;
	int16_t word_10;
	int16_t word_12_x; //X
	int16_t word_14_y; //Y
	int16_t word_16;
	int8_t byte_18_act;//1-hidden 2-activated
	int8_t byte_19;
	int16_t word_20;
}type_mapScreenPortals_E17CC;
#pragma pack (16)
	type_mapScreenPortals_E17CC mapScreenPortals_E17CC[168] = {
	//0 2  4    6       8   10      12      14        16    18 19 20 stub
	{0, 0, 0x74, 0x1de, 28, 0xa40,   0x341, 0x213,      0,    2,  0, 0, 0 }
};

uint8_t unk_E17CC_0x194x[0xE6C] = {
	*/

	/*memcpy((uint8_t*)x_BYTE_E25ED_db_str, x_BYTE_E25ED_db, 0x1E0);
	char outname[512];
	char buffer[512];
	sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
	FILE* file = fopen(outname, "wt");
	sprintf(buffer, "type_x_BYTE_E25ED_db_str x_BYTE_E25ED_db_str[16]={\n");
	fwrite(buffer, strlen(buffer), 1, file);
	for (int i = 0; i < 17; i++)
	{
		sprintf(buffer, "{0x%08X,0x%08X,0x%04X,0x%04X,0x%04X,0x%04X,0x%04X,0x%04X,0x%02X,0x%02X,0x%04X,0x%04X,0x%04X,0x%04X},\n", x_BYTE_E25ED_db_str[i].dword_0, x_BYTE_E25ED_db_str[i].dword_4, x_BYTE_E25ED_db_str[i].word_8, x_BYTE_E25ED_db_str[i].word_10,
			x_BYTE_E25ED_db_str[i].word_12, x_BYTE_E25ED_db_str[i].word_14, x_BYTE_E25ED_db_str[i].word_16, x_BYTE_E25ED_db_str[i].word_18,
			x_BYTE_E25ED_db_str[i].byte_20, x_BYTE_E25ED_db_str[i].byte_21, x_BYTE_E25ED_db_str[i].word_22, x_BYTE_E25ED_db_str[i].word_24,
			x_BYTE_E25ED_db_str[i].word_26, x_BYTE_E25ED_db_str[i].word_28);
		fwrite(buffer, strlen(buffer), 1, file);
	}
	sprintf(buffer, "};\n");
	fwrite(buffer, strlen(buffer), 1, file);
	fclose(file); */
	/*
	typedef struct {//lenght 30
		int32_t dword_0;
		int32_t dword_4;
		int16_t word_8;
		int16_t word_10;
		int16_t word_12;
		int16_t word_14;
		int16_t word_16;
		int16_t word_18;
		int8_t byte_20;
		int8_t byte_21;
		int16_t word_22;
		int16_t word_24;
		int16_t word_26;
		//int8_t stuba[1];
		int16_t word_28;
		//int8_t stubb[1];
	} type_x_BYTE_E25ED_db_str;

	type_x_BYTE_E25ED_db_str x_BYTE_E25ED_db_str[16]=;
	uint8_t x_BYTE_E25ED_db[0x1E0] = {
	*/
	/*memcpy((uint8_t*)&str_D4C52ar[1], x_DWORD_D4C52ar+8, 0x132);
	str_D4C52ar[0].dword_0 = 0x2a5c44;
	char outname[512];
	char buffer[512];
	sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
	FILE* file = fopen(outname,"wt");
	sprintf(buffer,"type_D4C52ar str_D4C52ar[18]={\n");
	fwrite(buffer,strlen(buffer),1,file);
	for (int i = 0; i < 17; i++)
	{
		sprintf(buffer, "0x%08X,0x%08X,0x%04X,0x%08X,0x%08X,\n", str_D4C52ar[i].dword_0, str_D4C52ar[i].dword_4, str_D4C52ar[i].word_8, str_D4C52ar[i].dword_10, str_D4C52ar[i].dword_14);
		fwrite(buffer, strlen(buffer), 1, file);
	}
	sprintf(buffer, "};\n");
	fwrite(buffer, strlen(buffer), 1, file);
	fclose(file);*/
	/*
	memcpy(x_DWORD_D4C52ar_strtest, x_DWORD_D4C52ar + 0x002A7400 - 0x2a5c52, 14*500);
	char outname[512];
	char buffer[512];
	sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
	FILE* file = fopen(outname, "wt");
	sprintf(buffer, "type_D4C52ar2 x_DWORD_D4C52ar_strA0[32]={\n");
	fwrite(buffer, strlen(buffer), 1, file);
	for (int i = 0; i < 500; i++)
	{
		sprintf(buffer, "0x%08X,0x%04X,0x%08X,0x%08X,\n", x_DWORD_D4C52ar_strtest[i].dword_0, x_DWORD_D4C52ar_strtest[i].word_4, x_DWORD_D4C52ar_strtest[i].dword_6, x_DWORD_D4C52ar_strtest[i].dword_10);
		fwrite(buffer, strlen(buffer), 1, file);
	}
	sprintf(buffer, "};\n");
	fwrite(buffer, strlen(buffer), 1, file);
	fclose(file);
	*/
	//memcpy((uint8_t*)str_D4D84ar, x_DWORD_D4C52arx+0x132, 0x67da);

	/*
	memcpy((uint8_t*)& SPELLS_BEGIN_BUFFER_str, SPELLS_BEGIN_BUFFER_DA818x, 26*80);
	char outname[512];
	char buffer[512];
	sprintf(outname, "c:\\prenos\\remc2\\outstr.txt");
	FILE* file = fopen(outname, "wt");
	sprintf(buffer, "type_SPELLS_BEGIN_BUFFER_str SPELLS_BEGIN_BUFFER_str[26] =\n");
	fwrite(buffer, strlen(buffer), 1, file);
	for (int i = 0; i < 26; i++)
	{
		sprintf(buffer, "{{0x%02X,0x%02X,{\n", SPELLS_BEGIN_BUFFER_str[i].byte_0, SPELLS_BEGIN_BUFFER_str[i].byte_1);
		fwrite(buffer, strlen(buffer), 1, file);
		sprintf(buffer, "{0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%04X,0x%04X,0x%02X,0x%02X},\n", SPELLS_BEGIN_BUFFER_str[i].subspell[0].dword_2, SPELLS_BEGIN_BUFFER_str[i].subspell[0].dword_6, SPELLS_BEGIN_BUFFER_str[i].subspell[0].dword_A, SPELLS_BEGIN_BUFFER_str[i].subspell[0].dword_E, SPELLS_BEGIN_BUFFER_str[i].subspell[0].dword_0x12, SPELLS_BEGIN_BUFFER_str[i].subspell[0].word_0x16x, SPELLS_BEGIN_BUFFER_str[i].subspell[0].word_0x18, SPELLS_BEGIN_BUFFER_str[i].subspell[0].byte_0x1A, SPELLS_BEGIN_BUFFER_str[i].subspell[0].byte_0x1B);
		fwrite(buffer, strlen(buffer), 1, file);
		sprintf(buffer, "{0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%04X,0x%04X,0x%02X,0x%02X},\n", SPELLS_BEGIN_BUFFER_str[i].subspell[1].dword_2, SPELLS_BEGIN_BUFFER_str[i].subspell[1].dword_6, SPELLS_BEGIN_BUFFER_str[i].subspell[1].dword_A, SPELLS_BEGIN_BUFFER_str[i].subspell[1].dword_E, SPELLS_BEGIN_BUFFER_str[i].subspell[1].dword_0x12, SPELLS_BEGIN_BUFFER_str[i].subspell[1].word_0x16x, SPELLS_BEGIN_BUFFER_str[i].subspell[1].word_0x18, SPELLS_BEGIN_BUFFER_str[i].subspell[1].byte_0x1A, SPELLS_BEGIN_BUFFER_str[i].subspell[1].byte_0x1B);
		fwrite(buffer, strlen(buffer), 1, file);
		sprintf(buffer, "{0x%08X,0x%08X,0x%08X,0x%08X,0x%08X,0x%04X,0x%04X,0x%02X,0x%02X}}},\n", SPELLS_BEGIN_BUFFER_str[i].subspell[2].dword_2, SPELLS_BEGIN_BUFFER_str[i].subspell[2].dword_6, SPELLS_BEGIN_BUFFER_str[i].subspell[2].dword_A, SPELLS_BEGIN_BUFFER_str[i].subspell[2].dword_E, SPELLS_BEGIN_BUFFER_str[i].subspell[2].dword_0x12, SPELLS_BEGIN_BUFFER_str[i].subspell[2].word_0x16x, SPELLS_BEGIN_BUFFER_str[i].subspell[2].word_0x18, SPELLS_BEGIN_BUFFER_str[i].subspell[2].byte_0x1A, SPELLS_BEGIN_BUFFER_str[i].subspell[2].byte_0x1B);
		fwrite(buffer, strlen(buffer), 1, file);
	}
	sprintf(buffer, "};\n");
	fwrite(buffer, strlen(buffer), 1, file);
	fclose(file);
	*/
	/*
	int32_t dword_2;
	int32_t dword_6;
	int32_t dword_A;
	int32_t dword_E;
	int32_t dword_0x12;
	int16_t word_0x16x;
	int16_t word_0x18;
	int8_t byte_0x1A;
	uint8_t byte_0x1B;
}
type_SPELLS_BEGIN_BUFFER_str_sub;

typedef struct {//lenght 80
	int8_t byte_0;
	uint8_t byte_1;
	type_SPELLS_BEGIN_BUFFER_str_sub subspell[3];
}
type_SPELLS_BEGIN_BUFFER_str;

	type_SPELLS_BEGIN_BUFFER_str SPELLS_BEGIN_BUFFER_str[26] =
	{{1,2,{
	{3,4,0,0,0,0,0,0,0},
	{5,6,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0}}}
	};
	*/
	/*type_SPELLS_BEGIN_BUFFER_str SPELLS_BEGIN_BUFFER_str[26];

	int8_t SPELLS_BEGIN_BUFFER_DA818x[0x820] = {*/
}

void begin_plugin() {
	//convert_x_WORD_E2970y_to_x_WORD_E2970();
	//x_D41A0_BYTEARRAY_4_struct.dwordindex_2388 = 10022;//fix sound param
}

void initposistruct() {//fix it
	posistruct1 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);
	posistruct2 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);
	posistruct3 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);
	posistruct4 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);
	posistruct5 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);
	posistruct6 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);
	posistruct7 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);
	posistruct8 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);
	posistruct9 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);
	posistruct10 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);
	posistruct11 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);

	x_DWORD_EA3D4 = (bitmap_pos_struct_t*)malloc(sizeof(bitmap_pos_struct_t) * 1700);
};

//----- (00055F70) --------------------------------------------------------
int sub_main(int argc, char** argv, char**  /*envp*/)//236F70
{
	int exitCode = 0;
	try
	{
		begin_plugin();

		preconvert();//rewrite and remove it later

		*xadataclrd0dat.colorPalette_var28 = (uint8_t*)malloc(4096);//fix it

		//*xadataspellsdat.colorPalette_var28 = (uint8_t*)malloc(50000);

		signed int v3; // edi
		unsigned __int16 v4; // si
		//skip __int16 v6; // [esp+0h] [ebp-1Ch]
		//__int16 v7; // [esp+Ch] [ebp-10h]

		//fix it
		v3 = 0;
		v4 = 0;
		//fix it

		//skip memset(&v6, 0, 28);//236F7F - 26D250
		//v7 = 0;
		//skip v6 = 0x3301;
	//removed  int386(0x21, (REGS*)&v6, (REGS*)&v6);//236F9D - 279D52 //INT 21,33 - Get/Set System Values (Ctl-Break/Boot Drive) AH = 33h AL = 01 to set Ctrl - Break checking flag
	//may be INT 33,1 Show Mouse Cursor see:https://www.equestionanswers.com/c/c-int33-mouse-service.php
		//skip signal(7, 1);//236FA9 - 279DC0
		//skip signal(4, 1);//236FB5 - 279DC0
		//skip signal(6, 1);//236FC1 - 279DC0

		printf("Reading Ini file\n");
		if (!readini()) exit(1);

		spdlog::level::level_enum level = spdlog::level::info;

#ifdef _DEBUG
		level = GetLoggingLevelFromString("Debug");
#else
		level = GetLoggingLevelFromString(loggingLevel.c_str());
#endif
		InitializeLogging(level);

		if (assignToSpecificCores)
		{
#ifdef _MSC_VER
			SetThreadIdealProcessor(GetCurrentThread(), 0);
			DWORD_PTR dw = SetThreadAffinityMask(GetCurrentThread(), DWORD_PTR(1) << 0);
#endif
		}

		if (CommandLineParams.DoDisableGraphicsEnhance()) {
			Logger->debug("Disabling enhanced graphics");
			bigSprites = false;
			bigTextures = false;
			texturepixels = 32;
		}

		//Set Paths for game data
		Logger->debug("Getting Game data paths");
		gameDataPath = GetSubDirectoryPath(gameFolder);
		cdDataPath = GetSubDirectoryPath(cdFolder);
		bigGraphicsPath = GetSubDirectoryPath(bigGraphicsFolder);

		Logger->debug("Initializing graphics Width: {} Height: {}", windowResWidth, windowResHeight);
		VGA_Init(windowResWidth, windowResHeight, gameResWidth, gameResHeight, maintainAspectRatio, displayIndex);
		gamepad_init(gameResWidth, gameResHeight);

		Logger->info("Finding Game Data...");
		if (std::string mainfile = GetSubDirectoryFile(gameFolder, "CDATA", "TMAPS0-0.DAT"); !file_exists(mainfile.c_str()))//test original file
		{
			if (std::filesystem::is_directory(gameDataPath))
			{
				Logger->info("Original game not found in {0} sub folder ", gameFolder);
				Logger->info("Installing game data from CD_Files...");
			}
			else
			{
				Logger->error("Sub folder {0} does not exist!", gameFolder);
				mydelay(5000);
				exit(1);//iso not found
			}
		}
		else
		{
			Logger->info("Original Game Data Found!");
		}

		//dos_setvect(9, null_vector, 0);

		initposistruct();

		sub_56210_process_command_line(argc, argv);//236FD4 - 237210
		if (CommandLineParams.ModeTestNetwork()) {
			if (Iam_server || Iam_client)
				InitNetworkInfo();
		}

		//-init 0x2a51a4 je nekde tu
		if (CommandLineParams.DoCopySkipConfig()) {
			x_BYTE_D41AD_skip_screen = config_skip_screen;
		}

		Initialize();//236FDC - 23C8D0//rozdil 1E1000

		sub_46830_main_loop(/*0, */v3, v4);//227830

		sub_5BC20();//23CC20 //remove devices?
		sub_56730_clean_memory();//237730

		if (CommandLineParams.ModeTestNetwork()) {
			if (Iam_server || Iam_client)
			{
				EndMyNetLib();
				/*EndLibNetClient();
				if (Iam_server)
					EndLibNetServer();*/
			}
		}
	}
	catch (const thread_exit_exception& e)
	{
		Logger->info("Immediate Exit called");
	}
	catch (const std::exception& e)
	{
		Logger->critical("Critial Error: {}", e.what());
		exitCode = -1;
	}
	Logger->info("Exited Game");
	return exitCode;
}