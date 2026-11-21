#pragma once

// A VR headset's head orientation, sent by the OpenXR renderer with EventType::E_VR_HEAD_POSE once a frame and read by
// the game's VR code (Vr.cpp), which turns it into the original head tracker's angles.
// Angles in radians, OpenXR's convention: yaw about the up axis (+ = turned left), pitch about the right axis
// (+ = looking up), roll about the view direction (+ = tilted left).
struct VrHeadPose
{
	bool active = false;   // false: the headset stopped tracking (or its session ended): the head looks straight ahead
	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;
};
