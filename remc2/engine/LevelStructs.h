#pragma once
#ifndef MAIN_LEVEL_STRUCTS
#define MAIN_LEVEL_STRUCTS
#if (!defined(WIN32) && !defined(__linux__)) || defined(COMPILE_FOR_64BIT)
#define TEST_x64//only for x64 testing
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cstdint>
#include <functional>
#include <chrono>

#include "stdint.h"
#ifdef _MSC_VER
#include <windows.h>
#endif
#include "defs.h"
#include "global_types.h"

#include "BasicTerrain.h"
#pragma pack (1)

#pragma pack (16)

#endif //MAIN_LEVEL_STRUCTS

