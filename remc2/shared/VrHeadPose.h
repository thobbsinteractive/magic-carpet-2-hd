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
	// Each eye (0 left, 1 right): where straight ahead is in its image, from the centre, a fraction of the width (+ to
	// the right). The runtime's fields of view are asymmetric (wider to the temple), so straight ahead is nearer the
	// nose: the left eye's is +, the right eye's -. The image must be drawn so, or the eyes do not meet
	float eyeCentreX[2] = { 0.0f, 0.0f };
};
