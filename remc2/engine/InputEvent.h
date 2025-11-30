#pragma once
#include <cstdint>
#pragma pack (push, 1)
struct InputEvent
{
	uint32_t tick = 0;
	bool IsMouse = false;
	uint32_t mouse_buttons = 0;
	int16_t mouse_x = -1;
	int16_t mouse_y = -1;
	bool IsKeyPress = false;
	bool keyPressed = false;
	uint16_t scanCodeChar = 0;
};
#pragma pop;