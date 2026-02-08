#pragma once
#include <map>
#include <string>
#include <SDL2/SDL_scancode.h>

class ConfigToSdlScancode
{
private:
	std::map<std::string, SDL_Scancode> m_ConfigToSdlScancodeMap;

public:
	ConfigToSdlScancode()
	{
		m_ConfigToSdlScancodeMap[""] = SDL_SCANCODE_UNKNOWN;
		m_ConfigToSdlScancodeMap["A"] = SDL_SCANCODE_A;
		m_ConfigToSdlScancodeMap["B"] = SDL_SCANCODE_B;
		m_ConfigToSdlScancodeMap["C"] = SDL_SCANCODE_C;
		m_ConfigToSdlScancodeMap["D"] = SDL_SCANCODE_D;
		m_ConfigToSdlScancodeMap["E"] = SDL_SCANCODE_E;
		m_ConfigToSdlScancodeMap["F"] = SDL_SCANCODE_F;
		m_ConfigToSdlScancodeMap["G"] = SDL_SCANCODE_G;
		m_ConfigToSdlScancodeMap["H"] = SDL_SCANCODE_H;
		m_ConfigToSdlScancodeMap["I"] = SDL_SCANCODE_I;
		m_ConfigToSdlScancodeMap["J"] = SDL_SCANCODE_J;
		m_ConfigToSdlScancodeMap["K"] = SDL_SCANCODE_K;
		m_ConfigToSdlScancodeMap["L"] = SDL_SCANCODE_L;
		m_ConfigToSdlScancodeMap["M"] = SDL_SCANCODE_M;
		m_ConfigToSdlScancodeMap["N"] = SDL_SCANCODE_N;
		m_ConfigToSdlScancodeMap["O"] = SDL_SCANCODE_O;
		m_ConfigToSdlScancodeMap["P"] = SDL_SCANCODE_P;
		m_ConfigToSdlScancodeMap["Q"] = SDL_SCANCODE_Q;
		m_ConfigToSdlScancodeMap["R"] = SDL_SCANCODE_R;
		m_ConfigToSdlScancodeMap["S"] = SDL_SCANCODE_S;
		m_ConfigToSdlScancodeMap["T"] = SDL_SCANCODE_T;
		m_ConfigToSdlScancodeMap["U"] = SDL_SCANCODE_U;
		m_ConfigToSdlScancodeMap["V"] = SDL_SCANCODE_V;
		m_ConfigToSdlScancodeMap["W"] = SDL_SCANCODE_W;
		m_ConfigToSdlScancodeMap["X"] = SDL_SCANCODE_X;
		m_ConfigToSdlScancodeMap["Y"] = SDL_SCANCODE_Y;
		m_ConfigToSdlScancodeMap["Z"] = SDL_SCANCODE_Z;
		m_ConfigToSdlScancodeMap["1"] = SDL_SCANCODE_1;
		m_ConfigToSdlScancodeMap["2"] = SDL_SCANCODE_2;
		m_ConfigToSdlScancodeMap["3"] = SDL_SCANCODE_3;
		m_ConfigToSdlScancodeMap["4"] = SDL_SCANCODE_4;
		m_ConfigToSdlScancodeMap["5"] = SDL_SCANCODE_5;
		m_ConfigToSdlScancodeMap["6"] = SDL_SCANCODE_6;
		m_ConfigToSdlScancodeMap["7"] = SDL_SCANCODE_7;
		m_ConfigToSdlScancodeMap["8"] = SDL_SCANCODE_8;
		m_ConfigToSdlScancodeMap["9"] = SDL_SCANCODE_9;
		m_ConfigToSdlScancodeMap["0"] = SDL_SCANCODE_0;

		m_ConfigToSdlScancodeMap["RETURN"] = SDL_SCANCODE_RETURN;
		m_ConfigToSdlScancodeMap["ESCAPE"] = SDL_SCANCODE_ESCAPE;
		m_ConfigToSdlScancodeMap["BACKSPACE"] = SDL_SCANCODE_BACKSPACE;
		m_ConfigToSdlScancodeMap["TAB"] = SDL_SCANCODE_TAB;
		m_ConfigToSdlScancodeMap["SPACE"] = SDL_SCANCODE_SPACE;
		m_ConfigToSdlScancodeMap["MINUS"] = SDL_SCANCODE_MINUS;
		m_ConfigToSdlScancodeMap["EQUALS"] = SDL_SCANCODE_EQUALS;
		m_ConfigToSdlScancodeMap["LEFTBRACKET"] = SDL_SCANCODE_LEFTBRACKET;
		m_ConfigToSdlScancodeMap["RIGHTBRACKET"] = SDL_SCANCODE_RIGHTBRACKET;
		m_ConfigToSdlScancodeMap["BACKSLASH"] = SDL_SCANCODE_BACKSLASH;
		m_ConfigToSdlScancodeMap["NONUSHASH"] = SDL_SCANCODE_NONUSHASH;
		m_ConfigToSdlScancodeMap["SEMICOLON"] = SDL_SCANCODE_SEMICOLON;
		m_ConfigToSdlScancodeMap["APOSTROPHE"] = SDL_SCANCODE_APOSTROPHE;
		m_ConfigToSdlScancodeMap["GRAVE"] = SDL_SCANCODE_GRAVE;
		m_ConfigToSdlScancodeMap["COMMA"] = SDL_SCANCODE_COMMA;
		m_ConfigToSdlScancodeMap["PERIOD"] = SDL_SCANCODE_PERIOD;
		m_ConfigToSdlScancodeMap["SLASH"] = SDL_SCANCODE_SLASH;
		m_ConfigToSdlScancodeMap["CAPSLOCK"] = SDL_SCANCODE_CAPSLOCK;

		m_ConfigToSdlScancodeMap["F1"] = SDL_SCANCODE_F1;
		m_ConfigToSdlScancodeMap["F2"] = SDL_SCANCODE_F2;
		m_ConfigToSdlScancodeMap["F3"] = SDL_SCANCODE_F3;
		m_ConfigToSdlScancodeMap["F4"] = SDL_SCANCODE_F4;
		m_ConfigToSdlScancodeMap["F5"] = SDL_SCANCODE_F5;
		m_ConfigToSdlScancodeMap["F6"] = SDL_SCANCODE_F6;
		m_ConfigToSdlScancodeMap["F7"] = SDL_SCANCODE_F7;
		m_ConfigToSdlScancodeMap["F8"] = SDL_SCANCODE_F8;
		m_ConfigToSdlScancodeMap["F9"] = SDL_SCANCODE_F9;
		m_ConfigToSdlScancodeMap["F10"] = SDL_SCANCODE_F10;
		m_ConfigToSdlScancodeMap["F11"] = SDL_SCANCODE_F11;
		m_ConfigToSdlScancodeMap["F12"] = SDL_SCANCODE_F12;

		m_ConfigToSdlScancodeMap["PRINTSCREEN"] = SDL_SCANCODE_PRINTSCREEN;
		m_ConfigToSdlScancodeMap["SCROLLLOCK"] = SDL_SCANCODE_SCROLLLOCK;
		m_ConfigToSdlScancodeMap["PAUSE"] = SDL_SCANCODE_PAUSE;
		m_ConfigToSdlScancodeMap["INSERT"] = SDL_SCANCODE_INSERT;
		m_ConfigToSdlScancodeMap["HOME"] = SDL_SCANCODE_HOME;
		m_ConfigToSdlScancodeMap["PAGEUP"] = SDL_SCANCODE_PAGEUP;
		m_ConfigToSdlScancodeMap["DELETE"] = SDL_SCANCODE_DELETE;
		m_ConfigToSdlScancodeMap["END"] = SDL_SCANCODE_END;
		m_ConfigToSdlScancodeMap["PAGEDOWN"] = SDL_SCANCODE_PAGEDOWN;
		m_ConfigToSdlScancodeMap["RIGHT"] = SDL_SCANCODE_RIGHT;
		m_ConfigToSdlScancodeMap["LEFT"] =SDL_SCANCODE_LEFT;
		m_ConfigToSdlScancodeMap["DOWN"] =SDL_SCANCODE_DOWN;
		m_ConfigToSdlScancodeMap["UP"] = SDL_SCANCODE_UP;

		m_ConfigToSdlScancodeMap["NUMLOCKCLEAR"] = SDL_SCANCODE_NUMLOCKCLEAR;

		m_ConfigToSdlScancodeMap["KP_DIVIDE"] = SDL_SCANCODE_KP_DIVIDE;
		m_ConfigToSdlScancodeMap["KP_MULTIPLY"] = SDL_SCANCODE_KP_MULTIPLY;
		m_ConfigToSdlScancodeMap["KP_MINUS"] = SDL_SCANCODE_KP_MINUS;
		m_ConfigToSdlScancodeMap["KP_PLUS"] = SDL_SCANCODE_KP_PLUS;
		m_ConfigToSdlScancodeMap["KP_ENTER"] = SDL_SCANCODE_KP_ENTER;
		m_ConfigToSdlScancodeMap["KP_1"] = SDL_SCANCODE_KP_1;
		m_ConfigToSdlScancodeMap["KP_2"] = SDL_SCANCODE_KP_2;
		m_ConfigToSdlScancodeMap["KP_3"] = SDL_SCANCODE_KP_3;
		m_ConfigToSdlScancodeMap["KP_4"] = SDL_SCANCODE_KP_4;
		m_ConfigToSdlScancodeMap["KP_5"] = SDL_SCANCODE_KP_5;
		m_ConfigToSdlScancodeMap["KP_6"] = SDL_SCANCODE_KP_6;
		m_ConfigToSdlScancodeMap["KP_7"] = SDL_SCANCODE_KP_7;
		m_ConfigToSdlScancodeMap["KP_8"] = SDL_SCANCODE_KP_8;
		m_ConfigToSdlScancodeMap["KP_9"] = SDL_SCANCODE_KP_9;
		m_ConfigToSdlScancodeMap["KP_0"] = SDL_SCANCODE_KP_0;
		m_ConfigToSdlScancodeMap["KP_PERIOD"] = SDL_SCANCODE_KP_PERIOD;

		m_ConfigToSdlScancodeMap["NONUSBACKSLASH"] = SDL_SCANCODE_NONUSBACKSLASH;
		m_ConfigToSdlScancodeMap["APPLICATION"] = SDL_SCANCODE_APPLICATION;
		m_ConfigToSdlScancodeMap["POWER"] = SDL_SCANCODE_POWER;
		m_ConfigToSdlScancodeMap["KP_EQUALS"] = SDL_SCANCODE_KP_EQUALS;
		m_ConfigToSdlScancodeMap["F13"] = SDL_SCANCODE_F13;
		m_ConfigToSdlScancodeMap["F14"] = SDL_SCANCODE_F14;
		m_ConfigToSdlScancodeMap["F15"] = SDL_SCANCODE_F15;
		m_ConfigToSdlScancodeMap["F16"] = SDL_SCANCODE_F16;
		m_ConfigToSdlScancodeMap["F17"] = SDL_SCANCODE_F17;
		m_ConfigToSdlScancodeMap["F18"] = SDL_SCANCODE_F18;
		m_ConfigToSdlScancodeMap["F19"] = SDL_SCANCODE_F19;
		m_ConfigToSdlScancodeMap["F20"] = SDL_SCANCODE_F20;
		m_ConfigToSdlScancodeMap["F21"] = SDL_SCANCODE_F21;
		m_ConfigToSdlScancodeMap["F22"] = SDL_SCANCODE_F22;
		m_ConfigToSdlScancodeMap["F23"] = SDL_SCANCODE_F23;
		m_ConfigToSdlScancodeMap["F24"] = SDL_SCANCODE_F24;
		m_ConfigToSdlScancodeMap["EXECUTE"] = SDL_SCANCODE_EXECUTE;
		m_ConfigToSdlScancodeMap["HELP"] = SDL_SCANCODE_HELP;
		m_ConfigToSdlScancodeMap["MENU"] = SDL_SCANCODE_MENU;
		m_ConfigToSdlScancodeMap["SELECT"] = SDL_SCANCODE_SELECT;
		m_ConfigToSdlScancodeMap["STOP"] = SDL_SCANCODE_STOP;
		m_ConfigToSdlScancodeMap["AGAIN"] = SDL_SCANCODE_AGAIN;
		m_ConfigToSdlScancodeMap["UNDO"] = SDL_SCANCODE_UNDO;
		m_ConfigToSdlScancodeMap["CUT"] = SDL_SCANCODE_CUT;
		m_ConfigToSdlScancodeMap["COPY"] = SDL_SCANCODE_COPY;
		m_ConfigToSdlScancodeMap["PASTE"] = SDL_SCANCODE_PASTE;
		m_ConfigToSdlScancodeMap["FIND"] = SDL_SCANCODE_FIND;
		m_ConfigToSdlScancodeMap["MUTE"] = SDL_SCANCODE_MUTE;
		m_ConfigToSdlScancodeMap["VOLUMEUP"] = SDL_SCANCODE_VOLUMEUP;
		m_ConfigToSdlScancodeMap["VOLUMEDOWN"] = SDL_SCANCODE_VOLUMEDOWN;
		m_ConfigToSdlScancodeMap["KP_COMMA"] = SDL_SCANCODE_KP_COMMA;
		m_ConfigToSdlScancodeMap["KP_EQUALSAS400"] = SDL_SCANCODE_KP_EQUALSAS400;

		m_ConfigToSdlScancodeMap["INTERNATIONAL1"] = SDL_SCANCODE_INTERNATIONAL1;
		m_ConfigToSdlScancodeMap["INTERNATIONAL2"] = SDL_SCANCODE_INTERNATIONAL2;
		m_ConfigToSdlScancodeMap["INTERNATIONAL3"] = SDL_SCANCODE_INTERNATIONAL3;
		m_ConfigToSdlScancodeMap["INTERNATIONAL4"] = SDL_SCANCODE_INTERNATIONAL4;
		m_ConfigToSdlScancodeMap["INTERNATIONAL5"] = SDL_SCANCODE_INTERNATIONAL5;
		m_ConfigToSdlScancodeMap["INTERNATIONAL6"] = SDL_SCANCODE_INTERNATIONAL6;
		m_ConfigToSdlScancodeMap["INTERNATIONAL7"] = SDL_SCANCODE_INTERNATIONAL7;
		m_ConfigToSdlScancodeMap["INTERNATIONAL8"] = SDL_SCANCODE_INTERNATIONAL8;
		m_ConfigToSdlScancodeMap["INTERNATIONAL9"] = SDL_SCANCODE_INTERNATIONAL9;
		m_ConfigToSdlScancodeMap["LANG1"] = SDL_SCANCODE_LANG1;
		m_ConfigToSdlScancodeMap["LANG2"] = SDL_SCANCODE_LANG2;
		m_ConfigToSdlScancodeMap["LANG3"] = SDL_SCANCODE_LANG3;
		m_ConfigToSdlScancodeMap["LANG4"] = SDL_SCANCODE_LANG4;
		m_ConfigToSdlScancodeMap["LANG5"] = SDL_SCANCODE_LANG5;
		m_ConfigToSdlScancodeMap["LANG6"] = SDL_SCANCODE_LANG6;
		m_ConfigToSdlScancodeMap["LANG7"] = SDL_SCANCODE_LANG7;
		m_ConfigToSdlScancodeMap["LANG8"] = SDL_SCANCODE_LANG8;
		m_ConfigToSdlScancodeMap["LANG9"] = SDL_SCANCODE_LANG9;

		m_ConfigToSdlScancodeMap["ALTERASE"] = SDL_SCANCODE_ALTERASE;
		m_ConfigToSdlScancodeMap["SYSREQ"] = SDL_SCANCODE_SYSREQ;
		m_ConfigToSdlScancodeMap["CANCEL"] = SDL_SCANCODE_CANCEL;
		m_ConfigToSdlScancodeMap["CLEAR"] = SDL_SCANCODE_CLEAR;
		m_ConfigToSdlScancodeMap["PRIOR"] = SDL_SCANCODE_PRIOR;
		m_ConfigToSdlScancodeMap["RETURN2"] = SDL_SCANCODE_RETURN2;
		m_ConfigToSdlScancodeMap["SEPARATOR"] = SDL_SCANCODE_SEPARATOR;
		m_ConfigToSdlScancodeMap["OUT"] = SDL_SCANCODE_OUT;
		m_ConfigToSdlScancodeMap["OPER"] = SDL_SCANCODE_OPER;
		m_ConfigToSdlScancodeMap["CLEARAGAIN"] = SDL_SCANCODE_CLEARAGAIN;
		m_ConfigToSdlScancodeMap["CRSEL"] = SDL_SCANCODE_CRSEL;
		m_ConfigToSdlScancodeMap["EXSEL"] = SDL_SCANCODE_EXSEL;

		m_ConfigToSdlScancodeMap["KP_00"] = SDL_SCANCODE_KP_00;
		m_ConfigToSdlScancodeMap["KP_000"] = SDL_SCANCODE_KP_000;
		m_ConfigToSdlScancodeMap["THOUSANDSSEPARATOR"] = SDL_SCANCODE_THOUSANDSSEPARATOR;
		m_ConfigToSdlScancodeMap["DECIMALSEPARATOR"] = SDL_SCANCODE_DECIMALSEPARATOR;
		m_ConfigToSdlScancodeMap["CURRENCYUNIT"] = SDL_SCANCODE_CURRENCYUNIT;
		m_ConfigToSdlScancodeMap["CURRENCYSUBUNIT"] = SDL_SCANCODE_CURRENCYSUBUNIT;
		m_ConfigToSdlScancodeMap["KP_LEFTPAREN"] = SDL_SCANCODE_KP_LEFTPAREN;
		m_ConfigToSdlScancodeMap["KP_RIGHTPAREN"] = SDL_SCANCODE_KP_RIGHTPAREN;
		m_ConfigToSdlScancodeMap["KP_LEFTBRACE"] = SDL_SCANCODE_KP_LEFTBRACE;
		m_ConfigToSdlScancodeMap["KP_RIGHTBRACE"] = SDL_SCANCODE_KP_RIGHTBRACE;
		m_ConfigToSdlScancodeMap["KP_TAB"] = SDL_SCANCODE_KP_TAB;
		m_ConfigToSdlScancodeMap["KP_BACKSPACE"] = SDL_SCANCODE_KP_BACKSPACE;
		m_ConfigToSdlScancodeMap["KP_A"] = SDL_SCANCODE_KP_A;
		m_ConfigToSdlScancodeMap["KP_B"] = SDL_SCANCODE_KP_B;
		m_ConfigToSdlScancodeMap["KP_C"] = SDL_SCANCODE_KP_C;
		m_ConfigToSdlScancodeMap["KP_D"] = SDL_SCANCODE_KP_D;
		m_ConfigToSdlScancodeMap["KP_E"] = SDL_SCANCODE_KP_E;
		m_ConfigToSdlScancodeMap["KP_F"] = SDL_SCANCODE_KP_F;
		m_ConfigToSdlScancodeMap["KP_XOR"] = SDL_SCANCODE_KP_XOR;
		m_ConfigToSdlScancodeMap["KP_POWER"] = SDL_SCANCODE_KP_POWER;
		m_ConfigToSdlScancodeMap["KP_PERCENT"] = SDL_SCANCODE_KP_PERCENT;
		m_ConfigToSdlScancodeMap["KP_LESS"] = SDL_SCANCODE_KP_LESS;
		m_ConfigToSdlScancodeMap["KP_GREATER"] = SDL_SCANCODE_KP_GREATER;
		m_ConfigToSdlScancodeMap["KP_AMPERSAND"] = SDL_SCANCODE_KP_AMPERSAND;
		m_ConfigToSdlScancodeMap["KP_DBLAMPERSAND"] = SDL_SCANCODE_KP_DBLAMPERSAND;
		m_ConfigToSdlScancodeMap["KP_VERTICALBAR"] = SDL_SCANCODE_KP_VERTICALBAR;
		m_ConfigToSdlScancodeMap["KP_DBLVERTICALBAR"] = SDL_SCANCODE_KP_DBLVERTICALBAR;
		m_ConfigToSdlScancodeMap["KP_COLON"] = SDL_SCANCODE_KP_COLON;
		m_ConfigToSdlScancodeMap["KP_HASH"] = SDL_SCANCODE_KP_HASH;
		m_ConfigToSdlScancodeMap["KP_SPACE"] = SDL_SCANCODE_KP_SPACE;
		m_ConfigToSdlScancodeMap["KP_AT"] = SDL_SCANCODE_KP_AT;
		m_ConfigToSdlScancodeMap["KP_EXCLAM"] = SDL_SCANCODE_KP_EXCLAM;
		m_ConfigToSdlScancodeMap["KP_MEMSTORE"] = SDL_SCANCODE_KP_MEMSTORE;
		m_ConfigToSdlScancodeMap["KP_MEMRECALL"] = SDL_SCANCODE_KP_MEMRECALL;
		m_ConfigToSdlScancodeMap["KP_MEMCLEAR"] = SDL_SCANCODE_KP_MEMCLEAR;
		m_ConfigToSdlScancodeMap["KP_MEMADD"] = SDL_SCANCODE_KP_MEMADD;
		m_ConfigToSdlScancodeMap["KP_MEMSUBTRACT"] = SDL_SCANCODE_KP_MEMSUBTRACT;
		m_ConfigToSdlScancodeMap["KP_MEMMULTIPLY"] = SDL_SCANCODE_KP_MEMMULTIPLY;
		m_ConfigToSdlScancodeMap["KP_MEMDIVIDE"] = SDL_SCANCODE_KP_MEMDIVIDE;
		m_ConfigToSdlScancodeMap["KP_PLUSMINUS"] = SDL_SCANCODE_KP_PLUSMINUS;
		m_ConfigToSdlScancodeMap["KP_CLEAR"] = SDL_SCANCODE_KP_CLEAR;
		m_ConfigToSdlScancodeMap["KP_CLEARENTRY"] = SDL_SCANCODE_KP_CLEARENTRY;
		m_ConfigToSdlScancodeMap["KP_BINARY"] = SDL_SCANCODE_KP_BINARY;
		m_ConfigToSdlScancodeMap["KP_OCTAL"] = SDL_SCANCODE_KP_OCTAL;
		m_ConfigToSdlScancodeMap["KP_DECIMAL"] = SDL_SCANCODE_KP_DECIMAL;
		m_ConfigToSdlScancodeMap["KP_HEXADECIMAL"] = SDL_SCANCODE_KP_HEXADECIMAL;

		m_ConfigToSdlScancodeMap["LCTRL"] = SDL_SCANCODE_LCTRL;
		m_ConfigToSdlScancodeMap["LSHIFT"] = SDL_SCANCODE_LSHIFT;
		m_ConfigToSdlScancodeMap["LALT"] = SDL_SCANCODE_LALT;
		m_ConfigToSdlScancodeMap["LGUI"] = SDL_SCANCODE_LGUI;
		m_ConfigToSdlScancodeMap["RCTRL"] = SDL_SCANCODE_RCTRL;
		m_ConfigToSdlScancodeMap["RSHIFT"] = SDL_SCANCODE_RSHIFT;
		m_ConfigToSdlScancodeMap["RALT"] = SDL_SCANCODE_RALT;
		m_ConfigToSdlScancodeMap["RGUI"] = SDL_SCANCODE_RGUI;
	}

	SDL_Scancode GetScancode(std::string text)
	{
		if (m_ConfigToSdlScancodeMap.count(text) > 0)
			return m_ConfigToSdlScancodeMap[text];
		return SDL_SCANCODE_UNKNOWN;
	}
};