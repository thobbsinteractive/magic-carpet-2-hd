#pragma once
#ifndef ENGINE_EDITOR_H
#define ENGINE_EDITOR_H

#include "kiss_sdl.h"

#define IS_EDITOR
#include "../engine/Entity.h"
#include "../sub_main.h"
#include "../engine/DatTabIndexes.h"
#include "../engine/LevelInit.h"
//#include "../engine/engine_support.h"
//#include "../Terrain.h"

void editor_run();

#endif