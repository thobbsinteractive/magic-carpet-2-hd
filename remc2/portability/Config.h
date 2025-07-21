#pragma once
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "../utilities/Maths.h"

class Config
{
private:
	struct Paths
	{
		std::string m_GameFolder = "";
		std::string m_CdFolder = "";
	};

	struct Sound
	{
		bool m_HqSound = true;
		bool m_OggMusic = true;
		std::string m_OggFolder;
		bool m_OggMusicAlternative = false;
		bool m_FixSpeedSound = true;
	};

	struct GameDetail
	{
		int m_GameResWidth = 640;
		int m_GameResHeight = 480;
		int m_GameUiScale = 1;
		bool m_UseEnhancedGraphics = false;
		std::string m_BigGraphicsFolder = "";
		bool m_Sky = true;
		bool m_Reflections = true;
		bool m_DynamicLighting = true;
	};

	struct Threading
	{
		bool m_isActive = false;
		int m_NumberOfRenderThreads = 0;
		bool m_AssignToSpecificCores = false;
	};

	struct Graphics
	{
		int m_DisplayIndex = 0;
		int m_WindowResWidth = 640;
		int m_WindowResHeight = 480;	
		bool m_MaintainAspectRatio = true;
		bool m_StartWindowed = false;
		std::string m_ForceRender = "";
		GameDetail m_GameDetail;
		Threading m_Threading;
	};

	struct Game
	{
		std::string m_LoggingLevel = "Info";
		int m_MaxGameFps = 0;
		int m_FmvFps = 20;
		bool m_SkipIntro = false;
	};

	struct Mouse
	{
		bool m_InvertYAxis = true;
		bool m_InvertXAxis = false;
		uint16_t m_spellLeft = 1;
		uint16_t m_spellRight = 2;
		uint16_t m_spellMenu = 4;
		uint16_t m_markSpell = 3;
	};

	struct Keyboard
	{
		uint16_t m_forward = 0;
		uint16_t m_backwards = 0;
		uint16_t m_left = 0;
		uint16_t m_right = 0;
		uint16_t m_map = 0;
		uint16_t m_spellMenu = 0;
		uint16_t m_markSpell = 0;
	};

	struct GamePad
	{
		std::string m_Name = "";
		uint16_t m_ControllerId = 0;
		uint16_t m_ButtonMiniMap = 0;
		uint16_t m_ButtonSpell = 0;
		uint16_t m_ButtonPauseMenu = 0;
		uint16_t m_ButtonEsc = 0;
		uint16_t m_ButtonFireL = 0;
		uint16_t m_ButtonFireR = 0;
		uint16_t m_ButtonFwd = 0;
		uint16_t m_ButtonBack = 0;
		uint16_t m_ButtonMenuSelect = 0;
		uint16_t m_TriggerDeadZone = 3000;
		bool m_HapticEnabled = false;
		uint16_t m_HapticMaxGain = 75;
		uint16_t m_HatNav = 0;
		uint16_t m_HatMov = 0;
		bool m_HatNavInv = false;
		bool m_HatMovInv = false;
		uint16_t m_AxisLong = 0;
		uint16_t m_AxisLongDeadZone = 3000;
		uint16_t m_AxisLongNavDeadZone = 6000;
		bool m_AxisLongInv = false;
		uint16_t m_AxisTrans = 0;
		bool m_AxisTransInv = false;
		uint16_t m_AxisTransDeadZone = 3000;
		uint16_t m_AxisTransNavDeadZone = 6000;
		uint16_t m_AxisNavNs = 0;
		bool m_AxisNavNsInv = false;
		uint16_t m_AxisNavEw = 0;
		bool m_AxisNavEwInv = false;
		uint16_t m_AxisFireR = 0;
		uint16_t m_AxisFireL = 0;
		uint16_t m_AxisYaw = 0;
		bool m_AxisYawInv = false;
		uint16_t m_AxisYawDeadZone = 3000;
		std::vector<Maths::Zone> m_AxisYawSensitivity;
		uint16_t m_AxisPitch = 0;
		bool m_AxisPitchInv = false;
		uint16_t m_AxisPitchDeadZone = 3000;
		std::vector<Maths::Zone> m_AxisPitchSensitivity;
	};

	struct Controls
	{
		Mouse m_Mouse;
		Keyboard m_Keyboard;
		GamePad m_GamePad;
	};

	void LoadSettings(rapidjson::Document& document);
	void LoadGraphics(rapidjson::GenericObject<false, rapidjson::Value>& settings);
	void LoadGameDetail(rapidjson::GenericObject<false, rapidjson::Value>& graphics);
	void LoadThreading(rapidjson::GenericObject<false, rapidjson::Value>& graphics);
	void LoadSound(rapidjson::GenericObject<false, rapidjson::Value>& settings);
	void LoadPaths(rapidjson::GenericObject<false, rapidjson::Value>& settings);
	void LoadControls(rapidjson::GenericObject<false, rapidjson::Value>& settings);
	void LoadGame(rapidjson::GenericObject<false, rapidjson::Value>& settings);
	std::string ReadStringValue(rapidjson::GenericObject<false, rapidjson::Value>& settings, const char* name);
	int ReadIntValue(rapidjson::GenericObject<false, rapidjson::Value>& settings, const char* name);
	bool ReadBoolValue(rapidjson::GenericObject<false, rapidjson::Value>& settings, const char* name);
	SDL_Scancode ReadKeyScancode(rapidjson::GenericObject<false, rapidjson::Value>& settings, const char* name);
	std::string ReadFileToString(std::string fileName);

public:
	std::string m_Name;
	std::string m_Version;
	Paths m_Paths;
	Sound m_Sound;
	Graphics m_Graphics;
	Game m_Game;
	Controls m_Controls;

	Config(std::string fileName);
};

#endif //CONFIG_H