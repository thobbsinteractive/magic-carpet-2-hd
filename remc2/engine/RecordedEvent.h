#pragma once
#include <cstdint>
struct RecordedEventHeader
{
	uint16_t Level = 0;
	uint16_t PlayerCount = 0;
};

struct RecordedEventTurn
{
	uint32_t Turn = 0;
	uint32_t SizeBytes = 0;
	uint8_t* Bytes = nullptr;
};

struct RecordedEventPlayer
{
	uint16_t PlayerIdx = 0;
	uint32_t TurnCount = 0;
	std::map<uint32_t, RecordedEventTurn*>* Turns = nullptr;
};

struct RecordedEvent
{
	RecordedEventHeader* Header = nullptr;
	std::map<uint16_t, RecordedEventPlayer*>* Players = nullptr;
};
