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

// A single convex, screen-space polygon ready for rasterization.
// Vertices must be wound consistently (CW or CCW - see
// VulkanPolygonRenderer::SetFrontFace) and are triangulated as a fan
// (0,1,2 / 0,2,3 / ...), so any convex N-gon works without pre-splitting.
struct RenderPolygon
{
	std::vector<HWVertex> Vertices;	// 3+ vertices, fan order
	uint32_t TextureId = 0;					// id returned by VulkanPolygonRenderer::UploadTexture
};
