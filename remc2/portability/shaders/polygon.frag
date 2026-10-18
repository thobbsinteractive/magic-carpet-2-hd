#version 450

layout(location = 0) in vec2  fragUV;     // texel coordinates (terrain tile: 0..32, sprite: frame rect in texels)
layout(location = 1) in float fragShade;  // terrain: brightness / neutral (may exceed 1.0), sprites: colour LUT row / 32

layout(set = 0, binding = 0) uniform sampler2D  uPalette;    // 256x1 RGBA8
layout(set = 0, binding = 1) uniform usampler2D uIndexTex;   // R8_UINT, terrain atlas or stacked sprite frames
// 256x256 R8_UINT, texel(x = low byte, y = high byte) = ColourLookupTable_F6EE0[COLOUR_BLEND_LOOKUP_OFFSET + (y << 8 | x)]
layout(set = 0, binding = 2) uniform usampler2D uBlendLut;

layout(push_constant) uniform PC
{
	uint  mode;    // 255 = terrain, 0..8 = original visibilityIdx
	uint  tint;    // dword0x07 low byte, used by modes 4/5
	float alpha;   // per-polygon alpha (1.0 = opaque, 0.5 = shadow / transparent sprite)
} pc;

layout(location = 0) out vec4 outColor;

const uint MODE_TERRAIN = 255u;
const uint MODE_WATER   = 26u;
const uint WATER_TEXELS = 12u;   // palette indices below this are the water surface

vec3 palette(uint i)
{
	return texelFetch(uPalette, ivec2(int(i), 0), 0).rgb;
}

void main()
{
	const bool isTerrain = (pc.mode == MODE_TERRAIN) || (pc.mode == MODE_WATER);

	ivec2 size  = textureSize(uIndexTex, 0);
	vec2  uv    = isTerrain ? fragUV : fragUV * vec2(size);
	ivec2 texel = clamp(ivec2(floor(uv)), ivec2(0), size - 1);
	uint  idx   = texelFetch(uIndexTex, texel, 0).r;

	if (isTerrain)
	{
		float shade = clamp(fragShade, 0.0, 2.0);
		vec3  rgb   = min(palette(idx) * shade, vec3(1.0));
		float a     = (pc.mode == MODE_WATER && idx < WATER_TEXELS) ? pc.alpha : 1.0;
		outColor = vec4(rgb, a);
		return;
	}

	// ---- sprites: index 0 is transparent in every mode ----
	if (idx == 0u)
		discard;

	float shade = clamp(fragShade, 0.0, 1.0);

	switch (pc.mode)
	{
	case 0u:   // opaque, no shading (fully lit)
		outColor = vec4(palette(idx), pc.alpha);
		break;

	case 1u:   // shaded
		outColor = vec4(palette(idx) * shade, pc.alpha);
		break;

	case 2u:   // transparent (was: blend with destination)
	case 3u:
		outColor = vec4(palette(idx), pc.alpha);
		break;

	case 4u:   // tint blend: LUT[tint << 8 | src]
	{
		uint o = texelFetch(uBlendLut, ivec2(int(idx), int(pc.tint & 0xFFu)), 0).r;
		outColor = vec4(palette(o), pc.alpha);
		break;
	}

	case 5u:   // tint blend: LUT[src << 8 | tint]
	{
		uint o = texelFetch(uBlendLut, ivec2(int(pc.tint & 0xFFu), int(idx)), 0).r;
		outColor = vec4(palette(o), pc.alpha);
		break;
	}

	case 6u:   // transparent, then shaded
	case 7u:
		outColor = vec4(palette(idx) * shade, pc.alpha);
		break;

	case 8u:   // shadow: black, blended with the destination at pc.alpha
		outColor = vec4(0.0, 0.0, 0.0, pc.alpha);
		break;

	default:
		outColor = vec4(palette(idx) * shade, pc.alpha);
		break;
	}
}