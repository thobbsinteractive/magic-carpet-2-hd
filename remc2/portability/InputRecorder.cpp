#include "InputRecorder.h"
#include "../engine/EventDispatcher.h"
#include "../engine/GameState.h"
using namespace std;

InputRecorder::InputRecorder(const char* filePath)
{
	m_FilePath = filePath;
	m_InputEvents = new std::map<uint16_t, InputEvent*>();
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
	m_IsRecording = true;
}

void InputRecorder::ClearInputEvents()
{
	map<uint16_t, InputEvent*>::iterator levelIt;
	map<uint16_t, InputPlayer*>::iterator playIt;
	map<uint32_t, InputTurn*>::iterator turnIt;

	for (levelIt = m_InputEvents->begin(); levelIt != m_InputEvents->end(); levelIt++)
	{

		for (playIt = levelIt->second->Players->begin(); playIt != levelIt->second->Players->end(); playIt++)
		{
			for (turnIt = playIt->second->Turns->begin(); turnIt != playIt->second->Turns->end(); turnIt++)
			{
				delete turnIt->second;
			}
			playIt->second->Turns->clear();
			delete playIt->second;
		}
		levelIt->second->Players->clear();
		delete levelIt->second;
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

InputTurn* InputRecorder::GetCurrentPlayerActions(int level, int playerIdx, int turn)
{
	if (!m_IsPlaying || m_InputEvents->count(level) == 0 || m_InputEvents->at(level)->Players->count(playerIdx) == 0 || m_InputEvents->at(level)->Players->at(playerIdx)->Turns->count(turn) == 0)
		return nullptr;

	return m_InputEvents->at(level)->Players->at(playerIdx)->Turns->at(turn);
}

void InputRecorder::RecordPlayerActions(uint16_t level, uint16_t playerIdx, uint32_t turn, uint64_t sizeBytes, uint8_t* buffer)
{
	if (!m_IsRecording)
		return;

	if (m_InputEvents->count(level) == 0) 
	{
		m_InputEvents->insert(std::pair<uint16_t, InputEvent*>(level, new InputEvent()));
		m_InputEvents->at(level)->Header = new InputEventHeader();
		m_InputEvents->at(level)->Header->Level = level;
		m_InputEvents->at(level)->Players = new std::map<uint16_t, InputPlayer*>();
	}
	if (m_InputEvents->at(level)->Players->count(playerIdx) == 0)
	{
		m_InputEvents->at(level)->Players->insert(std::pair<uint16_t, InputPlayer*>(playerIdx, new InputPlayer()));
		m_InputEvents->at(level)->Players->at(playerIdx) = new InputPlayer();
		m_InputEvents->at(level)->Players->at(playerIdx)->PlayerIdx = playerIdx;
		m_InputEvents->at(level)->Players->at(playerIdx)->Turns = new std::map<uint32_t, InputTurn*>();
	}
	if (m_InputEvents->at(level)->Players->at(playerIdx)->Turns->count(turn) == 0)
	{
		m_InputEvents->at(level)->Players->at(playerIdx)->Turns->insert({ turn, { new InputTurn() } });
	}
	m_InputEvents->at(level)->Players->at(playerIdx)->Turns->at(turn)->Turn = turn;

	m_InputEvents->at(level)->Players->at(playerIdx)->Turns->at(turn)->SizeBytes = sizeBytes;
	m_InputEvents->at(level)->Players->at(playerIdx)->Turns->at(turn)->Bytes = new uint8_t[sizeBytes];
	memcpy(m_InputEvents->at(level)->Players->at(playerIdx)->Turns->at(turn)->Bytes, buffer, sizeBytes);
}

bool InputRecorder::SaveRecordingToFile(const char* outputFileName)
{
	try
	{
		if (m_InputEvents == nullptr || m_InputEvents->empty())
			return false;

		FILE* eventsFile = fopen(outputFileName, "wb");
		if (!eventsFile)
			return false;

		std::string fileSignature = "MC2-HD-Recording";

		fwrite((uint8_t*)fileSignature.c_str(), fileSignature.length() * sizeof(char), 1, eventsFile);

		std::vector<InputTurn*>* playerTurns = new std::vector<InputTurn*>();

		map<uint16_t, InputEvent*>::iterator levelIt;
		map<uint16_t, InputPlayer*>::iterator playIt;
		map<uint32_t, InputTurn*>::iterator turnIt;

		for (levelIt = m_InputEvents->begin(); levelIt != m_InputEvents->end(); levelIt++)
		{
			int level = levelIt->first;
			auto* inputEventHeader = new InputEventHeader();
			inputEventHeader->Level = level;
			inputEventHeader->PlayerCount = levelIt->second->Players->size();
			
			fwrite((uint8_t*)inputEventHeader, sizeof(InputEventHeader), 1, eventsFile);

			for (playIt = levelIt->second->Players->begin(); playIt != levelIt->second->Players->end(); playIt++)
			{
				uint16_t playerIndex = playIt->first;
				for (turnIt = playIt->second->Turns->begin(); turnIt != playIt->second->Turns->end(); turnIt++)
				{
					playerTurns->push_back(turnIt->second);
				}
				uint32_t turnCount = playerTurns->size();

				fwrite(&playerIndex, sizeof(uint16_t), 1, eventsFile);
				fwrite(&turnCount, sizeof(uint32_t), 1, eventsFile);

				for (int i = 0; i < playerTurns->size(); i++)
				{
					auto turn = playerTurns->at(i);
					fwrite(turn, 8, 1, eventsFile);
					fwrite(turn->Bytes, playerTurns->at(i)->SizeBytes, 1, eventsFile);
				}
				
				playerTurns->clear();
			}
			delete inputEventHeader;
		}
		delete playerTurns;
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

		uint16_t level = 0;
		uint16_t playerCount = 0;

		char* fileSignature = new char[17];

		fread(fileSignature, sizeof(char), 16, eventsFile);
		fileSignature[16] = NULL;

		if (strcmp(fileSignature, m_FileSignature.c_str()) != 0)
			return false;

		while (fread(&level, sizeof(InputEventHeader::Level), 1, eventsFile))
		{
			fread(&playerCount, sizeof(InputEventHeader::PlayerCount), 1, eventsFile);

			if (m_InputEvents->count(level) == 0)
			{
				m_InputEvents->insert(std::pair<uint16_t, InputEvent*>(level, new InputEvent()));
				m_InputEvents->at(level)->Header = new InputEventHeader();
				m_InputEvents->at(level)->Header->Level = level;
				m_InputEvents->at(level)->Header->PlayerCount = playerCount;
				m_InputEvents->at(level)->Players = new std::map<uint16_t, InputPlayer*>();
			}


			uint16_t playerIdx = 0;
			uint32_t turnCount = 0;
			while (playerCount--)
			{
				fread(&playerIdx, sizeof(InputPlayer::PlayerIdx), 1, eventsFile);
				fread(&turnCount, sizeof(InputPlayer::TurnCount), 1, eventsFile);

				if (m_InputEvents->at(level)->Players->count(playerIdx) == 0)
				{
					m_InputEvents->at(level)->Players->insert(std::pair<uint16_t, InputPlayer*>(playerIdx, new InputPlayer()));
					m_InputEvents->at(level)->Players->at(playerIdx)->PlayerIdx = playerIdx;
					m_InputEvents->at(level)->Players->at(playerIdx)->TurnCount = turnCount;
					m_InputEvents->at(level)->Players->at(playerIdx)->Turns = new std::map<uint32_t, InputTurn*>();
				}

				for (int i = 0; i < turnCount; i++)
				{
					InputTurn* turn = new InputTurn();
					fread(turn, 8, 1, eventsFile);
					turn->Bytes = new uint8_t[turn->SizeBytes];
					fread(turn->Bytes, turn->SizeBytes, 1, eventsFile);
					m_InputEvents->at(level)->Players->at(playerIdx)->Turns->insert({ turn->Turn, turn });
				}
			}
		}
		return fclose(eventsFile) == 0;
	}
	catch (exception ex)
	{
		return false;
	}
}
