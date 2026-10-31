#version 450

layout(location = 0) in vec2  fragUV;
layout(location = 1) in float fragShade;  // lighting row / 32 (terrain: 0..1.97, sprites: 0..1.25); fog is already in the row

layout(set = 0, binding = 0) uniform sampler2D  uPalette;    // 256x1 RGBA8
layout(set = 0, binding = 1) uniform usampler2D uIndexTex;   // R8_UINT, terrain atlas or stacked sprite frames
layout(set = 0, binding = 2) uniform usampler2D uBlendLut;   // 256x256, tint modes 4/5
layout(set = 0, binding = 3) uniform usampler2D uShadeLut;   // 256x64, x = texel, y = lighting row -> palette index

layout(push_constant) uniform PC
{
	uint  mode;      // 255 = terrain, 26 = water terrain, 0..8 = original visibilityIdx
	uint  tint;      // dword0x07 low byte, used by modes 4/5
	float alpha;     // per-polygon alpha
	uint  fogIndex;  // no longer used by the shader; kept so the 16-byte push constant layout is unchanged
} pc;

layout(location = 0) out vec4 outColor;

const uint  MODE_TERRAIN = 255u;
const uint  MODE_SKY = 254u;
const uint  MODE_WATER   = 26u;
const uint  WATER_TEXELS = 12u;

const int   TERRAIN_TILES_WIDE = 8;      // kTerrainAtlasTilesWide
const float TERRAIN_LOD_BIAS   = 0.0;    // > 0 blurs sooner, < 0 keeps detail longer (and shimmers)

const float WATER_GAIN   = 2.0;
const vec3  WATER_LIFT   = vec3(0.0, 0.04, 0.06);

vec3 palette(uint i)
{
	return texelFetch(uPalette, ivec2(int(i), 0), 0).rgb;
}

// The software renderer's lit[row << 8 | texel], with the two nearest rows mixed so the steps do not band
vec3 litColour(uint idx, float row)
{
	row = clamp(row, 0.0, 63.0);
	int  r0 = int(floor(row));
	int  r1 = min(r0 + 1, 63);
	vec3 a = palette(texelFetch(uShadeLut, ivec2(int(idx), r0), 0).r);
	vec3 b = palette(texelFetch(uShadeLut, ivec2(int(idx), r1), 0).r);
	return mix(a, b, fract(row));
}

// One terrain texel at a mip level, kept inside the tile uv is in so tiles don't bleed together
uint terrainTexel(vec2 uv, int level, int tileSize)
{
	int   levelTile = max(tileSize >> level, 1);
	ivec2 tile      = ivec2(floor(uv / float(tileSize))) * levelTile;
	ivec2 texel     = ivec2(floor(uv / float(1 << level)));
	texel = clamp(texel, tile, tile + levelTile - 1);
	texel = clamp(texel, ivec2(0), textureSize(uIndexTex, level) - 1);
	return texelFetch(uIndexTex, texel, level).r;
}

void main()
{
	const bool isTerrain = (pc.mode == MODE_TERRAIN) || (pc.mode == MODE_WATER);
	const bool isSky     = (pc.mode == MODE_SKY);

	ivec2 size = textureSize(uIndexTex, 0);

	// ---- terrain and water ----
	// Palette indices can't be filtered, so pick the level from the footprint of the pixel
	// in atlas texels, look up nearest texels in the two closest levels and blend their lit
	// colours. Up close (lod 0) this is exactly the unmipped lookup.
	if (isTerrain)
	{
		vec2  dx       = dFdx(fragUV);
		vec2  dy       = dFdy(fragUV);
		float lod      = 0.5 * log2(max(max(dot(dx, dx), dot(dy, dy)), 1e-8)) + TERRAIN_LOD_BIAS;
		int   levels   = textureQueryLevels(uIndexTex);
		int   tileSize = size.x / TERRAIN_TILES_WIDE;
		lod = clamp(lod, 0.0, float(levels - 1));

		int   level = int(floor(lod));
		float blend = lod - float(level);
		float row   = fragShade * 32.0;

		uint  idx = terrainTexel(fragUV, level, tileSize);
		vec3  c   = litColour(idx, row);
		if (blend > 0.0)
			c = mix(c, litColour(terrainTexel(fragUV, level + 1, tileSize), row), blend);

		float alpha = 1.0;
		if (pc.mode == MODE_WATER && idx < WATER_TEXELS)
			alpha = pc.alpha;      // no gain, no lift; fog comes only from litColour

		outColor = vec4(c, alpha);
		return;
	}

	vec2  uv   = fragUV * vec2(size);
	ivec2 texel = ivec2(floor(uv));

	if (isSky)
		texel = ((texel % size) + size) % size;   // wrap, negatives included
	else
		texel = clamp(texel, ivec2(0), size - 1);

	uint  idx = texelFetch(uIndexTex, texel, 0).r;

	const float row = fragShade * 32.0;

	// ---- sky: tiles, opaque, no transparency ----
	if (isSky)
	{
		outColor = vec4(palette(idx), 1.0);
		return;
	}

	// ---- sprites: index 0 is transparent in every mode ----
	if (idx == 0u)
		discard;

	switch (pc.mode)
	{
	case 0u:   // texel as is
	case 2u:   // blended with the screen
	case 3u:
		outColor = vec4(palette(idx), pc.alpha);
		break;

	case 1u:   // lit
	case 6u:   // blended, then lit
	case 7u:
		outColor = vec4(litColour(idx, row), pc.alpha);
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

	case 8u:   // shadow: black, blended with the destination at pc.alpha
		outColor = vec4(0.0, 0.0, 0.0, pc.alpha);
		break;

	default:
		outColor = vec4(litColour(idx, row), pc.alpha);
		break;
	}
}