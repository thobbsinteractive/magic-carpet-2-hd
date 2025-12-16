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

typedef struct {
	uint8_t creflections_0x21AA;//uint8_t byte_0x21AA;//8618 can reflections//str_0x21AA.str.creflections_0x21AA
	uint8_t cshadows_0x21AB;//uint8_t byte_0x21AB;//8619 can shadows//str_0x21AA.str.cshadows_0x21AB
	uint8_t csky_0x21AC;//uint8_t byte_0x21AC;//8620 can sky//str_0x21AA.str.csky_0x21AC
	uint8_t calter_0x21AD;//uint8_t byte_0x21AD;//8621//str_0x21AA.str.calter_0x21AD
}
type_str_0x21AA;
typedef struct {
	uint8_t xxxx_0x21AE;//uint8_t byte_0x21AE;//str_0x21AE.str.xxxx_0x21AE
	uint8_t xxxx_0x21AF;//uint8_t byte_0x21AF;//str_0x21AE.str.xxxx_0x21AF
	uint8_t xxxx_0x21B0;//uint8_t byte_0x21B0;//str_0x21AE.str.xxxx_0x21B0
	uint8_t xxxx_0x21B1;//uint8_t byte_0x21B1;//str_0x21AE.str.xxxx_0x21B1
}
type_str_0x21AE;
typedef struct {
	uint8_t xxxx_0x21B2;//uint8_t byte_0x21B2;//str_0x21B2.str.xxxx_0x21B2
	uint8_t xxxx_0x21B3;//uint8_t byte_0x21B3;//str_0x21B2.str.xxxx_0x21B3
	uint8_t cresolution_0x21B4;//uint8_t byte_0x21B4;//str_0x21B2.str.cresolution_0x21B4
	uint8_t xxxx_0x21B5;//uint8_t byte_0x21B5;//str_0x21B2.str.xxxx_0x21B5
}
type_str_0x21B2;
typedef struct {
	uint8_t clights_0x21B6;//uint8_t byte_0x21B6;//str_0x21B6.str.clights_0x21B6
	uint8_t csetting_0x21B7;//uint8_t byte_0x21B7;//str_0x21B6.str.csetting_0x21B7
	uint8_t ctransparency_0x21B8;//uint8_t byte_0x21B8;//str_0x21B6.str.ctransparency_0x21B8
	uint8_t cflat_0x21B9;//uint8_t byte_0x21B9;//str_0x21B6.str.cflat_0x21B9
}
type_str_0x21B6;
#pragma pack (16)
#endif //MAIN_LEVEL

