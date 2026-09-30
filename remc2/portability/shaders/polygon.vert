#version 450

layout(location = 0) in vec3  inPosW;    // NDC x, y, and w
layout(location = 1) in vec2  inUV;      // texel coordinates, 0..32
layout(location = 2) in float inShade;   // 0..1

layout(location = 0) out vec2  fragUV;
layout(location = 1) out float fragShade;

void main()
{
    gl_Position = vec4(inPosW.xy * inPosW.z, 0.0, inPosW.z);
    fragUV    = inUV;
    fragShade = inShade;
}