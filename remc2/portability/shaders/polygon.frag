#version 450

layout(location = 0) in vec2  fragUV;
layout(location = 1) in float fragShade;

layout(set = 0, binding = 0) uniform sampler2D  uPalette;    // 256x1 RGBA8
layout(set = 0, binding = 1) uniform usampler2D uIndexTex;   // 32x32 R8_UINT

layout(location = 0) out vec4 outColor;

void main()
{
	ivec2 size  = textureSize(uIndexTex, 0); // 256 x 608
	ivec2 texel = clamp(ivec2(floor(fragUV)), ivec2(0), size - 1);
	uint idx = texelFetch(uIndexTex, texel, 0).r;
    vec3 color = texelFetch(uPalette, ivec2(int(idx), 0), 0).rgb;

    float shade = clamp(fragShade, 0.0, 1.0);
    outColor = vec4(color * shade, 1.0);
}