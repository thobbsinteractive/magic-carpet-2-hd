#pragma once
#ifndef READCONFIG
#define READCONFIG

#include <filesystem>
#include <iostream>
#include "../remc2/engine/CommandLineParser.h"
#include "../remc2/portability/Config.h"
#include "../remc2/portability/port_filesystem.h"

class ReadConfig
{
private:
	std::string FindConfigFile();

public:
	bool SetConfig();
};

#endif //READCONFIG
