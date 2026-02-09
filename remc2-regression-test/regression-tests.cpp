#include "regression-tests.h"

int run_regtest(int level, bool afterload, int index, int saveIndex)//236F70
{
	int exitCode = 0;
	Logger->info("Testing aftreload {} for Level {}", index, level);

	unitTests = true;
	std::string locUnitTestsPath;
	if (afterload)
		locUnitTestsPath = get_exe_path() + "/memimages/regressions/afterloadtest" + std::to_string(index);
	else
		locUnitTestsPath = get_exe_path() + "/memimages/regressions/level" + std::to_string(level);
	unitTestsPath = locUnitTestsPath;
	int locEndTestsCode;
	endTestsCode = &locEndTestsCode;

	int argc = 0;
	char* argv[9];
	std::string path = get_exe_path() + "/regression-config.ini";
	if (afterload)
	{
		argc = 9;
		char arg1[] = "remc2";
		char arg2[] = "--mode_test_regressions_game";
		strcpy(arg2, "--mode_debug_afterload");
		char arg3[4];
		sprintf(arg3, "%d", saveIndex);
		char arg4[] = "--text_output_to_console";
		char arg5[] = "--set_level";
		char arg6[4];
		sprintf(arg6, "%d", level - 1);
		char arg7[] = "--config_file_path";
		char* arg8 = &path[0];
		char arg9[] = "--debugafterload";
		argv[0] = arg1;
		argv[1] = arg2;
		argv[2] = arg3;
		argv[3] = arg4;
		argv[4] = arg5;
		argv[5] = arg6;
		argv[6] = arg7;
		argv[7] = arg8;
		argv[8] = arg9;
	}
	else
	{
		argc = 7;
		char arg1[] = "remc2";
		char arg2[] = "--mode_test_regressions_game";
		char arg3[] = "--text_output_to_console";
		char arg4[] = "--set_level";
		char arg5[4];
		sprintf(arg5, "%d", level - 1);
		char arg6[] = "--config_file_path";
		char* arg7 = &path[0];
		argv[0] = arg1;
		argv[1] = arg2;
		argv[2] = arg3;
		argv[3] = arg4;
		argv[4] = arg5;
		argv[5] = arg6;
		argv[6] = arg7;
	}
	char* envp[] = { nullptr };

	for (int i = 0; i < 100; i++)
	{
		compstr[i].adress = 0;
		compstr[i].index = 0;
	}

	CommandLineParams.Init(argc, argv);
	support_begin();
	x_BYTE_D4B80 = 0;
	CleanF5538_716A0();
	for (int i = 0; i < 504; i++)
		str_DWORD_F66F0x[i] = nullptr;
	x_DWORD_E9C28_str = nullptr;

	try
	{
		sub_main(argc, argv, envp);
	}
	catch (const thread_exit_exception& e) {}
	catch (const std::exception& e)
	{
		Logger->error("Exception running main thread: {}", e.what());
		exitCode = -1;
	}

	support_end();
	if (locEndTestsCode == 20)
		if (afterload)
			Logger->info("Test aftreload {} for Level {} - OK\n\n", index, level);
		else
			Logger->info("Test Level {} - OK\n\n", level);
	else
	{
		if (afterload)
			Logger->info("Test aftreload {} for Level {} - FAILED\n\n", index, level);
		else
			Logger->error("Test Level {} - FAILED\n\n", level);
		exitCode = -1;
	}
	return exitCode;
}