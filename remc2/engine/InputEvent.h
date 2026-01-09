#pragma once
#include <cstdint>
struct InputEvent
{
	uint16_t level = 0;
	uint16_t playerIdx = 0;
	uint32_t turn = 0;
	int32_t dword_0x0_0 = 0;
	int16_t word_0x4_4 = 0;
	int16_t word_0x6_6 = 0;
	int16_t word_0x18_24_next_entity = 0;
	int16_t word_0x1A_26 = 0;
	int16_t str_611_SpellIndexLeft_0x451_1105 = 0;
	int8_t str_611_byte_0x455_1109 = 0;
	int16_t str_611_SpellIndexRight_0x453_1107 = 0;
	int8_t str_611_byte_0x456_1110 = 0;
};