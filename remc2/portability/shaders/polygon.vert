#version 450

layout(location = 0) in vec2 inPos;        // screen-space pixel coords
layout(location = 1) in vec2 inUV;         // texel-space texture coords
layout(location = 2) in float inBrightness;// 0-1

layout(location = 0) out vec2 fragUV;
layout(location = 1) out float fragBrightness;

layout(push_constant) uniform PushConstants
{
	vec2 screenSize;
} pc;

void main()
{
	// Screen pixel space -> NDC. No projection matrix needed since the
	// existing software projection code already produced screen coords.
	vec2 ndc = (inPos / pc.screenSize) * 2.0 - 1.0;
	gl_Position = vec4(ndc, 0.0, 1.0);

	fragUV = inUV;
	fragBrightness = inBrightness;
}
