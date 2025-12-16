#pragma once
#ifndef MAIN_BASIC_MINI
#define MAIN_BASIC_MINI

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

extern uint8_t* Zero_pointer;

void qmemcpy(void* a, void* b, size_t c);

#endif //MAIN_BASIC_MINI