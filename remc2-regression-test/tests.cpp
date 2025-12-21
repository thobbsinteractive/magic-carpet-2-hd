// Unit-tests.cpp : This file contains the main function. This is where the program execution starts and ends.
//
#include <chrono>
#include <thread>
#include <iostream>
#include "regression-tests.h"


int CountFailedRegressionTests() {
	int numFailedTests = 0;

	Logger->info("--- Regressions tests ---");
	for (int i = 1; i <= 25; i++)
		if (i != 22 && i != 25)
			if (run_regtest(i) != 0)
			{
				numFailedTests++;
			}

	// diff in level 22:
	//   the first frame with a diff has it at 0x7dba and the following bytes:
	//      buffer[32186] = 53   adress[32186] = 150
	//      buffer[32187] = 120   adress[32187] = 189
	//      buffer[32194] = 18   adress[32194] = 0
	//      buffer[32195] = 4   adress[32195] = 0
	//   in type_event_0x6E8E struct_0x6E8E[1000];
	//   size of struct_0x6E8E[1000] is 0x29040 = 168000
	//   168 byte per element
	//   -> diff in the 23rd element struct_0x6E8E[22] at position 140
	//       -> maxMana_0x8C_140 and word_0x94_148

	// diff in level 25
	// byte_counter_current_objective_box_0x36E04 = 0 instead of 200

	return numFailedTests;
}

int main(int argc, char** argv)
{
	//if (CommandLineParams.DoShowDebugMessages1()) -- for suppress messages
	int numFailedTests = 0;

	InitializeLogging(spdlog::level::info);
	numFailedTests += CountFailedRegressionTests();

	if (numFailedTests == 0)
	{
		Logger->info("All tests passed");
	}
	else
	{
		Logger->error("{} tests failed!", numFailedTests);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	return numFailedTests;
}
