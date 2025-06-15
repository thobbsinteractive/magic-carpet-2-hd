#pragma once

#include <string>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class Config
{
private:
	struct Paths
	{
		std::string m_GameFolder = "";
		std::string m_CdFolder = "";
	};

	std::string ReadFileToString(std::string fileName);

public:
	Paths m_Paths;

	Config(std::string fileName);
};