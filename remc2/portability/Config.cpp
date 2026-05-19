#include <filesystem>
#include <iostream>
#include <fstream>
#include "Config.h"

using json = nlohmann::json;

Config::Config(std::string fileName)
{
	m_FileName = fileName;
	LoadFromFile(fileName);
}

bool Config::LoadFromFile()
{
	return LoadFromFile(m_FileName);
}

bool Config::LoadFromFile(std::string fileName)
{
	if (std::filesystem::exists(fileName))
	{
		auto jsonStr = ReadFileToString(fileName);

		if (jsonStr.size() > 0)
		{
			try
			{
				json document = json::parse(jsonStr);
				if (document.contains("settings"))
				{
					LoadSettings(document);
					return true;
				}
			}
			catch (const json::parse_error& e)
			{
				std::cout << "JSON parse error: " << e.what();
			}
		}
	}
	return false;
}

std::string Config::ReadStringValue(const json& settings, const char* name)
{
	if (settings.contains(name) && settings[name].is_string())
	{
		return settings[name].get<std::string>();
	}
	return "";
}

int Config::ReadIntValue(const json& settings, const char* name)
{
	if (settings.contains(name) && settings[name].is_number())
	{
		return settings[name].get<int>();
	}
	return 0;
}

float Config::ReadFloatValue(const json& settings, const char* name)
{
	if (settings.contains(name) && settings[name].is_number())
	{
		return settings[name].get<float>();
	}
	return 0.0f;
}

bool Config::ReadBoolValue(const json& settings, const char* name)
{
	if (settings.contains(name) && settings[name].is_boolean())
	{
		return settings[name].get<bool>();
	}
	return false;
}

SDL_Scancode Config::ReadKeyScancode(const json& settings, const char* name)
{
	if (settings.contains(name) && settings[name].is_string())
	{
		return m_ConfigToSdlScancode.GetScancode(settings[name].get<std::string>());
	}
	return SDL_Scancode::SDL_SCANCODE_UNKNOWN;
}

void Config::SetString(json& obj, const char* key, const std::string& value)
{
	obj[key] = value;
}

void Config::SetInt(json& obj, const char* key, int value)
{
	obj[key] = value;
}

void Config::SetFloat(json& obj, const char* key, float value)
{
	obj[key] = value;
}

void Config::SetBool(json& obj, const char* key, bool value)
{
	obj[key] = value;
}

json& Config::GetOrCreate(json& parent, const char* key)
{
	if (!parent.contains(key))
	{
		parent[key] = json::object();
	}
	return parent[key];
}

void Config::LoadSettings(json& document)
{
	auto& settingsArray = document["settings"];

	for (auto& entry : settingsArray)
	{
		if (entry.contains("name") && entry.contains("isActive") && entry["isActive"].get<bool>() == true)
		{
			m_Name = ReadStringValue(entry, "name");
			m_Version = ReadStringValue(entry, "version");
			LoadPaths(entry);
			LoadSound(entry);
			LoadGraphics(entry);
			LoadGame(entry);
			LoadControls(entry);
			break;
		}
	}
}

void Config::LoadGame(const json& settings)
{
	if (settings.contains("game"))
	{
		const auto& game = settings["game"];
		m_Game.m_MaxGameFps = ReadIntValue(game, "maxGameFps");
		m_Game.m_FmvFps = ReadIntValue(game, "fmvFps");
	}
}

void Config::LoadControls(const json& settings)
{
	if (!settings.contains("controls")) return;

	const auto& controls = settings["controls"];

	if (controls.contains("mouse"))
	{
		for (const auto& mouse : controls["mouse"])
		{
			if (mouse.contains("isActive") && mouse["isActive"].get<bool>() == true)
			{
				m_Controls.m_Mouse.m_InvertXAxis = ReadBoolValue(mouse, "invertXAxis");
				m_Controls.m_Mouse.m_InvertYAxis = ReadBoolValue(mouse, "invertYAxis");
				m_Controls.m_Mouse.m_mouseScaleX = ReadFloatValue(mouse, "mouseScaleX");
				m_Controls.m_Mouse.m_mouseScaleY = ReadFloatValue(mouse, "mouseScaleY");
				m_Controls.m_Mouse.m_disableLRButtonsMenuOpen = ReadBoolValue(mouse, "disableLRButtonsMenuOpen");
				m_Controls.m_Mouse.m_spellLeft = ReadIntValue(mouse, "spellLeft");
				m_Controls.m_Mouse.m_spellRight = ReadIntValue(mouse, "spellRight");
				m_Controls.m_Mouse.m_map = ReadIntValue(mouse, "map");
				m_Controls.m_Mouse.m_spellMenu = ReadIntValue(mouse, "spellMenu");
				m_Controls.m_Mouse.m_spellMenuMark = ReadIntValue(mouse, "spellMenuMark");
				break;
			}
		}
	}

	if (controls.contains("keyboard"))
	{
		for (const auto& keyboard : controls["keyboard"])
		{
			if (keyboard.contains("isActive") && keyboard["isActive"].get<bool>() == true)
			{
				m_Controls.m_Keyboard.m_forward = ReadKeyScancode(keyboard, "forward");
				m_Controls.m_Keyboard.m_backwards = ReadKeyScancode(keyboard, "backwards");
				m_Controls.m_Keyboard.m_left = ReadKeyScancode(keyboard, "left");
				m_Controls.m_Keyboard.m_right = ReadKeyScancode(keyboard, "right");
				m_Controls.m_Keyboard.m_map = ReadKeyScancode(keyboard, "map");
				m_Controls.m_Keyboard.m_spellMenu = ReadKeyScancode(keyboard, "spellMenu");
				m_Controls.m_Keyboard.m_spellMenuMark = ReadKeyScancode(keyboard, "spellMenuMark");
				break;
			}
		}
	}

	if (controls.contains("gamePad"))
	{
		for (const auto& gamePad : controls["gamePad"])
		{
			if (gamePad.contains("isActive") && gamePad["isActive"].get<bool>() == true)
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

				if (gamePad.contains("axisYawSensitivity"))
				{
					const auto& axisYawSensitivity = gamePad["axisYawSensitivity"];
					if (axisYawSensitivity.contains("zones"))
					{
						for (const auto& zone : axisYawSensitivity["zones"])
						{
							if (zone.contains("start") && zone.contains("end") && zone.contains("factor"))
							{
								m_Controls.m_GamePad.m_AxisYawSensitivity.push_back(Maths::Zone{
									(uint16_t)zone["start"].get<int>(),
									(uint16_t)zone["end"].get<int>(),
									zone["factor"].get<double>()
									});
							}
						}
					}
				}

				m_Controls.m_GamePad.m_AxisPitch = (uint16_t)ReadIntValue(gamePad, "axisPitch");
				m_Controls.m_GamePad.m_AxisPitchInv = ReadBoolValue(gamePad, "axisPitchInv");
				m_Controls.m_GamePad.m_AxisPitchDeadZone = (uint16_t)ReadIntValue(gamePad, "axisPitchDeadZone");

				if (gamePad.contains("axisPitchSensitivity"))
				{
					const auto& axisPitchSensitivity = gamePad["axisPitchSensitivity"];
					if (axisPitchSensitivity.contains("zones"))
					{
						for (const auto& zone : axisPitchSensitivity["zones"])
						{
							if (zone.contains("start") && zone.contains("end") && zone.contains("factor"))
							{
								m_Controls.m_GamePad.m_AxisPitchSensitivity.push_back(Maths::Zone{
									(uint16_t)zone["start"].get<int>(),
									(uint16_t)zone["end"].get<int>(),
									zone["factor"].get<double>()
									});
							}
						}
					}
				}
				break;
			}
		}
	}
}

void Config::LoadGraphics(const json& settings)
{
	if (settings.contains("graphics"))
	{
		const auto& graphics = settings["graphics"];
		m_Graphics.m_DisplayIndex = ReadIntValue(graphics, "displayIndex");
		m_Graphics.m_WindowResWidth = ReadIntValue(graphics, "windowResWidth");
		m_Graphics.m_WindowResHeight = ReadIntValue(graphics, "windowResHeight");
		m_Graphics.m_MaintainAspectRatio = ReadBoolValue(graphics, "maintainAspectRatio");
		m_Graphics.m_StartWindowed = ReadBoolValue(graphics, "startWindowed");

		LoadGameDetail(graphics);
		LoadThreading(graphics);
	}
}

void Config::LoadGameDetail(const json& graphics)
{
	if (graphics.contains("gameDetail"))
	{
		const auto& gameDetail = graphics["gameDetail"];
		m_Graphics.m_GameDetail.m_GameResWidth = ReadIntValue(gameDetail, "gameResWidth");
		m_Graphics.m_GameDetail.m_GameResHeight = ReadIntValue(gameDetail, "gameResHeight");
		m_Graphics.m_GameDetail.m_GameUiScale = ReadIntValue(gameDetail, "gameUiScale");
		m_Graphics.m_GameDetail.m_UseHighResGraphics = ReadBoolValue(gameDetail, "useHighResGraphics");
		m_Graphics.m_GameDetail.m_HighResGraphicsFolder = ReadStringValue(gameDetail, "highResGraphicsFolder");
		m_Graphics.m_GameDetail.m_UseFixedMenuGraphics = ReadBoolValue(gameDetail, "useFixedMenuGraphics");
		m_Graphics.m_GameDetail.m_FixedMenuGraphicsFolder = ReadStringValue(gameDetail, "fixedMenuGraphicsFolder");
		m_Graphics.m_GameDetail.m_UseExtendedFonts = ReadBoolValue(gameDetail, "useExtendedFonts");
		m_Graphics.m_GameDetail.m_ExtendedFontsFolder = ReadStringValue(gameDetail, "extendedFontsFolder");
		m_Graphics.m_GameDetail.m_Sky = ReadBoolValue(gameDetail, "sky");
		m_Graphics.m_GameDetail.m_Reflections = ReadBoolValue(gameDetail, "reflections");
		m_Graphics.m_GameDetail.m_DynamicLighting = ReadBoolValue(gameDetail, "dynamicLighting");
		m_Graphics.m_GameDetail.m_ViewDistanceScale = ReadIntValue(gameDetail, "viewDistanceScale");
	}
}

void Config::LoadThreading(const json& graphics)
{
	if (graphics.contains("threading"))
	{
		const auto& threading = graphics["threading"];
		m_Graphics.m_Threading.m_isActive = threading.value("isActive", false);
		if (m_Graphics.m_Threading.m_isActive)
		{
			m_Graphics.m_Threading.m_SizePercentToThreadRender = ReadFloatValue(threading, "sizePercentToThreadRender");
			m_Graphics.m_Threading.m_NumberOfRenderThreads = (uint8_t)ReadIntValue(threading, "numberOfRenderThreads");
			m_Graphics.m_Threading.m_AssignToSpecificCores = ReadBoolValue(threading, "assignToSpecificCores");
		}
	}
}

void Config::LoadSound(const json& settings)
{
	if (settings.contains("sound"))
	{
		const auto& sound = settings["sound"];
		m_Sound.m_HqSound = ReadBoolValue(sound, "hqSound");
		m_Sound.m_OggMusic = ReadBoolValue(sound, "oggMusic");
		m_Sound.m_OggFolder = ReadStringValue(sound, "oggFolder");
		m_Sound.m_OggMusicAlternative = ReadBoolValue(sound, "oggMusicAlternative");
		m_Sound.m_FixSpeedSound = ReadBoolValue(sound, "fixSpeedSound");
		m_Sound.m_AutoShowObjectivesForForeignLanguages = ReadBoolValue(sound, "autoShowObjectivesForForeignLanguages");
		m_Sound.m_MaxSimultaniousSounds = ReadIntValue(sound, "maxSimultaniousSounds");
		m_Sound.m_SpeechFolder = ReadStringValue(sound, "speechFolder");
	}
}

void Config::LoadPaths(const json& settings)
{
	if (settings.contains("paths"))
	{
		const auto& paths = settings["paths"];
		m_Paths.m_GameFolder = ReadStringValue(paths, "gameFolder");
		m_Paths.m_CdFolder = ReadStringValue(paths, "cdFolder");
	}
}

std::string Config::ReadFileToString(std::string fileName)
{
	std::string jsonStr;

	try
	{
		std::ifstream is(fileName, std::ifstream::binary);
		std::string line;
		while (std::getline(is, line))
		{
			jsonStr += line;
			jsonStr.push_back('\n');
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error Stopping Worker Thread: " << e.what();
	}
	return jsonStr;
}

void Config::SavePaths(json& settings)
{
	auto& paths = GetOrCreate(settings, "paths");
	SetString(paths, "gameFolder", m_Paths.m_GameFolder);
	SetString(paths, "cdFolder", m_Paths.m_CdFolder);
}

void Config::SaveSound(json& settings)
{
	auto& sound = GetOrCreate(settings, "sound");
	SetBool(sound, "hqSound", m_Sound.m_HqSound);
	SetBool(sound, "oggMusic", m_Sound.m_OggMusic);
	SetString(sound, "oggFolder", m_Sound.m_OggFolder);
	SetBool(sound, "oggMusicAlternative", m_Sound.m_OggMusicAlternative);
	SetBool(sound, "fixSpeedSound", m_Sound.m_FixSpeedSound);
	SetBool(sound, "autoShowObjectivesForForeignLanguages", m_Sound.m_AutoShowObjectivesForForeignLanguages);
	SetInt(sound, "maxSimultaniousSounds", m_Sound.m_MaxSimultaniousSounds);
	SetString(sound, "speechFolder", m_Sound.m_SpeechFolder);
}

void Config::SaveGameDetail(json& graphics)
{
	auto& gd = GetOrCreate(graphics, "gameDetail");
	SetInt(gd, "gameResWidth", m_Graphics.m_GameDetail.m_GameResWidth);
	SetInt(gd, "gameResHeight", m_Graphics.m_GameDetail.m_GameResHeight);
	SetInt(gd, "gameUiScale", m_Graphics.m_GameDetail.m_GameUiScale);
	SetBool(gd, "useHighResGraphics", m_Graphics.m_GameDetail.m_UseHighResGraphics);
	SetString(gd, "highResGraphicsFolder", m_Graphics.m_GameDetail.m_HighResGraphicsFolder);
	SetBool(gd, "useFixedMenuGraphics", m_Graphics.m_GameDetail.m_UseFixedMenuGraphics);
	SetString(gd, "fixedMenuGraphicsFolder", m_Graphics.m_GameDetail.m_FixedMenuGraphicsFolder);
	SetBool(gd, "useExtendedFonts", m_Graphics.m_GameDetail.m_UseExtendedFonts);
	SetString(gd, "extendedFontsFolder", m_Graphics.m_GameDetail.m_ExtendedFontsFolder);
	SetBool(gd, "sky", m_Graphics.m_GameDetail.m_Sky);
	SetBool(gd, "reflections", m_Graphics.m_GameDetail.m_Reflections);
	SetBool(gd, "dynamicLighting", m_Graphics.m_GameDetail.m_DynamicLighting);
	SetInt(gd, "viewDistanceScale", m_Graphics.m_GameDetail.m_ViewDistanceScale);
}

void Config::SaveThreading(json& graphics)
{
	auto& t = GetOrCreate(graphics, "threading");
	SetBool(t, "isActive", m_Graphics.m_Threading.m_isActive);
	SetFloat(t, "sizePercentToThreadRender", m_Graphics.m_Threading.m_SizePercentToThreadRender);
	SetInt(t, "numberOfRenderThreads", m_Graphics.m_Threading.m_NumberOfRenderThreads);
	SetBool(t, "assignToSpecificCores", m_Graphics.m_Threading.m_AssignToSpecificCores);
}

void Config::SaveGraphics(json& settings)
{
	auto& gfx = GetOrCreate(settings, "graphics");
	SetInt(gfx, "displayIndex", m_Graphics.m_DisplayIndex);
	SetInt(gfx, "windowResWidth", m_Graphics.m_WindowResWidth);
	SetInt(gfx, "windowResHeight", m_Graphics.m_WindowResHeight);
	SetBool(gfx, "maintainAspectRatio", m_Graphics.m_MaintainAspectRatio);
	SetBool(gfx, "startWindowed", m_Graphics.m_StartWindowed);
	SetString(gfx, "forceRender", m_Graphics.m_ForceRender);
	SaveGameDetail(gfx);
	SaveThreading(gfx);
}

void Config::SaveGame(json& settings)
{
	auto& game = GetOrCreate(settings, "game");
	SetInt(game, "maxGameFps", m_Game.m_MaxGameFps);
	SetInt(game, "fmvFps", m_Game.m_FmvFps);
	SetBool(game, "skipIntro", m_Game.m_SkipIntro);
}

void Config::SaveControls(json& settings)
{
	auto& controls = GetOrCreate(settings, "controls");

	if (!controls.contains("mouse"))
		controls["mouse"] = json::array({ json::object() });
	auto& mouse = controls["mouse"][0];
	SetBool(mouse, "isActive", true);
	SetBool(mouse, "invertXAxis", m_Controls.m_Mouse.m_InvertXAxis);
	SetBool(mouse, "invertYAxis", m_Controls.m_Mouse.m_InvertYAxis);
	SetFloat(mouse, "mouseScaleX", m_Controls.m_Mouse.m_mouseScaleX);
	SetFloat(mouse, "mouseScaleY", m_Controls.m_Mouse.m_mouseScaleY);
	SetBool(mouse, "disableLRButtonsMenuOpen", m_Controls.m_Mouse.m_disableLRButtonsMenuOpen);
	SetInt(mouse, "spellLeft", m_Controls.m_Mouse.m_spellLeft);
	SetInt(mouse, "spellRight", m_Controls.m_Mouse.m_spellRight);
	SetInt(mouse, "map", m_Controls.m_Mouse.m_map);
	SetInt(mouse, "spellMenu", m_Controls.m_Mouse.m_spellMenu);
	SetInt(mouse, "spellMenuMark", m_Controls.m_Mouse.m_spellMenuMark);

	if (!controls.contains("keyboard"))
		controls["keyboard"] = json::array({ json::object() });
	auto& keyboard = controls["keyboard"][0];
	SetBool(keyboard, "isActive", true);
	SetString(keyboard, "forward", m_ConfigToSdlScancode.GetName(m_Controls.m_Keyboard.m_forward));
	SetString(keyboard, "backwards", m_ConfigToSdlScancode.GetName(m_Controls.m_Keyboard.m_backwards));
	SetString(keyboard, "left", m_ConfigToSdlScancode.GetName(m_Controls.m_Keyboard.m_left));
	SetString(keyboard, "right", m_ConfigToSdlScancode.GetName(m_Controls.m_Keyboard.m_right));
	SetString(keyboard, "map", m_ConfigToSdlScancode.GetName(m_Controls.m_Keyboard.m_map));
	SetString(keyboard, "spellMenu", m_ConfigToSdlScancode.GetName(m_Controls.m_Keyboard.m_spellMenu));
	SetString(keyboard, "spellMenuMark", m_ConfigToSdlScancode.GetName(m_Controls.m_Keyboard.m_spellMenuMark));
}

void Config::SaveSettings(json& document)
{
	if (!document.contains("settings")) return;

	for (auto& entry : document["settings"])
	{
		if (entry.contains("isActive") && entry["isActive"].get<bool>())
		{
			SavePaths(entry);
			SaveSound(entry);
			SaveGraphics(entry);
			SaveGame(entry);
			SaveControls(entry);
			break;
		}
	}
}

bool Config::SaveToFile()
{
	return SaveToFile(m_FileName);
}

bool Config::SaveToFile(std::string fileName)
{
	json document;
	{
		auto jsonStr = ReadFileToString(fileName);
		if (!jsonStr.empty())
		{
			try
			{
				document = json::parse(jsonStr);
				if (!document.is_object())
					document = json::object();
			}
			catch (const json::parse_error&)
			{
				document = json::object();
			}
		}
		else
		{
			document = json::object();
		}
	}

	SaveSettings(document);

	std::ofstream f(fileName);
	if (!f.is_open()) return false;

	f << document.dump(1, '\t');
	return true;
}
