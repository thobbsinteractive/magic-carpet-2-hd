#pragma once
#include "SDL2/SDL.h"

struct InputMapping
{
	int Up = SDL_SCANCODE_W;
	int Right = SDL_SCANCODE_D;
	int Down = SDL_SCANCODE_S;
	int Left = SDL_SCANCODE_A;
	int SpellMenu = SDL_SCANCODE_LCTRL;
	int Map = SDL_SCANCODE_TAB;
	int SpellMenuMark = SDL_SCANCODE_LSHIFT;
};