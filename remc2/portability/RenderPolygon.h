#pragma once
#include <vector>
#include <cstdint>
#include "../engine/ProjectionVertex.h"

struct HWVertex
{
	float X = 0.0f, Y = 0.0f, W = 1.0f;   // NDC + w
	float U = 0.0f, V = 0.0f;             // 0..1
	float Shade = 0.0f;                   // 0..1
	uint32_t Layer = 0;                   // texture array layer = textIndex_41
};

struct RenderPolygon
{
	std::vector<HWVertex> Vertices;   // 3+ vertices, fan order
	uint32_t TextureId = 0;           // id returned by VulkanPolygonRenderer::UploadTexture
	uint8_t  SpriteMode = 0xFF;       // 0xFF = terrain; 0..8 = original visibilityIdx
	uint8_t  Tint = 0;                // dword0x07 low byte (modes 4/5)
};
