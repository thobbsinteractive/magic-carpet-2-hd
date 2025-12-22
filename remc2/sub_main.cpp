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