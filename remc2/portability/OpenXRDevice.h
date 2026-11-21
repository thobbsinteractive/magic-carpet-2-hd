#pragma once
#ifndef OPENXRDEVICE_H
#define OPENXRDEVICE_H

// The VR headset's eye image size, asked of the OpenXR runtime before the game starts (read_config): the game then runs
// in the eyes' shape, as with Looking Glass it runs at the cell size, so the views, the UI, the map and the menus are
// all laid out for the eye rather than stretched into it. Implemented in OpenXRRenderer.cpp.
struct OpenXREyeSize
{
	bool found = false;   // a runtime and a headset
	int width = 0;        // the runtime's recommended image size for one eye
	int height = 0;
};

OpenXREyeSize QueryOpenXREyeSize();

#endif //OPENXRDEVICE_H
