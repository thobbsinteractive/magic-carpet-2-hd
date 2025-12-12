#include "InputRecorder.h"
using namespace std;

InputRecorder::InputRecorder()
{
	m_InputEvents = new std::map<uint32_t, std::vector<InputEvent*>*>();
}

InputRecorder::~InputRecorder()
{
	ClearInputEvents();
	delete m_InputEvents;
}

void InputRecorder::StartRecording()
{
	m_Tick = 0;
	m_Iteration = 0;
	ClearInputEvents();
	m_IsRecording = true;
}

void InputRecorder::ClearInputEvents()
{
	map<uint32_t, std::vector<InputEvent*>*>::iterator it;
	for (it = m_InputEvents->begin(); it != m_InputEvents->end(); it++)
	{
		for (int i = 0; i < it->second->size(); i++)
			delete it->second->at(i);
		it->second->clear();
	}
	m_InputEvents->clear();
}

bool InputRecorder::StopRecording(const char* outputFileName)
{
	m_IsRecording = false;
	if (SaveRecordingToFile(outputFileName))
	{
		m_Tick = 0;
		m_Iteration = 0;
		ClearInputEvents();
		return true;
	}
	return false;
}

bool InputRecorder::StartPlayback(const char* inputFileName)
{
	m_Tick = 0;
	m_Iteration = 0;
	if (LoadRecordingFile(inputFileName))
		m_IsPlaying = true;

	return m_IsPlaying;
}

void InputRecorder::StopPlayback()
{
	m_IsPlaying = false;
}

void InputRecorder::IncrementTick()
{
	m_Tick++;
	m_Iteration = 0;
	if (m_Tick > m_InputEvents->rbegin()->first)
	{
		StopPlayback();
	}
}

std::vector<InputEvent*>* InputRecorder::GetCurrentInputEvents()
{
	if (!m_IsPlaying || m_InputEvents->count(m_Tick) == 0)
		return nullptr;

	return m_InputEvents->at(m_Tick);
}

void InputRecorder::RecordKeyPress(bool keyPressed, uint16_t scanCodeChar)
{
	if (!m_IsRecording)
		return;

	if (m_InputEvents->count(m_Tick) == 0) {
		m_InputEvents->insert(std::pair<int, std::vector<InputEvent*>*>(m_Tick, new std::vector<InputEvent*>()));
	}
	m_InputEvents->at(m_Tick)->push_back(new InputEvent());
	m_InputEvents->at(m_Tick)->at(m_Iteration)->tick = m_Tick;
	m_InputEvents->at(m_Tick)->at(m_Iteration)->iteration = m_Iteration;
	m_InputEvents->at(m_Tick)->at(m_Iteration)->isKeyPress = true;
	m_InputEvents->at(m_Tick)->at(m_Iteration)->keyPressed = keyPressed;
	m_InputEvents->at(m_Tick)->at(m_Iteration)->scanCodeChar = scanCodeChar;
	m_Iteration++;
}

void InputRecorder::RecordMouseInput(uint32_t mouse_buttons, int16_t mouse_x, int16_t mouse_y)
{
	if (!m_IsRecording)
		return;

	if (m_InputEvents->count(m_Tick) == 0) {
		m_InputEvents->insert(std::pair<int, std::vector<InputEvent*>*>(m_Tick, new std::vector<InputEvent*>()));
	}
	m_InputEvents->at(m_Tick)->push_back(new InputEvent());
	m_InputEvents->at(m_Tick)->at(m_Iteration)->tick = m_Tick;
	m_InputEvents->at(m_Tick)->at(m_Iteration)->iteration = m_Iteration;
	m_InputEvents->at(m_Tick)->at(m_Iteration)->isMouse = true;
	m_InputEvents->at(m_Tick)->at(m_Iteration)->mouse_buttons = mouse_buttons;
	m_InputEvents->at(m_Tick)->at(m_Iteration)->mouse_x = mouse_x;
	m_InputEvents->at(m_Tick)->at(m_Iteration)->mouse_y = mouse_y;
	m_Iteration++;
}

bool InputRecorder::SaveRecordingToFile(const char* outputFileName)
{
	try
	{
		FILE* eventsFile = fopen(outputFileName, "wb");
		if (!eventsFile)
			return false;

		map<uint32_t, std::vector<InputEvent*>*>::iterator it;
		for (it = m_InputEvents->begin(); it != m_InputEvents->end(); it++)
		{
			for (int i = 0; i < it->second->size(); i++)
				fwrite((uint8_t*)it->second->at(i), sizeof(InputEvent), 1, eventsFile);
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

		uint32_t tick = 0;
		uint16_t iteration = 0;
		while (fread(&tick, sizeof(InputEvent::tick), 1, eventsFile))
		{
			fread(&iteration, sizeof(InputEvent::iteration), 1, eventsFile);
			if (iteration == 0)
			{
				m_InputEvents->insert(std::pair<uint32_t, std::vector<InputEvent*>*>(tick, new std::vector<InputEvent*>()));
			}
			m_InputEvents->at(tick)->push_back(new InputEvent());
			m_InputEvents->at(tick)->at(iteration)->tick = tick;
			m_InputEvents->at(tick)->at(iteration)->iteration = iteration;
			fread(&m_InputEvents->at(tick)->at(iteration)->isMouse, sizeof(InputEvent::isMouse), 1, eventsFile);
			fseek(eventsFile, 1, SEEK_CUR); //padding
			fread(&m_InputEvents->at(tick)->at(iteration)->mouse_buttons, sizeof(InputEvent::mouse_buttons), 1, eventsFile);
			fread(&m_InputEvents->at(tick)->at(iteration)->mouse_x, sizeof(InputEvent::mouse_x), 1, eventsFile);
			fread(&m_InputEvents->at(tick)->at(iteration)->mouse_y, sizeof(InputEvent::mouse_y), 1, eventsFile);
			fread(&m_InputEvents->at(tick)->at(iteration)->isKeyPress, sizeof(InputEvent::isKeyPress), 1, eventsFile);
			fread(&m_InputEvents->at(tick)->at(iteration)->keyPressed, sizeof(InputEvent::keyPressed), 1, eventsFile);
			fread(&m_InputEvents->at(tick)->at(iteration)->scanCodeChar, sizeof(InputEvent::scanCodeChar), 1, eventsFile);
		}
		return fclose(eventsFile) == 0;
	}
	catch (exception ex)
	{
		return false;
	}
}
