#pragma once
#include <cstdint>
struct InputEventHeader
{
	uint16_t Level = 0;
	uint16_t PlayerCount = 0;
};

struct InputTurn
{
	uint32_t Turn = 0;
	int32_t dword_0x0_0 = 0;
	int16_t Roll_4 = 0;
	int16_t Pitch_6 = 0;
	int16_t Speed_12 = 0;
	int16_t word_0x18_24_next_entity = 0;
	int16_t word_0x1A_26 = 0;
	int16_t SpellIndexLeft_0x451_1105 = 0;
	int16_t SpellIndexRight_0x453_1107 = 0;
	int8_t SubSpellIndexLeft_1109 = 0;
	int8_t SubSpellIndexRight_1110 = 0;
	int32_t Life_8x0 = 0;
	int8_t PlayerAction_byte0 = 0;
};

struct InputPlayer
{
	uint16_t PlayerIdx = 0;
	uint32_t TurnCount = 0;
	std::map<uint32_t, InputTurn*>* Turns = nullptr;
};

struct InputEvent
{
	InputEventHeader* Header = nullptr;
	std::map<uint16_t, InputPlayer*>* Players = nullptr;
};
