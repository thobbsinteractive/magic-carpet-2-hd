#include "port_input_recorder.h"
using namespace std;

port_input_recorder::port_input_recorder()
{
	m_InputEvents = new std::map<uint32_t, InputEvent*>();
}

port_input_recorder::~port_input_recorder()
{
	ClearInputEvents();
}

void port_input_recorder::StartRecording()
{
	m_Tick = 0;
	ClearInputEvents();
	m_IsRecording = true;
}

void port_input_recorder::ClearInputEvents()
{
	map<uint32_t, InputEvent*>::iterator it;
	for (it = m_InputEvents->begin(); it != m_InputEvents->end(); it++)
	{
		delete it->second;
	}
	m_InputEvents->clear();
}

bool port_input_recorder::StopRecording(std::string outputFileName)
{
	m_IsRecording = false;
	if (SaveRecordingToFile(outputFileName))
	{
		m_Tick = 0;
		ClearInputEvents();
		return true;
	}
	return false;
}

bool port_input_recorder::StartPlayback(std::string inputFileName)
{
	m_Tick = 0;
	if (LoadRecordingFile(inputFileName))
		m_IsPlaying = true;

	return m_IsPlaying;
}

void port_input_recorder::StopPlayback()
{
	m_IsPlaying = false;
}

void port_input_recorder::IncrementTick()
{
	m_Tick++;
}

InputEvent* port_input_recorder::GetCurrentInputEvent()
{
	if (!m_IsPlaying || m_InputEvents->count(m_Tick) == 0)
		return nullptr;

	return m_InputEvents->at(m_Tick);
}

void port_input_recorder::RecordKeyPress(bool keyPressed, uint16_t scanCodeChar)
{
	if (!m_IsRecording)
		return;

	if (m_InputEvents->count(m_Tick) == 0) {
		m_InputEvents->insert(std::pair<int, InputEvent*>(m_Tick, new InputEvent()));
		m_InputEvents->at(m_Tick)->tick = m_Tick;
	}
	m_InputEvents->at(m_Tick)->IsKeyPress = true;
	m_InputEvents->at(m_Tick)->keyPressed = keyPressed;
	m_InputEvents->at(m_Tick)->scanCodeChar = scanCodeChar;
}

void port_input_recorder::RecordMouseInput(uint32_t mouse_buttons, int16_t mouse_x, int16_t mouse_y)
{
	if (!m_IsRecording)
		return;

	if (m_InputEvents->count(m_Tick) == 0) {
		m_InputEvents->insert(std::pair<uint32_t, InputEvent*>(m_Tick, new InputEvent()));
		m_InputEvents->at(m_Tick)->tick = m_Tick;
	}
	m_InputEvents->at(m_Tick)->IsMouse = true;
	m_InputEvents->at(m_Tick)->mouse_buttons = mouse_buttons;
	m_InputEvents->at(m_Tick)->mouse_x = mouse_x;
	m_InputEvents->at(m_Tick)->mouse_y = mouse_y;
}

bool port_input_recorder::SaveRecordingToFile(std::string outputFileName)
{
	outputFileName = "c:\\temp\\testRecording.bin";
	
	FILE* eventsFile = fopen(outputFileName.c_str(), "wb");
	if (!eventsFile)
		return false;

	map<uint32_t, InputEvent*>::iterator it;
	for (it = m_InputEvents->begin(); it != m_InputEvents->end(); it++)
	{
		fwrite((uint8_t*)it->second, sizeof(InputEvent), 1, eventsFile);
	}
	return fclose(eventsFile) == 0;
}

bool port_input_recorder::LoadRecordingFile(std::string inputFileName)
{
	inputFileName = "c:\\temp\\testRecording.bin";

	FILE* eventsFile = fopen(inputFileName.c_str(), "rb");
	if (eventsFile == nullptr)
		return false;

	uint32_t tick = 0;
	while (fread(&tick, sizeof(InputEvent::tick), 1, eventsFile))
	{
		m_InputEvents->insert(std::pair<uint32_t, InputEvent*>(tick, new InputEvent()));
		m_InputEvents->at(tick)->tick = tick;
		fread(&m_InputEvents->at(tick)->IsMouse, sizeof(InputEvent::IsMouse), 1, eventsFile);
		fread(&m_InputEvents->at(tick)->mouse_buttons, sizeof(InputEvent::mouse_buttons), 1, eventsFile);
		fread(&m_InputEvents->at(tick)->mouse_x, sizeof(InputEvent::mouse_x), 1, eventsFile);
		fread(&m_InputEvents->at(tick)->mouse_y, sizeof(InputEvent::mouse_y), 1, eventsFile);
		fread(&m_InputEvents->at(tick)->IsKeyPress, sizeof(InputEvent::IsKeyPress), 1, eventsFile);
		fread(&m_InputEvents->at(tick)->keyPressed, sizeof(InputEvent::keyPressed), 1, eventsFile);
		fread(&m_InputEvents->at(tick)->scanCodeChar, sizeof(InputEvent::scanCodeChar), 1, eventsFile);
	}
	return fclose(eventsFile) == 0;
}
