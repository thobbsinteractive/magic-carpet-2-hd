#pragma once
#include <cstdint>
struct InputEvent
{
	uint16_t level = 0;
	uint32_t turn = 0;
	int32_t dword_0x0_0 = 0;
	int16_t word_0x4_4 = 0;
	int16_t word_0x6_6 = 0;
	int16_t word_0x18_24_next_entity = 0;
	int16_t word_0x1A_26 = 0;
};