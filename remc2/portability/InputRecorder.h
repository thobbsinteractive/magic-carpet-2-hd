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
	uint32_t m_Tick = 0;
	uint16_t m_Iteration = 0;
	std::map<uint32_t, std::vector<InputEvent*>*>* m_InputEvents;

public:
	bool m_IsRecording = false;
	bool m_IsPlaying = false;
	const char* m_FilePath;

	InputRecorder(const char* filePath);
	~InputRecorder();

	void PlayPause(const GameState state);
	void StartRecording();
	bool StopRecording();
	void PauseRecording(bool pause);
	void ClearInputEvents();
	void IncrementTick();
	std::vector<InputEvent*>* GetCurrentInputEvents();
	
	bool StartPlayback();
	void StopPlayback();

	void RecordKeyPress(bool keyPressed, uint16_t scanCodeChar);
	void RecordMouseInput(uint32_t mouse_buttons, int16_t mouse_x, int16_t mouse_y);

	bool SaveRecordingToFile(const char* outputFileName);
	bool LoadRecordingFile(const char* inputFileName);
};

