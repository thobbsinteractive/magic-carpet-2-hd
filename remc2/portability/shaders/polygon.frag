#version 450

layout(location = 0) in vec2  fragUV;     // texel coordinates (terrain tile: 0..32, sprite: frame rect in texels)
layout(location = 1) in float fragShade;  // colour LUT row / 32  (1.0 = fully lit)

layout(set = 0, binding = 0) uniform sampler2D  uPalette;    // 256x1 RGBA8
layout(set = 0, binding = 1) uniform usampler2D uIndexTex;   // R8_UINT, terrain tiles or stacked sprite frames
// 256x256 R8_UINT, texel(x = low byte, y = high byte) = ColourLookupTable_F6EE0[COLOUR_BLEND_LOOKUP_OFFSET + (y << 8 | x)]
layout(set = 0, binding = 2) uniform usampler2D uBlendLut;

layout(push_constant) uniform PC
{
	uint  mode;    // 255 = terrain, 0..8 = original visibilityIdx
	uint  tint;    // dword0x07 low byte, used by modes 4/5
	float alpha;   // constant blend factor for the destination-dependent modes (2, 3, 6, 7)
} pc;

layout(location = 0) out vec4 outColor;

const uint MODE_TERRAIN = 255u;

vec3 palette(uint i)
{
	return texelFetch(uPalette, ivec2(int(i), 0), 0).rgb;
}

void main()
{
	ivec2 size  = textureSize(uIndexTex, 0);
	ivec2 texel = clamp(ivec2(floor(fragUV)), ivec2(0), size - 1);
	uint  idx   = texelFetch(uIndexTex, texel, 0).r;

	float shade = clamp(fragShade, 0.0, 1.0);

	// ---- terrain: unchanged ----
	if (pc.mode == MODE_TERRAIN)
	{
		outColor = vec4(palette(idx) * shade, 1.0);
		return;
	}

	// ---- sprites: index 0 is transparent in every mode ----
	if (idx == 0u)
		discard;

	switch (pc.mode)
	{
	case 0u:   // opaque, no shading (only used when fully lit)
		outColor = vec4(palette(idx), 1.0);
		break;

	case 1u:   // shaded: ColourLookupTable[row << 8 | src]
		outColor = vec4(palette(idx) * shade, 1.0);
		break;

	case 2u:   // blend with destination: LUT[BLEND + (src << 8 | dst)]
	case 3u:   // blend with destination: LUT[BLEND + (dst << 8 | src)]
		outColor = vec4(palette(idx), pc.alpha);
		break;

	case 4u:   // blend with tint: LUT[BLEND + (tint << 8 | src)], no destination needed
	{
		uint o = texelFetch(uBlendLut, ivec2(int(idx), int(pc.tint & 0xFFu)), 0).r;
		outColor = vec4(palette(o), 1.0);
		break;
	}

	case 5u:   // blend with tint: LUT[BLEND + (src << 8 | tint)], no destination needed
	{
		uint o = texelFetch(uBlendLut, ivec2(int(pc.tint & 0xFFu), int(idx)), 0).r;
		outColor = vec4(palette(o), 1.0);
		break;
	}

	case 6u:   // blend with destination, then shade
	case 7u:
		outColor = vec4(palette(idx) * shade, pc.alpha);
		break;

	case 8u:   // shadow: darken destination wherever the sprite is non-zero
		outColor = vec4(0.0, 0.0, 0.0, clamp(abs(1.0 - fragShade), 0.0, 1.0));
		break;

	default:
		outColor = vec4(palette(idx) * shade, 1.0);
		break;
	}
}