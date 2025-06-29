#include "Config.h"
#include <filesystem>
#include <iostream>
#include <fstream>

Config::Config(std::string fileName)
{
	if (std::filesystem::exists(fileName))
	{
		auto json = ReadFileToString(fileName);

		if (json.size() > 0)
		{
			rapidjson::Document document;
			document.Parse(json.c_str());
			if (document.HasMember("settings"))
			{
				LoadSettings(document);
			}
		}
	}
}

std::string Config::ReadStringValue(rapidjson::GenericObject<false, rapidjson::Value>& settings, const char* name)
{
	if (settings.HasMember(name))
	{
		return settings[name].GetString();
	}
	return "";
}

int Config::ReadIntValue(rapidjson::GenericObject<false, rapidjson::Value>& settings, const char* name)
{
	if (settings.HasMember(name))
	{
		return settings[name].GetInt();
	}
	return 0;
}

bool Config::ReadBoolValue(rapidjson::GenericObject<false, rapidjson::Value>& settings, const char* name)
{
	if (settings.HasMember(name))
	{
		return settings[name].GetBool();
	}
	return false;
}

void Config::LoadSettings(rapidjson::Document& document)
{
	auto settingsArray = document["settings"].GetArray();

	for (int i = 0; i < settingsArray.Size(); i++)
	{
#ifdef __linux__
		auto settings = settingsArray[i].GetObject();
#else
		auto settings = settingsArray[i].GetObj();
#endif
		if (settings.HasMember("name") && settings.HasMember("isActive") && settings["isActive"].GetBool() == true)
		{
			m_Name = ReadStringValue(settings, "name");
			m_Version = ReadStringValue(settings, "version");
			LoadPaths(settings);
			LoadSound(settings);
			LoadGraphics(settings);
			LoadGame(settings);
			LoadControls(settings);
			break;
		}
	}
}

void Config::LoadGame(rapidjson::GenericObject<false, rapidjson::Value>& settings)
{
	if (settings.HasMember("game"))
	{
#ifdef __linux__
		auto game = settings["game"].GetObject();
#else
		auto game = settings["game"].GetObj();
#endif
		m_Game.m_LoggingLevel = ReadStringValue(game, "loggingLevel");
		m_Game.m_MaxGameFps = ReadIntValue(game, "maxGameFps");
		m_Game.m_FmvFps = ReadIntValue(game, "fmvFps");
	}
}

void Config::LoadControls(rapidjson::GenericObject<false, rapidjson::Value>& settings)
{
	if (settings.HasMember("controls"))
	{
#ifdef __linux__
		auto controls = settings["controls"].GetObject();
#else
		auto controls = settings["controls"].GetObj();
#endif

		m_Controls.m_InvertYAxis = ReadBoolValue(controls, "invertYAxis");
		m_Controls.m_InvertXAxis = ReadBoolValue(controls, "invertXAxis");

		if (controls.HasMember("gamePad"))
		{
			auto gamePadArray = controls["gamePad"].GetArray();

			for (int i = 0; i < gamePadArray.Size(); i++)
			{
#ifdef __linux__
				auto gamePad = gamePadArray[i].GetObject();
#else
				auto gamePad = gamePadArray[i].GetObj();
#endif
				if (gamePad.HasMember("isActive") && gamePad["isActive"].GetBool() == true)
				{
					m_Controls.m_GamePad.m_Name = ReadStringValue(gamePad, "name");
					m_Controls.m_GamePad.m_ButtonMiniMap = (uint16_t)ReadIntValue(gamePad, "buttonMiniMap");
					m_Controls.m_GamePad.m_ButtonSpell = (uint16_t)ReadIntValue(gamePad, "buttonSpell");
					m_Controls.m_GamePad.m_ButtonPauseMenu = (uint16_t)ReadIntValue(gamePad, "buttonPauseMenu");
					m_Controls.m_GamePad.m_ButtonEsc = (uint16_t)ReadIntValue(gamePad, "buttonEsc");
					m_Controls.m_GamePad.m_ButtonFireL = (uint16_t)ReadIntValue(gamePad, "buttonFireL");
					m_Controls.m_GamePad.m_ButtonFireR = (uint16_t)ReadIntValue(gamePad, "buttonFireR");
					m_Controls.m_GamePad.m_ButtonMenuSelect = (uint16_t)ReadIntValue(gamePad, "buttonMenuSelect");
					m_Controls.m_GamePad.m_TriggerDeadZone = (uint16_t)ReadIntValue(gamePad, "triggerDeadZone");
					m_Controls.m_GamePad.m_HapticEnabled = ReadBoolValue(gamePad, "hapticEnabled");
					m_Controls.m_GamePad.m_HapticMaxGain = (uint16_t)ReadIntValue(gamePad, "hapticMaxGain");
					m_Controls.m_GamePad.m_HatNav = (uint16_t)ReadIntValue(gamePad, "hatNav");
					m_Controls.m_GamePad.m_HatMov = (uint16_t)ReadIntValue(gamePad, "hatMov");
					m_Controls.m_GamePad.m_HatNavInv = ReadBoolValue(gamePad, "hatNavInv");
					m_Controls.m_GamePad.m_HatMovInv = ReadBoolValue(gamePad, "hatMovInv");
					m_Controls.m_GamePad.m_AxisLong = (uint16_t)ReadIntValue(gamePad, "axisLong");
					m_Controls.m_GamePad.m_AxisLongDeadZone = (uint16_t)ReadIntValue(gamePad, "axisLongDeadZone");
					m_Controls.m_GamePad.m_AxisLongNavDeadZone = (uint16_t)ReadIntValue(gamePad, "axisLongNavDeadZone");
					m_Controls.m_GamePad.m_AxisLongNavDeadZone = (uint16_t)ReadIntValue(gamePad, "axisLongNavDeadZone");
					m_Controls.m_GamePad.m_AxisLongInv = ReadBoolValue(gamePad, "axisLongInv");
					m_Controls.m_GamePad.m_AxisTrans = (uint16_t)ReadIntValue(gamePad, "axisTrans");
					m_Controls.m_GamePad.m_AxisTransDeadZone = (uint16_t)ReadIntValue(gamePad, "axisTransDeadZone");
					m_Controls.m_GamePad.m_AxisLongNavDeadZone = (uint16_t)ReadIntValue(gamePad, "axisTransNavDeadZone");
					m_Controls.m_GamePad.m_AxisNavNs = (uint16_t)ReadIntValue(gamePad, "axisNavNs");
					m_Controls.m_GamePad.m_AxisNavNsInv = ReadBoolValue(gamePad, "axisNavNsInv");
					m_Controls.m_GamePad.m_AxisNavEw = (uint16_t)ReadIntValue(gamePad, "axisNavEw");
					m_Controls.m_GamePad.m_AxisNavEwInv = ReadBoolValue(gamePad, "axisNavEwInv");
					m_Controls.m_GamePad.m_AxisFireR = (uint16_t)ReadIntValue(gamePad, "axisFireR");
					m_Controls.m_GamePad.m_AxisFireL = (uint16_t)ReadIntValue(gamePad, "axisFireL");
					m_Controls.m_GamePad.m_AxisYaw = (uint16_t)ReadIntValue(gamePad, "axisYaw");
					m_Controls.m_GamePad.m_AxisYawInv = ReadBoolValue(gamePad, "axisYawInv");
					m_Controls.m_GamePad.m_AxisYawDeadZone = (uint16_t)ReadIntValue(gamePad, "axisYawDeadZone");

					if (gamePad.HasMember("axisYawSensitivity"))
					{
#ifdef __linux__
						auto axisYawSensitivity = gamePad["axisYawSensitivity"].GetObject();
#else
						auto axisYawSensitivity = gamePad["axisYawSensitivity"].GetObj();
#endif
						if (axisYawSensitivity.HasMember("zones"))
						{
							auto zonesArray = axisYawSensitivity["zones"].GetArray();

							for (int z = 0; z < zonesArray.Size(); z++) // Uses SizeType instead of size_t
							{
#ifdef __linux__
								auto zone = zonesArray[z].GetObject();
#else
								auto zone = zonesArray[z].GetObj();
#endif
								if (zone.HasMember("start") && zone.HasMember("end") && zone.HasMember("factor"))
								{
									m_Controls.m_GamePad.m_AxisYawSensitivity.push_back(Maths::Zone{ (uint16_t)zone["start"].GetInt(), (uint16_t)zone["end"].GetInt(), zone["factor"].GetDouble() });
								}
							}
						}
					}

					m_Controls.m_GamePad.m_AxisPitch = (uint16_t)ReadIntValue(gamePad, "axisPitch");
					m_Controls.m_GamePad.m_AxisPitchInv = ReadBoolValue(gamePad, "axisPitchInv");
					m_Controls.m_GamePad.m_AxisPitchDeadZone = (uint16_t)ReadIntValue(gamePad, "axisPitchDeadZone");

					if (gamePad.HasMember("axisPitchSensitivity"))
					{
#ifdef __linux__
						auto axisPitchSensitivity = gamePad["axisPitchSensitivity"].GetObject();
#else
						auto axisPitchSensitivity = gamePad["axisPitchSensitivity"].GetObj();
#endif
						if (axisPitchSensitivity.HasMember("zones"))
						{
							auto zonesArray = axisPitchSensitivity["zones"].GetArray();

							for (int z = 0; z < zonesArray.Size(); z++) // Uses SizeType instead of size_t
							{
#ifdef __linux__
								auto zone = zonesArray[z].GetObject();
#else
								auto zone = zonesArray[z].GetObj();
#endif
								if (zone.HasMember("start") && zone.HasMember("end") && zone.HasMember("factor"))
								{
									m_Controls.m_GamePad.m_AxisPitchSensitivity.push_back(Maths::Zone{ (uint16_t)zone["start"].GetInt(), (uint16_t)zone["end"].GetInt(), zone["factor"].GetDouble() });
								}
							}
						}
					}
					break;
				}
			}
		}
	}
}

void Config::LoadGraphics(rapidjson::GenericObject<false, rapidjson::Value>& settings)
{
	if (settings.HasMember("graphics"))
	{
#ifdef __linux__
		auto graphics = settings["graphics"].GetObject();
#else
		auto graphics = settings["graphics"].GetObj();
#endif
		m_Graphics.m_DisplayIndex = ReadIntValue(graphics, "displayIndex");
		m_Graphics.m_WindowResWidth = ReadIntValue(graphics, "windowResWidth");
		m_Graphics.m_WindowResHeight = ReadIntValue(graphics, "windowResHeight");
		m_Graphics.m_MaintainAspectRatio = ReadBoolValue(graphics, "maintainAspectRatio");
		m_Graphics.m_StartWindowed = ReadBoolValue(graphics, "startWindowed");

		LoadGameDetail(graphics);
		LoadThreading(graphics);
	}
}

void Config::LoadGameDetail(rapidjson::GenericObject<false, rapidjson::Value>& graphics)
{
	if (graphics.HasMember("gameDetail"))
	{
#ifdef __linux__
		auto gameDetail = graphics["gameDetail"].GetObject();
#else
		auto gameDetail = graphics["gameDetail"].GetObj();
#endif
		m_Graphics.m_GameDetail.m_GameResWidth = ReadIntValue(gameDetail, "gameResWidth");
		m_Graphics.m_GameDetail.m_GameResHeight = ReadIntValue(gameDetail, "gameResHeight");
		m_Graphics.m_GameDetail.m_GameUiScale = ReadIntValue(gameDetail, "gameUiScale");
		m_Graphics.m_GameDetail.m_UseEnhancedGraphics = ReadBoolValue(gameDetail, "useEnhancedGraphics");
		m_Graphics.m_GameDetail.m_BigGraphicsFolder = ReadStringValue(gameDetail, "bigGraphicsFolder");
		m_Graphics.m_GameDetail.m_Sky = ReadBoolValue(gameDetail, "sky");
		m_Graphics.m_GameDetail.m_Reflections = ReadBoolValue(gameDetail, "reflections");
		m_Graphics.m_GameDetail.m_DynamicLighting = ReadBoolValue(gameDetail, "dynamicLighting");
	}
}

void Config::LoadThreading(rapidjson::GenericObject<false, rapidjson::Value>& graphics)
{
#ifdef __linux__
	auto threading = graphics["threading"].GetObject();
#else
	auto threading = graphics["threading"].GetObj();
#endif
	if (graphics.HasMember("threading"))
	{
		m_Graphics.m_Threading.m_isActive = threading["isActive"].GetBool();
		if (m_Graphics.m_Threading.m_isActive)
		{
			m_Graphics.m_Threading.m_NumberOfRenderThreads = (uint8_t)ReadIntValue(threading,"numberOfRenderThreads");
			m_Graphics.m_Threading.m_AssignToSpecificCores = ReadBoolValue(threading, "assignToSpecificCores");
		}
	}
}

void Config::LoadSound(rapidjson::GenericObject<false, rapidjson::Value>& settings)
{
	if (settings.HasMember("sound"))
	{
#ifdef __linux__
		auto sound = settings["sound"].GetObject();
#else
		auto sound = settings["sound"].GetObj();
#endif
		m_Sound.m_HqSound = ReadBoolValue(sound, "hqSound");
		m_Sound.m_OggMusic = ReadBoolValue(sound, "oggMusic");
		m_Sound.m_OggFolder = ReadStringValue(sound, "oggFolder");
		m_Sound.m_OggMusicAlternative = ReadBoolValue(sound, "oggMusicAlternative");
		m_Sound.m_FixSpeedSound = ReadBoolValue(sound, "fixSpeedSound");
	}
}

void Config::LoadPaths(rapidjson::GenericObject<false, rapidjson::Value>& settings)
{
	if (settings.HasMember("paths"))
	{
#ifdef __linux__
		auto paths = settings["paths"].GetObject();
#else
		auto paths = settings["paths"].GetObj();
#endif
		m_Paths.m_GameFolder = ReadStringValue(paths, "gameFolder");
		m_Paths.m_CdFolder = ReadStringValue(paths, "cdFolder");
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
