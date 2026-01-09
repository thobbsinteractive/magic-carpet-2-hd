#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include "../engine/InputEvent.h"
#include "../engine/GameState.h"

class InputRecorder
{
private:
	std::string m_FilePath;
	std::map<uint16_t, std::map<uint16_t, std::map<uint32_t, InputEvent*>*>*>* m_InputEvents;

public:
	bool m_IsRecording = false;
	bool m_IsPlaying = false;

	InputRecorder(const char* filePath);
	~InputRecorder();

	void PlayPause(const GameState state);
	void StartRecording();
	bool StopRecording();
	void PauseRecording(bool pause);
	void ClearInputEvents();
	
	bool StartPlayback();
	void StopPlayback();

	InputEvent* GetCurrentPlayerActions(int level, int playerIdx, int turn);
	void RecordPlayerActions(uint16_t level, uint16_t playerIdx, uint32_t turn, int32_t dword_0x0_0, int16_t word_0x4_4, int16_t word_0x6_6, int16_t word_0x18_24_next_entity, int16_t word_0x1A_26, int16_t str_611_SpellIndexLeft_0x451_1105, int16_t str_611_SpellIndexRight_0x453_1107, int8_t str_611_byte_0x455_1109, int8_t str_611_byte_0x456_1110);

	bool SaveRecordingToFile(const char* outputFileName);
	bool LoadRecordingFile(const char* inputFileName);
};

