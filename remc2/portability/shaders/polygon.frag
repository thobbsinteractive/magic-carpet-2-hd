#version 450

layout(location = 0) in vec2  fragUV;
layout(location = 1) in float fragShade;

layout(set = 0, binding = 0) uniform sampler2D  uPalette;    // 256x1 RGBA8
layout(set = 0, binding = 1) uniform usampler2D uIndexTex;   // 32x32 R8_UINT

layout(location = 0) out vec4 outColor;

void main()
{
    // texelFetch bypasses the sampler, which is unnormalized and can't
    // be used with implicit-LOD texture() calls anyway.
    
ivec2 texel = clamp(ivec2(floor(fragUV)), ivec2(0), ivec2(31));
outColor = vec4(vec2(texel) / 31.0, 0.0, 1.0);
}