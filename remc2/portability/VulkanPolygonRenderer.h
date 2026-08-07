#pragma once
#ifndef VULKANPOLYGONRENDER_H
#define VULKANPOLYGONRENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <vma/vk_mem_alloc.h>
#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <unordered_map>

#include "RenderPolygon.h"

struct SDL_Window;

// Minimal Vulkan renderer that takes a per-frame list of RenderPolygon
// (screen-space, palette-textured, Gouraud-shaded) and rasterizes them.
//
// Usage per frame:
//     renderer.BeginFrame();
//     renderer.DrawPolygons(polygons);
//     renderer.EndFrame();
//
// Textures are 8-bit palette-indexed (one byte per texel = palette index,
// same convention as m_gamePalletisedSurface in the SDL path). Upload once
// with UploadTexture() and reuse the returned TextureId across frames.
// Call SetPalette() whenever the 256-color palette changes.
class VulkanPolygonRenderer
{
public:
	VulkanPolygonRenderer() = default;
	~VulkanPolygonRenderer();

	VulkanPolygonRenderer(const VulkanPolygonRenderer&) = delete;
	VulkanPolygonRenderer& operator=(const VulkanPolygonRenderer&) = delete;

	// Creates instance/device/swapchain/pipeline. windowWidth/Height should
	// match the SDL window's drawable size in pixels.
	bool Init(SDL_Window* window, int windowWidth, int windowHeight);

	void Shutdown();

	// Call when the window is resized (recreates swapchain + framebuffers).
	void Resize(int windowWidth, int windowHeight);

	// rgb triplets, 256 entries (768 bytes), 0-255 per channel.
	// (If you're feeding it straight from VGA_Get_Palette()/tempPalettebuffer,
	// multiply by 4 first - those are 6-bit VGA DAC values, see VGA_Set_Palette.)
	void SetPalette(const uint8_t* rgb768);

	// Uploads an 8-bit indexed texture (width*height bytes, one palette
	// index per texel) and returns a TextureId to use in RenderPolygon.
	uint32_t UploadTexture(const uint8_t* indexedPixels, uint32_t width, uint32_t height);

	// Frees a previously uploaded texture's GPU resources.
	void FreeTexture(uint32_t textureId);

	// If your projection code winds vertices clockwise in screen space,
	// leave this at default; flip if you get back-face culling artifacts.
	void SetFrontFace(VkFrontFace frontFace);
	void SetCullMode(VkCullModeFlags cullMode); // default: VK_CULL_MODE_NONE

	bool BeginFrame(SDL_Surface* surface, SDL_Rect srcRect, SDL_Rect destRect, const std::vector<RenderPolygon>& polygons);
	void UploadOverlaySurface(SDL_Surface* surface, SDL_Rect srcRect, SDL_Rect destRect, VkCommandBuffer commandBuffer);
	void DrawOverlay(VkCommandBuffer commandBuffer);
	void DrawPolygons(const std::vector<RenderPolygon>& polygons);
	void DrawPolygonsWireframe(const std::vector<RenderPolygon>& polygons);
	void EndFrame();

private:
	struct Vertex
	{
		float x, y;			// screen-space pixel coords
		float u, v;			// texel-space coords (divided by texture size in shader via push constant... see .cpp)
		float brightness;	// 0-1
	};

	struct Texture
	{
		VkImage image = VK_NULL_HANDLE;
		VkImageView view = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;
		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	struct FrameData
	{
		VkCommandPool commandPool = VK_NULL_HANDLE;
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		VkSemaphore imageAvailable = VK_NULL_HANDLE;
		VkSemaphore renderFinished = VK_NULL_HANDLE;
		VkFence inFlight = VK_NULL_HANDLE;

		// Host-visible, persistently-mapped per-frame vertex/index buffers.
		VkBuffer vertexBuffer = VK_NULL_HANDLE;
		VmaAllocation vertexAllocation = VK_NULL_HANDLE;
		void* vertexMapped = nullptr;
		VkDeviceSize vertexBufferSize = 0;

		VkBuffer indexBuffer = VK_NULL_HANDLE;
		VmaAllocation indexAllocation = VK_NULL_HANDLE;
		void* indexMapped = nullptr;
		VkDeviceSize indexBufferSize = 0;
	};

	bool CreateSwapchain(int width, int height);
	void DestroySwapchain();
	bool CreateRenderPass();
	bool CreatePipeline();
	bool CreateWireframePipeline();
	bool CreateDescriptorResources();
	bool CreatePaletteTexture();
	bool CreateFrameData();
	bool CreateOverlayResources(uint32_t width, uint32_t height);
	bool CreateOverlayPipeline();
	void DestroyFrameData();
	void EnsureFrameBufferCapacity(FrameData& frame, size_t vertexBytes, size_t indexBytes);
	VkDescriptorSet AllocateTextureDescriptorSet(VkImageView view);

	VkImage m_overlayImage = VK_NULL_HANDLE;
	VmaAllocation m_overlayImageAlloc = VK_NULL_HANDLE;
	VkImageView m_overlayImageView = VK_NULL_HANDLE;
	VkSampler m_overlaySampler = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_overlayDescriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorSet m_overlayDescriptorSet = VK_NULL_HANDLE;
	VkPipelineLayout m_overlayPipelineLayout = VK_NULL_HANDLE;
	VkPipeline m_overlayPipeline = VK_NULL_HANDLE;

	VkBuffer m_overlayStagingBuffer = VK_NULL_HANDLE;
	VmaAllocation m_overlayStagingAlloc = VK_NULL_HANDLE;
	void* m_overlayStagingMapped = nullptr;
	uint32_t m_overlayWidth = 0;
	uint32_t m_overlayHeight = 0;

	vkb::Instance m_vkbInstance{};
	vkb::Device m_vkbDevice{};
	vkb::Swapchain m_vkbSwapchain{};

	VkInstance m_instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;
	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	uint32_t m_graphicsQueueFamily = 0;

	VmaAllocator m_allocator = VK_NULL_HANDLE;

	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
	VkFormat m_swapchainFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D m_swapchainExtent{};
	std::vector<VkImage> m_swapchainImages;
	std::vector<VkImageView> m_swapchainImageViews;
	std::vector<VkFramebuffer> m_framebuffers;

	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
	VkPipelineLayout m_wireframePipelineLayout = VK_NULL_HANDLE;
	VkPipeline m_pipeline = VK_NULL_HANDLE;
	VkPipeline m_wireframePipeline = VK_NULL_HANDLE;

	VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
	VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
	VkSampler m_nearestSampler = VK_NULL_HANDLE;

	// Palette: 256x1 RGBA8 texture, binding 0 in the descriptor set.
	VkImage m_paletteImage = VK_NULL_HANDLE;
	VkImageView m_paletteView = VK_NULL_HANDLE;
	VmaAllocation m_paletteAllocation = VK_NULL_HANDLE;
	VkBuffer m_paletteStagingBuffer = VK_NULL_HANDLE;
	VmaAllocation m_paletteStagingAllocation = VK_NULL_HANDLE;
	void* m_paletteStagingMapped = nullptr;

	std::unordered_map<uint32_t, Texture> m_textures;
	uint32_t m_nextTextureId = 1;

	static constexpr int kFramesInFlight = 2;
	FrameData m_frames[kFramesInFlight];
	size_t m_currentFrame = 0;
	uint32_t m_currentImageIndex = 0;

	VkFrontFace m_frontFace = VK_FRONT_FACE_CLOCKWISE;
	VkCullModeFlags m_cullMode = VK_CULL_MODE_NONE;

	int m_windowWidth = 0;
	int m_windowHeight = 0;
	SDL_Window* m_window = nullptr;
	SDL_Surface* m_overlayScaledSurface = nullptr;

	bool m_initialized = false;
};

#endif //VULKANPOLYGONRENDER_H 