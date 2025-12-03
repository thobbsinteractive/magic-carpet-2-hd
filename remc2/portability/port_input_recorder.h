#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <fstream>
#include "../engine/InputEvent.h"
#include <vector>

class port_input_recorder
{
private:
	uint32_t m_Tick = 0;
	uint16_t m_Iteration = 0;
	std::map<uint32_t, std::vector<InputEvent*>*>* m_InputEvents;

public:
	bool m_IsRecording = false;
	bool m_IsPlaying = false;

	port_input_recorder();
	~port_input_recorder();

	void StartRecording();
	bool StopRecording(const char* outputFileName);
	void ClearInputEvents();
	void IncrementTick();
	std::vector<InputEvent*>* GetCurrentInputEvents();
	
	bool StartPlayback(const char* inputFileName);
	void StopPlayback();

	void RecordKeyPress(bool keyPressed, uint16_t scanCodeChar);
	void RecordMouseInput(uint32_t mouse_buttons, int16_t mouse_x, int16_t mouse_y);

	bool SaveRecordingToFile(const char* outputFileName);
	bool LoadRecordingFile(const char* inputFileName);
};

