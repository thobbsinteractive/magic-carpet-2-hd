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
	const std::string m_FileSignature = "MC2-HD-Recording";
	std::string m_FilePath;
	std::map<uint16_t, InputEvent*>* m_InputEvents;

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

	InputTurn* GetCurrentPlayerActions(int level, int playerIdx, int turn);
	void RecordPlayerActions(uint16_t level, uint16_t playerIdx, uint32_t turn, uint64_t sizeBytes, uint8_t* buffer);

	bool SaveRecordingToFile(const char* outputFileName);
	bool LoadRecordingFile(const char* inputFileName);
};

