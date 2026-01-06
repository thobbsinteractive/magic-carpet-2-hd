#include "InputRecorder.h"
#include "../engine/EventDispatcher.h"
#include "../engine/GameState.h"
using namespace std;

InputRecorder::InputRecorder(const char* filePath)
{
	m_FilePath = filePath;
	m_InputEvents = new std::map<uint32_t, std::map<uint32_t, InputEvent*>*>();
	//std::function<void(GameState)> stateChangeCallBack = [this](GameState a) { this->PlayPause(a); };
	//EventDispatcher::I->RegisterEvent(new Event<GameState>(EventType::E_GAME_STATE_CHANGE, stateChangeCallBack));
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
	map<uint32_t, map<uint32_t, InputEvent*>*>::iterator it;
	map<uint32_t, InputEvent*>::iterator itturn;
	for (it = m_InputEvents->begin(); it != m_InputEvents->end(); it++)
	{
		for (itturn = it->second->begin(); itturn != it->second->end(); itturn++)
		{
			delete itturn->second;
		}
		it->second->clear();
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

InputEvent* InputRecorder::GetCurrentPlayerActions(int level, int turn)
{
	if (!m_IsPlaying || m_InputEvents->count(level) == 0 || m_InputEvents->at(level)->count(turn) == 0)
		return nullptr;

	return m_InputEvents->at(level)->at(turn);
}

void InputRecorder::RecordPlayerActions(uint32_t level, uint32_t turn, int32_t dword_0x0_0, int16_t word_0x4_4, int16_t word_0x6_6, int16_t word_0x18_24_next_entity, int16_t word_0x1A_26, int16_t str_611_SpellIndexLeft_0x451_1105, int16_t str_611_SpellIndexRight_0x453_1107, int8_t str_611_byte_0x455_1109, int8_t str_611_byte_0x456_1110)
{
	if (!m_IsRecording)
		return;

	if (m_InputEvents->count(level) == 0) {
		m_InputEvents->insert(std::pair<uint32_t, std::map<uint32_t, InputEvent*>*>(level, new std::map<uint32_t, InputEvent*>()));
	}
	if (m_InputEvents->at(level)->count(turn) == 0)
	{
		m_InputEvents->at(level)->insert({ turn, { new InputEvent() } });
	}
	m_InputEvents->at(level)->at(turn)->level = level;
	m_InputEvents->at(level)->at(turn)->turn = turn;
	m_InputEvents->at(level)->at(turn)->dword_0x0_0 = dword_0x0_0;
	m_InputEvents->at(level)->at(turn)->word_0x4_4 = word_0x4_4;
	m_InputEvents->at(level)->at(turn)->word_0x6_6 = word_0x6_6;
	m_InputEvents->at(level)->at(turn)->word_0x18_24_next_entity = word_0x18_24_next_entity;
	m_InputEvents->at(level)->at(turn)->word_0x1A_26 = word_0x1A_26;
	m_InputEvents->at(level)->at(turn)->str_611_SpellIndexLeft_0x451_1105 = str_611_SpellIndexLeft_0x451_1105;
	m_InputEvents->at(level)->at(turn)->str_611_SpellIndexRight_0x453_1107 = str_611_SpellIndexRight_0x453_1107;
	m_InputEvents->at(level)->at(turn)->str_611_byte_0x455_1109 = str_611_byte_0x455_1109;
	m_InputEvents->at(level)->at(turn)->str_611_byte_0x456_1110 = str_611_byte_0x456_1110;

}

bool InputRecorder::SaveRecordingToFile(const char* outputFileName)
{
	try
	{
		FILE* eventsFile = fopen(outputFileName, "wb");
		if (!eventsFile)
			return false;

		map<uint32_t, map<uint32_t, InputEvent*>*>::iterator it;
		for (it = m_InputEvents->begin(); it != m_InputEvents->end(); it++)
		{
			map<uint32_t, InputEvent*>::iterator it2;
			for (it2 = it->second->begin(); it2 != it->second->end(); it2++)
			{
				fwrite((uint8_t*)it2->second, sizeof(InputEvent), 1, eventsFile);
			}
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

		uint32_t level = 0;
		int32_t currentLevel = -1;
		uint32_t turn = 0;
		while (fread(&level, sizeof(InputEvent::level), 1, eventsFile))
		{
			fread(&turn, sizeof(InputEvent::turn), 1, eventsFile);

			if (level != currentLevel)
			{
				m_InputEvents->insert(std::pair<uint32_t, std::map<uint32_t, InputEvent*>*>(level, new std::map<uint32_t, InputEvent*>()));
				currentLevel = level;
			}
			m_InputEvents->at(level)->insert({turn, new InputEvent() });

			fread(&m_InputEvents->at(level)->at(turn)->dword_0x0_0, sizeof(InputEvent::dword_0x0_0), 1, eventsFile);
			fread(&m_InputEvents->at(level)->at(turn)->word_0x4_4, sizeof(InputEvent::word_0x4_4), 1, eventsFile);
			fread(&m_InputEvents->at(level)->at(turn)->word_0x6_6, sizeof(InputEvent::word_0x6_6), 1, eventsFile);
			fread(&m_InputEvents->at(level)->at(turn)->word_0x18_24_next_entity, sizeof(InputEvent::word_0x18_24_next_entity), 1, eventsFile);
			fread(&m_InputEvents->at(level)->at(turn)->word_0x1A_26, sizeof(InputEvent::word_0x1A_26), 1, eventsFile);
			fread(&m_InputEvents->at(level)->at(turn)->str_611_SpellIndexLeft_0x451_1105, sizeof(InputEvent::str_611_SpellIndexLeft_0x451_1105), 1, eventsFile);
			fread(&m_InputEvents->at(level)->at(turn)->str_611_byte_0x455_1109, sizeof(InputEvent::str_611_byte_0x455_1109), 1, eventsFile);
			fseek(eventsFile, 1, SEEK_CUR); //padding
			fread(&m_InputEvents->at(level)->at(turn)->str_611_SpellIndexRight_0x453_1107, sizeof(InputEvent::str_611_SpellIndexRight_0x453_1107), 1, eventsFile);
			fread(&m_InputEvents->at(level)->at(turn)->str_611_byte_0x456_1110, sizeof(InputEvent::str_611_byte_0x456_1110), 1, eventsFile);
			fseek(eventsFile, 1, SEEK_CUR); //padding
		}
		return fclose(eventsFile) == 0;
	}
	catch (exception ex)
	{
		return false;
	}
}
