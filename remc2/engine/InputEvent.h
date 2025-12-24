#pragma once
#include <cstdint>
struct InputEvent
{
	uint32_t tick = 0;
	uint16_t iteration = 0;
	bool isMouse = false;
	uint32_t mouse_buttons = 0;
	int16_t mouse_x = -1;
	int16_t mouse_y = -1;
	bool isKeyPress = false;
	bool keyPressed = false;
	uint16_t gameKeyChar = 0;
};