#pragma once
#ifndef MAIN_LEVEL
#define MAIN_LEVEL
#include <stdio.h>
#include <cstdint>
#include <assert.h>
#include <array>
#include <sstream>
#include <stddef.h>
#include <stdarg.h>

//#include "png.h"
//#pragma comment(lib, "zlib.lib") // must be before libpng!
//#ifndef _WIN64
//#pragma comment(lib, "libpng15.lib") // must be after zlib!
//#else
//#endif

//#include "../portability/port_time.h"
//#include "../portability/port_sdl_vga_mouse.h"
//#include "../portability/port_outputs.h"
//#include "../portability/port_show_perifery.h"

#include "Basic_terrain.h"
#pragma pack (1)
typedef struct {
	uint8_t m_wReflections;
	uint8_t m_wShadows;
	uint8_t m_wSky;
	uint8_t m_wViewPortSize;
}
GraphicsStruct_t;
typedef struct {
	uint8_t m_wMiniMap;//8590 icons//str_0x218E.str.icons_0x218E
	uint8_t m_wTopBar;//8591 ??//str_0x218E.str.xxxx_0x218F
	uint8_t m_uiScreenSize;//8592 screen size?//str_0x218E.str.scr_size_0x2190
	uint8_t xxxx_0x2191;//8593//str_0x218E.str.xxxx_0x2191
}
DisplayStruct_t;
typedef struct {
	uint8_t xxxx_0x2192;//8594 ??//str_0x2192.str.xxxx_0x2192
	uint8_t xxxx_0x2193;//8595 ??//str_0x2192.str.xxxx_0x2193
	uint8_t m_wResolution;//8596 resolution//str_0x2192.str.resolution_0x2194
	uint8_t shadows_0x2195;//8593 shadows//str_0x2192.str.shadows_0x2195
}
type_str_0x2192;
typedef struct {
	uint8_t m_wDynamicLighting;//8598 lights//str_0x2196.str.lights_0x2196
	uint8_t setting_0x2197;//8599 setting//str_0x2196.str.setting_0x2197
	uint8_t transparency_0x2198;//8600 transparency//str_0x2196.str.transparency_0x2198
	uint8_t flat_0x2199;//8601 flat//str_0x2196.str.flat_0x2199
}
type_str_0x2196;
typedef struct {
	GraphicsStruct_t m_Graphics;//0x218A
	DisplayStruct_t m_Display;//0x218E
	type_str_0x2192 str_0x2192;
	type_str_0x2196 str_0x2196;
}
GameSettingsStruct_t;
#pragma pack (16)
#endif //MAIN_LEVEL

