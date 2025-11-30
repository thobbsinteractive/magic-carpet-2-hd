#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <fstream>
#include "../engine/InputEvent.h"

class port_input_recorder
{
private:
	uint32_t m_Tick = 0;
	std::map<uint32_t, InputEvent>* m_InputEvents;

public:
	bool m_IsRecording = false;
	bool m_IsPlaying = false;

	port_input_recorder();
	~port_input_recorder();

	void StartRecording();
	bool StopRecording(std::string outputFileName);
	void IncrementTick();
	InputEvent* GetCurrentInputEvent();
	
	bool StartPlayback(std::string inputFileName);
	void StopPlayback();

	void RecordKeyPress(bool keyPressed, uint16_t scanCodeChar);
	void RecordMouseInput(uint32_t mouse_buttons, int16_t mouse_x, int16_t mouse_y);

	bool SaveRecordingToFile(std::string outputFileName);
	bool LoadRecordingFile(std::string inputFileName);
};

