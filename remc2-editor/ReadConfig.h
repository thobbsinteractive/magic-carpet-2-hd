#pragma once
#ifndef READCONFIG
#define READCONFIG

#include <filesystem>

#include "../remc2/engine/CommandLineParser.h"
#include "../remc2/portability/port_filesystem.h"
#include "INIReader.h"

class ReadConfig
{
private:
	std::string FindIniFile();

public:
	bool ReadIni();
};

#endif //READCONFIG
