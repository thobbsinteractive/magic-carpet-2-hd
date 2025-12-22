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

typedef struct { // 479 // size 14 tj 0xe
	axis_3d axis_2BDE_11695;//11709 0 2 4
	axis_4d rotation__2BDE_11701;//_2BDE_11715 6 8 10 12 //aa
} type_struct_0x1d1_2BDE_11695;

typedef struct {//lenght 2124=0x84C
	dw_w_b dw_w_b_0_2BDE_11230;
	uint8_t byte_0x004_2BE0_11234;//2	//11234 - byte //4
	uint8_t byte_0x005_2BE0_11235;//5
	uint8_t byte_0x006_2BE4_11236;//6	//11236 - byte //6
	int16_t word_0x007_2BE4_11237;//index of player
	int8_t byte_0x009_2BE4_11239;
	uint16_t word_0x00a_2BE4_11240;//10 //11240 - word - index z EA3E4 //0xa
	uint8_t byte_0x00c_2BE0_11242_xx;
	uint8_t byte_0x00d_2BE0_11243_xx;
	int16_t word_0x00e_2BDE_11244; //14
	int16_t word_0x010_2BDE_11246; //16
	int32_t dword_0x012_2BE0_11248;//18 ??
	uint8_t byte_0x016_2BE0_11252_xx;//22
	uint8_t byte_0x017_2BE0_11253_xx;//23
	int32_t dword_0x018_2BDE_11254; //24 //pointer
	char array_0x01c_2BFA_11258[49];//28//11258 - byte* jmeno2?
	int16_t word_0x04d_2C2B_11307;//77 //11307 - word
	uint16_t word_0x04f_2C2D_11309;//79 //11309 - word
	char names_81[8][48];//81 ?8
	type_struct_0x1d1_2BDE_11695 struct_0x1d1_2BDE_11695[33];//465
	char WizardName_0x39f_2BFA_12157[64];//927//12157 - byte(11230+927) 100% name
	uint8_t byte_0x3DF_2BE4_12221;//991//12221 - byte
	uint8_t byte_0x3E0_2BE4_12222;//992//12222 - byte
	uint8_t byte_0x3E1_2BE4_12223;//993//12223 - byte - ok
	uint8_t byte_0x3E2_2BE4_12224;//994//12224 - byte
	uint8_t byte_0x3E3_2BE4_12225;//995
	uint8_t byte_0x3E4_2BE4_12226;//996
	uint8_t stub3[1];
	type_str_164 dword_0x3E6_2BE4_12228;//998//pointer or other
	int8_t byte_0x846_2BDE;//2118
	int8_t byte_0x847_2BDE;//2119
	int16_t word_0x848_2BDE;//2120
	int16_t word_0x84A_2BDE;//2122
} type_str_0x2BDE;//11230 // 84c lenght

typedef union {
	uint32_t dword;
	uint16_t* ptr16u;
	type_entity_0x30311* ptr0x30311;
	type_event_0x6E8E* ptr0x6E8E;
}
un_str_36552;

typedef struct {//size 10 count 8
	int8_t stages_3654C_byte0;//222540
	int8_t str_3654D_byte1;
	axis_2d str_3654E_axis;//objective parametres
	un_str_36552 str_36552_un; //pointer sometimes
} type_str_3654C;

typedef struct {//lenght 10
	int8_t str_0x6E3E_byte0;//0x6E3E-28222//D41A0_BYTESTR_0.array_0x6E3E[xx].str_0x6E3E_byte0
	int8_t str_0x6E3E_byte1;//0x6E3F-28223
	int8_t str_0x6E3E_byte2;//0x6E40-28224
	int8_t roll;//0x6E41-28225
	int8_t pitch;//0x6E42-28226
	int8_t str_0x6E3E_byte5;//0x6E43-28227
	uint16_t str_0x6E3E_word6;//0x6E44-28228
	uint16_t str_0x6E3E_word8;//0x6E46-28230
}type_str_0x6E3E;
/*
3-bit-28225
4-bit-28226
5-bit-28227
6-2bit-28228
8-2bit-28230
*/

//viz. unk_D7BD6

typedef struct {//lenght 108
	uint32_t dword_0x364D2;//lenght 108
	uint32_t dword_0x364D6[26];// lenght 0x68=104 end 0x3653e
}
type_str_0x364D2;

typedef struct {//lenght 39
	int8_t byte_0;
	int8_t byte_1;
	uint8_t byte_2;
	int8_t byte_3;
	axis_3d axis3d_4;
	un1 event_A;
	uint8_t array_E[25];
}
type_str_0x3664C;

typedef struct {//size 11
	uint8_t IsLevelEnd_0;
	uint8_t ObjectiveText_1;
	uint8_t ObjectiveDone_2;
	//uint8_t array_0x3659C_byte[3];//objectives 1-active 2-done
	uint8_t stage_0x3659F[8];
} type_substr_3659C;

typedef struct {//size 11 count 6
	type_substr_3659C substr_3659C;
} type_str_3659C;

typedef struct {//lenght 224791
	std::array<uint8_t, 4> stub0;
	uint32_t dword_0x4;
	uint32_t rand_0x8;
	int16_t LevelIndex_0xc;//player_index?
	int16_t word_0xe;
	std::array<uint8_t, 29> array_0x10; //0x10, next 0x2d(45)
	int32_t dword_0x2d;//45
	uint16_t word_0x31;//49
	uint16_t word_0x33;//51
	int32_t dword_0x35;//53//entity counter(max 1000 entity changes per step)
	std::array<uint8_t, 508> array_0x39;//57
	int32_t maptypeMusic_0x235;//act music
	uint8_t byte_0x239;//569
	int32_t dword_0x23a;
	int32_t dword_0x23e;
	int32_t dword_0x242;
	std::array<type_event_0x6E8E*, 1000> pointers_0x246;//pointers
	int32_t dword_0x11e6;//second entity counter
	std::array<type_event_0x6E8E*, 1000> dword_0x11EA;//??4586
	GameSettingsStruct_t m_GameSettings;
	uint32_t dword_0x219A;//8602
	uint32_t dword_0x219E;//8606
	uint32_t dword_0x21A2;//8610
	uint32_t dword_0x21A6;//8614
	type_str_0x21AA str_0x21AA;
	type_str_0x21AE str_0x21AE;
	type_str_0x21B2 str_0x21B2;
	type_str_0x21B6 str_0x21B6;
	std::array<uint8_t, 424> stub3b;
	std::array<axis_3d, 8> array_0x2362;
	std::array<uint8_t, 333> stub3c;
	std::array<uint8_t, 1791> stub3d;
	std::array<type_str_0x2BDE, 8> array_0x2BDE;
	type_str_0x6E3E array_0x6E3E[8];//28222	lenght 0xa size 0x8// game events
	type_event_0x6E8E struct_0x6E8E[1000];//28302 a8*3e8
	Type_Level_2FECE terrain_2FECE;// a1 = &x_D41A0_BYTEARRAY_0[0x2FECE/*196302*/];//fix - size 0x6604u//compress level
	type_str_0x364D2 str_0x364D2;//lenght 108
	int16_t word_0x3653E;//
	int16_t word_0x36540;//
	int16_t word_0x36542;//
	int16_t word_0x36544;//
	int16_t word_0x36546;//
	int16_t word_0x36548;//
	int16_t word_0x3654A;//
	type_str_3654C stages_0x3654C[8];//size 10 count 8
	type_str_3659C struct_0x3659C[8];//size 11 count 8 C-0 D-1 E-2 F-3
	type_str_0x3647Ac StageVars2_0x365F4[11];//8x11	//set 0x58 // end 0x3664c
	type_str_0x3664C str_0x3664C[50];// lenght 0x79e//39x50 (end-0x36dea)
	int8_t byte_0x36DEA_fly_asistant;
	int8_t byte_0x36DEB_xx;
	int16_t word_0x36DEC_mousex;
	int16_t word_0x36DEE_mousey;
	int16_t word_0x36DF0_mousexx;
	uint32_t dword_0x36DF2;//index
	type_str_160* dword_0x36DF6;//pointer
	int16_t word_0x36DFA;
	int16_t word_0x36DFC;
	int16_t word_0x36DFE;
	uint8_t countStageVars_0x36E00;
	uint8_t stageIndex_0x36E01;//count objectives
	int8_t byte_0x36E02;//temp objective
	int8_t byte_0x36E03;
	uint8_t byte_counter_current_objective_box_0x36E04;
	uint8_t stub3k[6];
	int8_t byte_0x36E0B;
	uint8_t stubend[11];
} type_D41A0_BYTESTR_0; // level data (gets saved to and loaded from SAVE/SLEVxx.dat) was originally forced to 224791 on load/save which is out of bounds


#pragma pack (16)

#endif //MAIN_LEVEL_STRUCTS

