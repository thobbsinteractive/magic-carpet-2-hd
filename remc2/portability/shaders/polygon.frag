#version 450

// binding 0: 256x1 RGBA8 palette
layout(binding = 0) uniform sampler2D paletteTex;
// binding 1: 8-bit palette-index texture (one byte per texel)
layout(binding = 1) uniform usampler2D indexedTex;

layout(location = 0) in vec2 fragUV;
layout(location = 1) in float fragBrightness;

layout(location = 0) out vec4 outColor;

void main()
{
	// texelFetch always takes integer texel coordinates regardless of the
	// sampler's normalized/unnormalized setting, so this works whether or
	// not VK_TRUE unnormalizedCoordinates is set on the sampler.
	uint paletteIndex = texelFetch(indexedTex, ivec2(fragUV), 0).r;
	vec4 color = texelFetch(paletteTex, ivec2(int(paletteIndex), 0), 0);

	// Gouraud-interpolated per-vertex light level, applied as a simple
	// linear multiply. Swap this for a shade-table lookup (light level x
	// palette index -> shaded index) if you want to match the original
	// engine's exact banded lighting instead.
	outColor = vec4(color.rgb * fragBrightness, color.a);
}
