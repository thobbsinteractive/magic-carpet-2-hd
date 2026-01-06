#include "InputRecorder.h"
#include "../engine/EventDispatcher.h"
#include "../engine/GameState.h"
using namespace std;

InputRecorder::InputRecorder(const char* filePath)
{
	m_FilePath = filePath;
	m_InputEvents = new std::map<uint32_t, InputEvent*>();
	std::function<void(GameState)> stateChangeCallBack = [this](GameState a) { this->PlayPause(a); };
	EventDispatcher::I->RegisterEvent(new Event<GameState>(EventType::E_GAME_STATE_CHANGE, stateChangeCallBack));
}

InputRecorder::~InputRecorder()
{
	ClearInputEvents();
	delete m_InputEvents;
}

void InputRecorder::PlayPause(const GameState state)
{
	switch (state)
	{
		case GameState::GAMEPLAY_LOADING:
			PauseRecording(true);
		case GameState::GAMEPLAY_STARTED:
			PauseRecording(false);
	};
}

void InputRecorder::StartRecording()
{
	ClearInputEvents();
	m_IsRecording = true;
}

void InputRecorder::ClearInputEvents()
{
	map<uint32_t, InputEvent*>::iterator it;
	for (it = m_InputEvents->begin(); it != m_InputEvents->end(); it++)
	{
		delete it->second;
	}
	m_InputEvents->clear();
}

bool InputRecorder::StopRecording()
{
	m_IsRecording = false;
	if (SaveRecordingToFile(m_FilePath.c_str()))
	{
		ClearInputEvents();
		return true;
	}
	return false;
}

void InputRecorder::PauseRecording(bool pause)
{
	m_IsRecording = !pause;
	m_IsPlaying = !pause;
}

bool InputRecorder::StartPlayback()
{
	if (LoadRecordingFile(m_FilePath.c_str()))
		m_IsPlaying = true;

	return m_IsPlaying;
}

void InputRecorder::StopPlayback()
{
	m_IsPlaying = false;
}

InputEvent* InputRecorder::GetCurrentPlayerActions(int turn)
{
	if (!m_IsPlaying || m_InputEvents->count(turn) == 0)
		return nullptr;

	return m_InputEvents->at(turn);
}

void InputRecorder::RecordPlayerActions(uint32_t turn, int32_t dword_0x0_0, int16_t word_0x4_4, int16_t word_0x6_6, int16_t word_0x18_24_next_entity, int16_t word_0x1A_26)
{
	if (!m_IsRecording)
		return;

	if (m_InputEvents->count(turn) == 0) {
		m_InputEvents->insert({ turn, new InputEvent() });
	}
	m_InputEvents->at(turn)->turn = turn;
	m_InputEvents->at(turn)->dword_0x0_0 = dword_0x0_0;
	m_InputEvents->at(turn)->word_0x4_4 = word_0x4_4;
	m_InputEvents->at(turn)->word_0x6_6 = word_0x6_6;
	m_InputEvents->at(turn)->word_0x18_24_next_entity = word_0x18_24_next_entity;
	m_InputEvents->at(turn)->word_0x1A_26 = word_0x1A_26;
}

bool InputRecorder::SaveRecordingToFile(const char* outputFileName)
{
	try
	{
		FILE* eventsFile = fopen(outputFileName, "wb");
		if (!eventsFile)
			return false;

		map<uint32_t, InputEvent*>::iterator it;
		for (it = m_InputEvents->begin(); it != m_InputEvents->end(); it++)
		{
			fwrite((uint8_t*)it->second, sizeof(InputEvent), 1, eventsFile);
		}
		return fclose(eventsFile) == 0;
	}
	catch (exception ex)
	{
		return false;
	}
}

bool InputRecorder::LoadRecordingFile(const char* inputFileName)
{
	try
	{
		FILE* eventsFile = fopen(inputFileName, "rb");
		if (eventsFile == nullptr)
			return false;

		uint32_t turn = 0;
		uint16_t iteration = 0;
		while (fread(&turn, sizeof(InputEvent::turn), 1, eventsFile))
		{
			m_InputEvents->insert(std::pair<uint32_t, InputEvent*>(turn, new InputEvent()));
			m_InputEvents->at(turn)->turn = turn;
			fread(&m_InputEvents->at(turn)->dword_0x0_0, sizeof(InputEvent::dword_0x0_0), 1, eventsFile);
			fread(&m_InputEvents->at(turn)->word_0x4_4, sizeof(InputEvent::word_0x4_4), 1, eventsFile);
			fread(&m_InputEvents->at(turn)->word_0x6_6, sizeof(InputEvent::word_0x6_6), 1, eventsFile);
			fread(&m_InputEvents->at(turn)->word_0x18_24_next_entity, sizeof(InputEvent::word_0x18_24_next_entity), 1, eventsFile);
			fread(&m_InputEvents->at(turn)->word_0x1A_26, sizeof(InputEvent::word_0x1A_26), 1, eventsFile);
		}
		return fclose(eventsFile) == 0;
	}
	catch (exception ex)
	{
		return false;
	}
}
