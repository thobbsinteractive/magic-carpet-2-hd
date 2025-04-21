#include "ReadConfig.h"
#include <iostream>

std::string ReadConfig::FindIniFile() {
	// find location of inifile and read it
	std::vector<std::string> inifile_locations;
	if (CommandLineParams.GetConfigFilePath().length() > 0) {
		inifile_locations.push_back(CommandLineParams.GetConfigFilePath());
	}
	else {
#ifdef __linux__
		auto env_home_dir = std::getenv("HOME");
		auto env_xdg_config_home_dir = std::getenv("XDG_CONFIG_HOME");
		std::filesystem::path home_dir;
		std::filesystem::path xdg_config_home_dir;
		if (env_home_dir) home_dir = env_home_dir;
		if (env_xdg_config_home_dir) xdg_config_home_dir = env_xdg_config_home_dir;

		if (std::filesystem::exists(xdg_config_home_dir)) {
			inifile_locations.emplace_back(xdg_config_home_dir / "remc2-editor" / "editor-config.ini");
		}
		if (std::filesystem::exists(home_dir)) {
			inifile_locations.emplace_back(home_dir / ".config" / "remc2-editor" / "editor-config.ini");
		}
#else //__linux__
		auto home_drive = std::getenv("HOMEDRIVE");
		auto home_path = std::getenv("HOMEPATH");
		if (home_drive && home_path) {
			std::string home_dir = std::string(home_drive) + "/" + std::string(home_path);
			inifile_locations.push_back(home_dir + "/remc2-editor/editor-config.ini");
		}
#endif //__linux__
		inifile_locations.push_back(get_exe_path() + "/editor-config.ini");
	}
	std::string inifile;
	// first location at which an inifile can be found is chosen
	for (auto inifile_location: inifile_locations) {
		if (std::filesystem::exists(inifile_location)) {
			inifile = inifile_location;
			break;
		}
	}


	return inifile;
}

bool ReadConfig::ReadIni() {

	std::string inifile = FindIniFile();
	if (std::filesystem::exists(inifile)) {
		Logger->info("Using inifile: {}", inifile);
	}
	else {
		Logger->error("Inifile cannot be found... Exiting");
		return false;
	}

	INIReader reader(inifile);

	if (reader.ParseError() < 0) {
		Logger->error("Can't load 'editor-config.ini'");
		return false;
	}

	std::string readstr2 = reader.GetString("main", "gameFolder", "");
	strcpy((char*)gameFolder, (char*)readstr2.c_str());
	std::string readstr4 = reader.GetString("main", "cdFolder", "");
	strcpy((char*)cdFolder, (char*)readstr4.c_str());

	return true;
};
