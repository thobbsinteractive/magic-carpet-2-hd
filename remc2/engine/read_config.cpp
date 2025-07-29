#include "read_config.h"

#include <cstdlib>
#include <ctype.h>
#include <filesystem>
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
    #include <direct.h>  
    #include <io.h>  
    #include <windows.h>
    #include "../portability/dirent-x.h"
#else
    #include "dirent.h"
#endif

#include "../portability/port_filesystem.h"
#include "../portability/port_sdl_sound.h"
#include "../portability/port_time.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

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
bool startWindowed = false;
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
float mouseScaleX = 1.0f;
float mouseScaleY = 1.0f;
bool disableLRButtonsMenuOpen = false;

gamepad_config_t gpc;
MouseInputMapping_t mouseMapping;
KeyboardInputMapping_t inputMapping;

std::string findConfigFile() {
	// find location of Config and read it
	std::vector<std::string> config_locations;
	if (CommandLineParams.GetConfigFilePath().length() > 0) {
		config_locations.push_back(CommandLineParams.GetConfigFilePath());
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
			config_locations.emplace_back(xdg_config_home_dir / "remc2" / "config.json");
		}
		if (std::filesystem::exists(home_dir)) {
			config_locations.emplace_back(home_dir / ".config" / "remc2" / "config.json");
		}
#else //__linux__
		auto home_drive = std::getenv("HOMEDRIVE");
		auto home_path = std::getenv("HOMEPATH");
		if (home_drive && home_path) {
			std::string home_dir = std::string(home_drive) + "/" + std::string(home_path);
			config_locations.push_back(home_dir + "/remc2/config.json");
		}
#endif //__linux__
		config_locations.push_back(get_exe_path() + "/config.json");
	}
	std::string configfile;
	// first location at which an configfile can be found is chosen
	for (auto configfile_location: config_locations) {
		if (std::filesystem::exists(configfile_location)) {
			configfile = configfile_location;
			break;
		}
	}

	return configfile;
}

std::vector<Maths::Zone> ReadZones(std::vector<Maths::Zone> zonesArray) {

	std::vector<Maths::Zone> zones;

	for (int i = 0; i < zonesArray.size(); i++) // Uses SizeType instead of size_t
	{
		zones.push_back(Maths::Zone{ zonesArray[i].m_xStart, zonesArray[i].m_xEnd, zonesArray[i].m_factor });
	}
	return zones;
}

bool SetConfig() {
	uint8_t gp_temp;
	std::string configFilePath = findConfigFile();
	if (std::filesystem::exists(configFilePath)) {
		if (CommandLineParams.DoShowDebugMessages1())
			std::cout << "Using Config File: " << configFilePath << "\n";
	}
	else {
		if (CommandLineParams.DoShowDebugMessages1())
			std::cout << "Config File cannot be found... Exiting\n";
		return false;
	}

	auto config = Config(configFilePath);

	//Game
	loggingLevel = config.m_Game.m_LoggingLevel;
	maxGameFps = config.m_Game.m_MaxGameFps;
	fmvFps = config.m_Game.m_FmvFps;
	if (config.m_Game.m_SkipIntro)
		config_skip_screen = 1;
	else
		config_skip_screen = 0;

	//Path
	strcpy((char*)gameFolder, config.m_Paths.m_GameFolder.c_str());
	strcpy((char*)cdFolder, config.m_Paths.m_CdFolder.c_str());

	//Sound
	hqsound = config.m_Sound.m_HqSound;
	fixspeedsound = config.m_Sound.m_FixSpeedSound;
	if (config.m_Sound.m_OggMusic)
	{
		oggmusic = true;
		hqsound = true;//for mp3 music must be activate hqsound
	}
	else
		oggmusic = false;
	oggmusicalternative = config.m_Sound.m_OggMusicAlternative;
	strcpy(oggmusicFolder, config.m_Sound.m_OggFolder.c_str());

	//Graphics
	displayIndex = config.m_Graphics.m_DisplayIndex;
	windowResWidth = config.m_Graphics.m_WindowResWidth;
	windowResHeight = config.m_Graphics.m_WindowResHeight;
	if (windowResWidth < 640 || windowResHeight < 480)
	{
		windowResWidth = 640;
		windowResHeight = 480;
	}
	maintainAspectRatio = config.m_Graphics.m_MaintainAspectRatio;
	startWindowed = config.m_Graphics.m_StartWindowed;
	gameResWidth = config.m_Graphics.m_GameDetail.m_GameResWidth;
	gameResHeight = config.m_Graphics.m_GameDetail.m_GameResHeight;
	if (gameResWidth < 320 || gameResHeight < 200)
	{
		gameResWidth = 320;
		gameResHeight = 200;
	}
	gameUiScale = config.m_Graphics.m_GameDetail.m_GameUiScale;
	if (gameUiScale < 1)
		gameUiScale = 1;
	if (gameUiScale > 8 || (640 * gameUiScale) > gameResWidth)
	{
		while (gameUiScale > 1 && (640 * gameUiScale) > gameResWidth)
		{
			gameUiScale--;
		}
	}
	strcpy(bigGraphicsFolder, config.m_Graphics.m_GameDetail.m_BigGraphicsFolder.c_str());
	if (config.m_Graphics.m_GameDetail.m_UseEnhancedGraphics && strlen(bigGraphicsFolder) > 0
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
	sky = config.m_Graphics.m_GameDetail.m_Sky;
	reflections = config.m_Graphics.m_GameDetail.m_Reflections;
	dynamicLighting = config.m_Graphics.m_GameDetail.m_DynamicLighting;
	multiThreadedRender = config.m_Graphics.m_Threading.m_isActive;
	numberOfRenderThreads = config.m_Graphics.m_Threading.m_NumberOfRenderThreads;
	if (multiThreadedRender)
	{
		assignToSpecificCores = config.m_Graphics.m_Threading.m_AssignToSpecificCores;

		if (numberOfRenderThreads < 1)
		{
			numberOfRenderThreads = 1;
		}
	}
	else
	{
		numberOfRenderThreads = 0;
	}

	//Controls
	invertYAxis = config.m_Controls.m_Mouse.m_InvertYAxis;
	invertXAxis = config.m_Controls.m_Mouse.m_InvertXAxis;

	//Mouse
	mouseScaleX = config.m_Controls.m_Mouse.m_mouseScaleX;
	mouseScaleY = config.m_Controls.m_Mouse.m_mouseScaleY;
	disableLRButtonsMenuOpen = config.m_Controls.m_Mouse.m_disableLRButtonsMenuOpen;
	mouseMapping.SpellLeft = config.m_Controls.m_Mouse.m_spellLeft;
	mouseMapping.SpellRight = config.m_Controls.m_Mouse.m_spellRight;
	mouseMapping.map = config.m_Controls.m_Mouse.m_map;
	mouseMapping.SpellMenu = config.m_Controls.m_Mouse.m_spellMenu;
	mouseMapping.SpellMenuMark = config.m_Controls.m_Mouse.m_spellMenuMark;

	//Keyboard
	inputMapping.Forward = config.m_Controls.m_Keyboard.m_forward;
	inputMapping.Backwards = config.m_Controls.m_Keyboard.m_backwards;
	inputMapping.Left = config.m_Controls.m_Keyboard.m_left;
	inputMapping.Right = config.m_Controls.m_Keyboard.m_right;
	inputMapping.Map = config.m_Controls.m_Keyboard.m_map;
	inputMapping.SpellMenu = config.m_Controls.m_Keyboard.m_spellMenu;
	inputMapping.SpellMenuMark = config.m_Controls.m_Keyboard.m_spellMenuMark;

	gpc.axis_yaw = config.m_Controls.m_GamePad.m_AxisYaw;
	gpc.axis_pitch = config.m_Controls.m_GamePad.m_AxisPitch;
	gpc.axis_long = config.m_Controls.m_GamePad.m_AxisLong;
	gpc.axis_trans = config.m_Controls.m_GamePad.m_AxisTrans;
	gpc.axis_nav_ns = config.m_Controls.m_GamePad.m_AxisNavNs;
	gpc.axis_nav_ew = config.m_Controls.m_GamePad.m_AxisNavEw;
	gpc.axis_fire_R = config.m_Controls.m_GamePad.m_AxisFireR;
	gpc.axis_fire_L = config.m_Controls.m_GamePad.m_AxisFireL;

	gp_temp = config.m_Controls.m_GamePad.m_AxisYawInv;
	if (gpc.axis_yaw) {
		gpc.axis_yaw -= 1; // go back to SDL axis notation
		gpc.axis_yaw_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = config.m_Controls.m_GamePad.m_AxisPitchInv;
	if (gpc.axis_pitch) {
		gpc.axis_pitch -= 1; // go back to SDL axis notation
		gpc.axis_pitch_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = config.m_Controls.m_GamePad.m_AxisLongInv;
	if (gpc.axis_long) {
		gpc.axis_long -= 1; // go back to SDL axis notation
		gpc.axis_long_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = config.m_Controls.m_GamePad.m_AxisTransInv;
	if (gpc.axis_trans) {
		gpc.axis_trans -= 1; // go back to SDL axis notation
		gpc.axis_trans_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = config.m_Controls.m_GamePad.m_AxisNavNsInv;
	if (gpc.axis_nav_ns) {
		gpc.axis_nav_ns -= 1; // go back to SDL axis notation
		gpc.axis_nav_ns_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = config.m_Controls.m_GamePad.m_AxisNavEwInv;
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

	gpc.controller_id = config.m_Controls.m_GamePad.m_ControllerId;
	gpc.button_fire_L = config.m_Controls.m_GamePad.m_ButtonFireL;
	gpc.button_fire_R = config.m_Controls.m_GamePad.m_ButtonFireR;
	gpc.button_spell = config.m_Controls.m_GamePad.m_ButtonSpell;
	gpc.button_minimap = config.m_Controls.m_GamePad.m_ButtonMiniMap;
	gpc.button_fwd = config.m_Controls.m_GamePad.m_ButtonFwd;
	gpc.button_back = config.m_Controls.m_GamePad.m_ButtonBack;
	gpc.button_pause_menu = config.m_Controls.m_GamePad.m_ButtonPauseMenu;
	gpc.button_esc = config.m_Controls.m_GamePad.m_ButtonEsc;
	gpc.button_menu_select = config.m_Controls.m_GamePad.m_ButtonMenuSelect;

	gpc.axis_yaw_sensitivity = ReadZones(config.m_Controls.m_GamePad.m_AxisYawSensitivity);
	gpc.axis_yaw_dead_zone = config.m_Controls.m_GamePad.m_AxisYawDeadZone;
	gpc.axis_pitch_sensitivity = ReadZones(config.m_Controls.m_GamePad.m_AxisPitchSensitivity);
	gpc.axis_pitch_dead_zone = config.m_Controls.m_GamePad.m_AxisPitchDeadZone;
	gpc.axis_long_dead_zone = config.m_Controls.m_GamePad.m_AxisLongDeadZone;
	gpc.axis_trans_dead_zone = config.m_Controls.m_GamePad.m_AxisTransDeadZone;

	gpc.axis_long_nav_dead_zone = config.m_Controls.m_GamePad.m_AxisLongNavDeadZone;
	gpc.axis_trans_nav_dead_zone = config.m_Controls.m_GamePad.m_AxisTransNavDeadZone;

	gpc.trigger_dead_zone = config.m_Controls.m_GamePad.m_TriggerDeadZone;

	gpc.hat_nav = config.m_Controls.m_GamePad.m_HatNav;
	gpc.hat_mov = config.m_Controls.m_GamePad.m_HatMov;

	gp_temp = config.m_Controls.m_GamePad.m_HatNavInv;
	if (gpc.hat_nav) {
		gpc.hat_nav -= 1; // go back to SDL axis notation
		gpc.hat_nav_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gp_temp = config.m_Controls.m_GamePad.m_HatMovInv;
	if (gpc.hat_mov) {
		gpc.hat_mov -= 1; // go back to SDL axis notation
		gpc.hat_mov_conf = GAMEPAD_ITEM_ENABLED | (gp_temp ? GAMEPAD_AXIS_INVERTED : 0);
	}

	gpc.haptic_enabled = config.m_Controls.m_GamePad.m_HapticEnabled;
	gpc.haptic_gain_max = config.m_Controls.m_GamePad.m_HapticMaxGain;

	return true;
};
