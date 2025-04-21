#include "read_config.h"

int config_skip_screen;
int texturepixels = 32;
int maxGameFps = 30;
int fmvFps = 20;
int menuFps = 30;
std::string loggingLevel = "Info";
int displayIndex = 0;
int windowResWidth = 640;
int windowResHeight = 480;
int gameResWidth = 640;
int gameResHeight = 480;
int gameUiScale = 1;
bool maintainAspectRatio = false;
bool forceWindow = false;
bool bigTextures = false;
bool bigSprites = false;
bool sky = true;
bool reflections = false;
bool dynamicLighting = false;
bool multiThreadedRender = false;
int numberOfRenderThreads = 0;
bool assignToSpecificCores = false;
bool openGLRender = false;
bool invertYAxis = true;
bool invertXAxis = false;
gamepad_config_t gpc;

std::string findIniFile() {
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
			inifile_locations.emplace_back(xdg_config_home_dir / "remc2" / "config.ini");
		}
		if (std::filesystem::exists(home_dir)) {
			inifile_locations.emplace_back(home_dir / ".config" / "remc2" / "config.ini");
		}
#else //__linux__
		auto home_drive = std::getenv("HOMEDRIVE");
		auto home_path = std::getenv("HOMEPATH");
		if (home_drive && home_path) {
			std::string home_dir = std::string(home_drive) + "/" + std::string(home_path);
			inifile_locations.push_back(home_dir + "/remc2/config.ini");
		}
#endif //__linux__
		inifile_locations.push_back(get_exe_path() + "/config.ini");
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

std::vector<Maths::Zone> ReadZones(std::string zonesJson) {
	std::vector<Maths::Zone> zones;

	if (zonesJson.size() > 0)
	{
		rapidjson::Document document;
		document.Parse(zonesJson.c_str());
		if (document.HasMember("Zones"))
		{
			auto zonesArray = document["Zones"].GetArray();
			for (int i = 0; i < zonesArray.Size(); i++) // Uses SizeType instead of size_t
			{
#ifdef __linux__
				auto zone = zonesArray[i].GetObject();
#else
				auto zone = zonesArray[i].GetObj();
#endif
				if (zone.HasMember("Start") && zone.HasMember("End") && zone.HasMember("Factor"))
				{
					zones.push_back(Maths::Zone{ (uint16_t)zone["Start"].GetInt(), (uint16_t)zone["End"].GetInt(), zone["Factor"].GetDouble() });
				}
			}
		}
	}
	return zones;
}

bool readini() {
	uint8_t gp_temp;
	std::string inifile = findIniFile();
	if (std::filesystem::exists(inifile)) {
		if (CommandLineParams.DoShowDebugMessages1())
			std::cout << "Using inifile: " << inifile << "\n";
	}
	else {
		if (CommandLineParams.DoShowDebugMessages1())
			std::cout << "Inifile cannot be found... Exiting\n";
		return false;
	}

	INIReader reader(inifile);

	if (reader.ParseError() < 0) {
		std::cout << "Can't load 'test.ini'\n";
		return false;
	}
	if (reader.GetBoolean("skips", "skipintro", true))
		config_skip_screen = 1;
	else
		config_skip_screen = 0;

	if (reader.GetBoolean("sound", "hqsound", true))
		hqsound = true;
	else
		hqsound = false;

	if (reader.GetBoolean("sound", "fixspeedsound", true))
		fixspeedsound = true;
	else
		fixspeedsound = false;

	if (reader.GetBoolean("sound", "oggmusic", true))
	{
		oggmusic = true;
		hqsound = true;//for mp3 music must be activate hqsound
	}
	else
		oggmusic = false;
	if (reader.GetBoolean("sound", "oggmusicalternative", true))
	{
		oggmusicalternative = true;
	}
	else
	{
		oggmusicalternative = false;
	}

	std::string readstr = reader.GetString("sound", "oggmusicFolder", "");
	strcpy(oggmusicFolder, (char*)readstr.c_str());

	std::string readstr3 = reader.GetString("graphics", "bigGraphicsFolder", "");
	strcpy(bigGraphicsFolder, (char*)readstr3.c_str());

	if (reader.GetBoolean("graphics", "useEnhancedGraphics", false) && strlen(bigGraphicsFolder) > 0 
		&& std::filesystem::is_directory(GetSubDirectoryPath(bigGraphicsFolder)))
	{
		bigSprites = true;
		bigTextures = true;
		texturepixels = 128;
	}
	else
	{
		texturepixels = 32;
	}

	displayIndex = reader.GetInteger("graphics", "displayIndex", 0);
	windowResWidth = reader.GetInteger("graphics", "windowResWidth", 640);
	windowResHeight = reader.GetInteger("graphics", "windowResHeight", 480);

	if (windowResWidth < 640 || windowResHeight < 480)
	{
		windowResWidth = 640;
		windowResHeight = 480;
	}

	gameResWidth = reader.GetInteger("graphics", "gameResWidth", 640);
	gameResHeight = reader.GetInteger("graphics", "gameResHeight", 480);

	if (gameResWidth < 320 || gameResHeight < 200)
	{
		gameResWidth = 320;
		gameResHeight = 200;
	}

	gameUiScale = reader.GetInteger("graphics", "gameUiScale", 1);

	if (gameUiScale < 1)
		gameUiScale = 1;

	if (gameUiScale > 8 || (640 * gameUiScale) > gameResWidth)
	{
		while (gameUiScale > 1 && (640 * gameUiScale) > gameResWidth)
		{
			gameUiScale--;
		}
	}

	maintainAspectRatio = reader.GetBoolean("graphics", "maintainAspectRatio", true);
	forceWindow = reader.GetBoolean("graphics", "forceWindow", false);
	sky = reader.GetBoolean("graphics", "sky", true);
	reflections = reader.GetBoolean("graphics", "reflections", false);
	dynamicLighting = reader.GetBoolean("graphics", "dynamicLighting", false);

	std::string readstr2 = reader.GetString("main", "gameFolder", "");
	strcpy((char*)gameFolder, (char*)readstr2.c_str());
	std::string readstr4 = reader.GetString("main", "cdFolder", "");
	strcpy((char*)cdFolder, (char*)readstr4.c_str());

	std::string readstr5 = reader.GetString("graphics", "forceRender", "");
	strcpy((char*)forceRender, (char*)readstr5.c_str());

	openGLRender = reader.GetBoolean("graphics", "openGLRender", false);

	if (!openGLRender)
	{
		multiThreadedRender = reader.GetBoolean("graphics", "multiThreadedRender", false);
		numberOfRenderThreads = reader.GetInteger("graphics", "numberOfRenderThreads", 0);

		if (multiThreadedRender)
		{
			assignToSpecificCores = reader.GetBoolean("graphics", "assignToSpecificCores", false);

			if (numberOfRenderThreads < 1)
			{
				numberOfRenderThreads = 1;
			}
		}
		else
		{
			numberOfRenderThreads = 0;
		}
	}

	maxGameFps = reader.GetInteger("game", "maxGameFps", 0);
	fmvFps = reader.GetInteger("game", "fmvFps", 20);
	loggingLevel = reader.GetString("game", "loggingLevel", "Info");
	invertYAxis = reader.GetBoolean("game", "invertYAxis", true);
	invertXAxis = reader.GetBoolean("game", "invertXAxis", false);

	gpc.axis_yaw = reader.GetInteger("gamepad", "axis_yaw", GAMEPAD_ITEM_DISABLED);
	gpc.axis_pitch = reader.GetInteger("gamepad", "axis_pitch", GAMEPAD_ITEM_DISABLED);
	gpc.axis_long = reader.GetInteger("gamepad", "axis_long", GAMEPAD_ITEM_DISABLED);
	gpc.axis_trans = reader.GetInteger("gamepad", "axis_trans", GAMEPAD_ITEM_DISABLED);
	gpc.axis_nav_ns = reader.GetInteger("gamepad", "axis_nav_ns", GAMEPAD_ITEM_DISABLED);
	gpc.axis_nav_ew = reader.GetInteger("gamepad", "axis_nav_ew", GAMEPAD_ITEM_DISABLED);
	gpc.axis_fire_R = reader.GetInteger("gamepad", "axis_fire_R", GAMEPAD_ITEM_DISABLED);
	gpc.axis_fire_L = reader.GetInteger("gamepad", "axis_fire_L", GAMEPAD_ITEM_DISABLED);

	gp_temp = reader.GetBoolean("gamepad", "axis_yaw_inv", 0);
	if (gpc.axis_yaw) {
		gpc.axis_yaw -= 1; // go back to SDL axis notation
		gpc.axis_yaw_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = reader.GetBoolean("gamepad", "axis_pitch_inv", 0);
	if (gpc.axis_pitch) {
		gpc.axis_pitch -= 1; // go back to SDL axis notation
		gpc.axis_pitch_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = reader.GetBoolean("gamepad", "axis_long_inv", 0);
	if (gpc.axis_long) {
		gpc.axis_long -= 1; // go back to SDL axis notation
		gpc.axis_long_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = reader.GetBoolean("gamepad", "axis_trans_inv", 0);
	if (gpc.axis_trans) {
		gpc.axis_trans -= 1; // go back to SDL axis notation
		gpc.axis_trans_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = reader.GetBoolean("gamepad", "axis_nav_ns_inv", 0);
	if (gpc.axis_nav_ns) {
		gpc.axis_nav_ns -= 1; // go back to SDL axis notation
		gpc.axis_nav_ns_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = reader.GetBoolean("gamepad", "axis_nav_ew_inv", 0);
	if (gpc.axis_nav_ew) {
		gpc.axis_nav_ew -= 1; // go back to SDL axis notation
		gpc.axis_nav_ew_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	if (gpc.axis_fire_R) {
		gpc.axis_fire_R -= 1; // go back to SDL axis notation
		gpc.axis_fire_R_conf = GAMEPAD_ITEM_ENABLED;
	}

	if (gpc.axis_fire_L) {
		gpc.axis_fire_L -= 1; // go back to SDL axis notation
		gpc.axis_fire_L_conf = GAMEPAD_ITEM_ENABLED;
	}

	gpc.button_fire_L = reader.GetInteger("gamepad", "button_fire_L", 0);
	gpc.button_fire_R = reader.GetInteger("gamepad", "button_fire_R", 0);
	gpc.controller_id = reader.GetInteger("gamepad", "controller_id", 0);
	gpc.button_spell = reader.GetInteger("gamepad", "button_spell", 0);
	gpc.button_minimap = reader.GetInteger("gamepad", "button_minimap", 0);
	gpc.button_fwd = reader.GetInteger("gamepad", "button_fwd", 0);
	gpc.button_back = reader.GetInteger("gamepad", "button_back", 0);
	gpc.button_pause_menu = reader.GetInteger("gamepad", "button_pause_menu", 0);
	gpc.button_esc = reader.GetInteger("gamepad", "button_esc", 0);
	gpc.button_menu_select = reader.GetInteger("gamepad", "button_menu_select", 0);

	gpc.axis_yaw_sensitivity = ReadZones(reader.GetString("gamepad", "axis_yaw_sensitivity", ""));
	gpc.axis_yaw_dead_zone = reader.GetInteger("gamepad", "axis_yaw_dead_zone", 3000);
	gpc.axis_pitch_sensitivity = ReadZones(reader.GetString("gamepad", "axis_pitch_sensitivity", ""));
	gpc.axis_pitch_dead_zone = reader.GetInteger("gamepad", "axis_pitch_dead_zone", 3000);
	gpc.axis_long_dead_zone = reader.GetInteger("gamepad", "axis_long_dead_zone", 3000);
	gpc.axis_trans_dead_zone = reader.GetInteger("gamepad", "axis_trans_dead_zone", 3000);
	gpc.axis_long_nav_dead_zone = reader.GetInteger("gamepad", "axis_long_nav_dead_zone", 6000);
	gpc.axis_trans_nav_dead_zone = reader.GetInteger("gamepad", "axis_trans_nav_dead_zone", 6000);

	gpc.trigger_dead_zone = reader.GetInteger("gamepad", "trigger_dead_zone", 3000);

	gpc.hat_nav = reader.GetInteger("gamepad", "hat_nav", GAMEPAD_ITEM_DISABLED);
	gpc.hat_mov = reader.GetInteger("gamepad", "hat_mov", GAMEPAD_ITEM_DISABLED);

	gp_temp = reader.GetBoolean("gamepad", "hat_nav_inv", 0);
	if (gpc.hat_nav) {
		gpc.hat_nav -= 1; // go back to SDL axis notation
		gpc.hat_nav_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = reader.GetBoolean("gamepad", "hat_mov_inv", 0);
	if (gpc.hat_mov) {
		gpc.hat_mov -= 1; // go back to SDL axis notation
		gpc.hat_mov_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gpc.haptic_enabled = reader.GetBoolean("gamepad", "haptic_enabled", false);
	gpc.haptic_gain_max = reader.GetInteger("gamepad", "haptic_max_gain", 75);

	return true;
};
