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
			m_Name = settings["name"].GetString();
			m_Version = settings["version"].GetString();
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
		if (game.HasMember("loggingLevel"))
			m_Game.m_LoggingLevel = game["loggingLevel"].GetString();

		if (game.HasMember("maxGameFps"))
			m_Game.m_MaxGameFps = game["maxGameFps"].GetInt();

		if (game.HasMember("fmvFps"))
			m_Game.m_FmvFps = game["fmvFps"].GetInt();
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

		if (controls.HasMember("invertYAxis"))
			m_Controls.m_InvertYAxis = controls["invertYAxis"].GetBool();

		if (controls.HasMember("invertXAxis"))
			m_Controls.m_InvertXAxis = controls["invertXAxis"].GetBool();

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
					m_Controls.m_GamePad.m_Name = settings["name"].GetString();

					if (gamePad.HasMember("buttonMiniMap"))
						m_Controls.m_GamePad.m_ButtonMiniMap = (uint16_t)gamePad["buttonMiniMap"].GetInt();

					if (gamePad.HasMember("buttonSpell"))
						m_Controls.m_GamePad.m_ButtonSpell = (uint16_t)gamePad["buttonSpell"].GetInt();

					if (gamePad.HasMember("buttonPauseMenu"))
						m_Controls.m_GamePad.m_ButtonPauseMenu = (uint16_t)gamePad["buttonPauseMenu"].GetInt();

					if (gamePad.HasMember("buttonEsc"))
						m_Controls.m_GamePad.m_ButtonEsc = (uint16_t)gamePad["buttonEsc"].GetInt();

					if (gamePad.HasMember("buttonFireL"))
						m_Controls.m_GamePad.m_ButtonFireL = (uint16_t)gamePad["buttonFireL"].GetInt();

					if (gamePad.HasMember("buttonFireR"))
						m_Controls.m_GamePad.m_ButtonFireR = (uint16_t)gamePad["buttonFireR"].GetInt();

					if (gamePad.HasMember("buttonMenuSelect"))
						m_Controls.m_GamePad.m_ButtonMenuSelect = (uint16_t)gamePad["buttonMenuSelect"].GetInt();

					if (gamePad.HasMember("triggerDeadZone"))
						m_Controls.m_GamePad.m_TriggerDeadZone = (uint16_t)gamePad["triggerDeadZone"].GetInt();

					if (gamePad.HasMember("hapticEnabled"))
						m_Controls.m_GamePad.m_HapticEnabled = gamePad["hapticEnabled"].GetBool();

					if (gamePad.HasMember("hapticMaxGain"))
						m_Controls.m_GamePad.m_HapticMaxGain = (uint16_t)gamePad["hapticMaxGain"].GetInt();

					if (gamePad.HasMember("hatNav"))
						m_Controls.m_GamePad.m_HatNav = (uint16_t)gamePad["hatNav"].GetInt();

					if (gamePad.HasMember("hatMov"))
						m_Controls.m_GamePad.m_HatMov = (uint16_t)gamePad["hatMov"].GetInt();

					if (gamePad.HasMember("hatNavInv"))
						m_Controls.m_GamePad.m_HatNavInv = gamePad["hatNavInv"].GetBool();

					if (gamePad.HasMember("hatMovInv"))
						m_Controls.m_GamePad.m_HatMovInv = gamePad["hatMovInv"].GetBool();

					if (gamePad.HasMember("axisLong"))
						m_Controls.m_GamePad.m_AxisLong = (uint16_t)gamePad["axisLong"].GetInt();

					if (gamePad.HasMember("axisLongDeadZone"))
						m_Controls.m_GamePad.m_AxisLongDeadZone = (uint16_t)gamePad["axisLongDeadZone"].GetInt();

					if (gamePad.HasMember("axisLongNavDeadZone"))
						m_Controls.m_GamePad.m_AxisLongNavDeadZone = (uint16_t)gamePad["axisLongNavDeadZone"].GetInt();
	
					if (gamePad.HasMember("axisLongNavDeadZone"))
						m_Controls.m_GamePad.m_AxisLongNavDeadZone = (uint16_t)gamePad["axisLongNavDeadZone"].GetInt();

					if (gamePad.HasMember("axisLongInv"))
						m_Controls.m_GamePad.m_AxisLongInv = gamePad["axisLongInv"].GetBool();

					if (gamePad.HasMember("axisTrans"))
						m_Controls.m_GamePad.m_AxisTrans = (uint16_t)gamePad["axisTrans"].GetInt();

					if (gamePad.HasMember("axisTransDeadZone"))
						m_Controls.m_GamePad.m_AxisTransDeadZone = (uint16_t)gamePad["axisTransDeadZone"].GetInt();

					if (gamePad.HasMember("axisTransNavDeadZone"))
						m_Controls.m_GamePad.m_AxisLongNavDeadZone = (uint16_t)gamePad["axisTransNavDeadZone"].GetInt();

					if (gamePad.HasMember("axisNavNs"))
						m_Controls.m_GamePad.m_AxisNavNs = (uint16_t)gamePad["axisNavNs"].GetInt();

					if (gamePad.HasMember("axisNavNsInv"))
						m_Controls.m_GamePad.m_AxisNavNsInv = gamePad["axisNavNsInv"].GetBool();

					if (gamePad.HasMember("axisNavEw"))
						m_Controls.m_GamePad.m_AxisNavEw = (uint16_t)gamePad["axisNavEw"].GetInt();

					if (gamePad.HasMember("axisNavEwInv"))
						m_Controls.m_GamePad.m_AxisNavEwInv = gamePad["axisNavEwInv"].GetBool();

					if (gamePad.HasMember("axisFireR"))
						m_Controls.m_GamePad.m_AxisFireR = (uint16_t)gamePad["axisFireR"].GetInt();

					if (gamePad.HasMember("axisFireL"))
						m_Controls.m_GamePad.m_AxisFireL = (uint16_t)gamePad["axisFireL"].GetInt();

					if (gamePad.HasMember("axisYaw"))
						m_Controls.m_GamePad.m_AxisYaw = (uint16_t)gamePad["axisYaw"].GetInt();

					if (gamePad.HasMember("axisYawInv"))
						m_Controls.m_GamePad.m_AxisYawInv = gamePad["axisYawInv"].GetBool();

					if (gamePad.HasMember("axisYawDeadZone"))
						m_Controls.m_GamePad.m_AxisYawDeadZone = (uint16_t)gamePad["axisYawDeadZone"].GetInt();

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

					if (gamePad.HasMember("axisPitch"))
						m_Controls.m_GamePad.m_AxisPitch = (uint16_t)gamePad["axisPitch"].GetInt();

					if (gamePad.HasMember("axisPitchInv"))
						m_Controls.m_GamePad.m_AxisPitchInv = gamePad["axisPitchInv"].GetBool();

					if (gamePad.HasMember("axisPitchDeadZone"))
						m_Controls.m_GamePad.m_AxisPitchDeadZone = (uint16_t)gamePad["axisPitchDeadZone"].GetInt();

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
		if (graphics.HasMember("displayIndex"))
			m_Graphics.m_DisplayIndex = graphics["displayIndex"].GetInt();

		if (graphics.HasMember("windowResWidth"))
			m_Graphics.m_WindowResWidth = graphics["windowResWidth"].GetInt();

		if (graphics.HasMember("windowResHeight"))
			m_Graphics.m_WindowResHeight = graphics["windowResHeight"].GetInt();

		if (graphics.HasMember("maintainAspectRatio"))
			m_Graphics.m_MaintainAspectRatio = graphics["maintainAspectRatio"].GetBool();

		if (graphics.HasMember("startWindowed"))
			m_Graphics.m_StartWindowed = graphics["startWindowed"].GetBool();

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
		if (gameDetail.HasMember("gameResWidth"))
			m_Graphics.m_GameDetail.m_GameResWidth = gameDetail["gameResWidth"].GetInt();

		if (gameDetail.HasMember("gameResHeight"))
			m_Graphics.m_GameDetail.m_GameResHeight = gameDetail["gameResHeight"].GetInt();

		if (gameDetail.HasMember("gameUiScale"))
			m_Graphics.m_GameDetail.m_GameUiScale = gameDetail["gameUiScale"].GetInt();

		if (gameDetail.HasMember("useEnhancedGraphics"))
			m_Graphics.m_GameDetail.m_UseEnhancedGraphics = gameDetail["useEnhancedGraphics"].GetBool();

		if (gameDetail.HasMember("bigGraphicsFolder"))
			m_Graphics.m_GameDetail.m_BigGraphicsFolder = gameDetail["bigGraphicsFolder"].GetString();

		if (gameDetail.HasMember("sky"))
			m_Graphics.m_GameDetail.m_Sky = gameDetail["sky"].GetBool();

		if (gameDetail.HasMember("reflections"))
			m_Graphics.m_GameDetail.m_Reflections = gameDetail["reflections"].GetBool();

		if (gameDetail.HasMember("dynamicLighting"))
			m_Graphics.m_GameDetail.m_DynamicLighting = gameDetail["dynamicLighting"].GetBool();
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
		if (threading.HasMember("isActive"))
			m_Graphics.m_Threading.m_isActive = threading["isActive"].GetBool();

		if (threading.HasMember("numberOfRenderThreads"))
			m_Graphics.m_Threading.m_NumberOfRenderThreads = (uint8_t)threading["numberOfRenderThreads"].GetInt();

		if (threading.HasMember("assignToSpecificCores"))
			m_Graphics.m_Threading.m_AssignToSpecificCores = threading["assignToSpecificCores"].GetBool();
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
		if (sound.HasMember("hqSound"))
			m_Sound.m_HqSound = sound["hqSound"].GetBool();

		if (sound.HasMember("oggMusic"))
			m_Sound.m_OggMusic = sound["oggMusic"].GetBool();

		if (sound.HasMember("oggFolder"))
			m_Sound.m_OggFolder = sound["oggFolder"].GetString();

		if (sound.HasMember("oggMusicAlternative"))
			m_Sound.m_OggMusicAlternative = sound["oggMusicAlternative"].GetBool();

		if (sound.HasMember("fixSpeedSound"))
			m_Sound.m_FixSpeedSound = sound["fixSpeedSound"].GetBool();
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
		if (paths.HasMember("gameFolder"))
			m_Paths.m_GameFolder = paths["gameFolder"].GetString();

		if (paths.HasMember("cdFolder"))
			m_Paths.m_CdFolder = paths["cdFolder"].GetString();
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
