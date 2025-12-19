#pragma once

#ifndef MAIN_SPELLS
#define MAIN_SPELLS
//#include "build_config.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <ctype.h>
#include <cstdint>
//#include <functional>
//#include <chrono>

//#include "stdint.h"
//#ifdef _MSC_VER
//#include <windows.h>
//#endif
//#include "defs.h"
//#include "global_types.h"

/* ???
Entity Sub-Type - Spell
00 : Fireball
01 : Heal
02 : Speed Up
03 : Posession
04 : Shield
05 : Beyond Sight
06 : Earthquake
07 : Meteor
08 : Volcano
09 : Crater
0A : Teleport
0B : Rubber Band
0C : Invisible
0D : Steal Mana
0E : Rebound
0F : Lightning
10 : Castle
11 : Skeleton
12 : Thunderbolt
13 : Mana Magnet
14 : Fire Wall
15 : Reverse Speed
16 : Smart Bomb
17 : Mini Fireball
*/
#pragma pack (1)
typedef struct {//lenght 26
	int32_t dword_2;
	int32_t dword_6;
	int32_t dword_A;
	int32_t dword_E;
	int32_t dword_0x12;
	int16_t word_0x16x;
	int16_t word_0x18;
	int8_t byte_0x1A;
	uint8_t byte_0x1B;
}
type_SPELLS_BEGIN_BUFFER_str_sub;

typedef struct {//lenght 80
	int8_t byte_0;
	uint8_t byte_1;
	type_SPELLS_BEGIN_BUFFER_str_sub subspell[3];
}
type_SPELLS_BEGIN_BUFFER_str;
#pragma pack (16)
extern type_SPELLS_BEGIN_BUFFER_str SPELLS_BEGIN_BUFFER_str[];
extern void SetDefaultSpells_5C0A0();
#endif //MAIN_SPELLS