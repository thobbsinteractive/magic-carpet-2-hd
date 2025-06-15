#include "Config.h"
#include <filesystem>
#include <iostream>
#include <fstream>

Config::Config(std::string fileName)
{
	std::string json = "";

	if (std::filesystem::exists(fileName))
	{
		auto json = ReadFileToString(fileName);

		if (json.size() > 0)
		{
			rapidjson::Document document;
			document.Parse(json.c_str());
			if (document.HasMember("paths"))
			{
#ifdef __linux__
				auto paths = document["paths"].GetObject();
#else
				auto paths = document["paths"].GetObj();
#endif
				if (paths.HasMember("gameFolder"))
					m_Paths.m_GameFolder = paths["gameFolder"].GetString();

				if (paths.HasMember("cdFolder"))
					m_Paths.m_CdFolder = paths["cdFolder"].GetString();
			}
		}
	}
}

std::string Config::ReadFileToString(std::string fileName)
{
	std::string json;

	try
	{
		std::ifstream is(fileName, std::ifstream::binary);
		std::string line;
		while (std::getline(is, line))
		{
			json += line;
			json.push_back('\n');
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error Stopping Worker Thread: " << e.what();
	}
	return json;
};
