#pragma once
#include <vector>
#include <cstdint>
#include "../engine/ProjectionVertex.h"

// A single convex, screen-space polygon ready for rasterization.
// Vertices must be wound consistently (CW or CCW - see
// VulkanPolygonRenderer::SetFrontFace) and are triangulated as a fan
// (0,1,2 / 0,2,3 / ...), so any convex N-gon works without pre-splitting.
struct RenderPolygon
{
	std::vector<ProjectionVertex> Vertices;	// 3+ vertices, fan order
	uint32_t TextureId = 0;					// id returned by VulkanPolygonRenderer::UploadTexture
};
