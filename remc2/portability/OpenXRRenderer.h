#pragma once
#ifndef OPENXRRENDERER_H
#define OPENXRRENDERER_H

#include "VulkanPolygonRenderer.h"

#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

// The Vulkan renderer for a VR headset, through OpenXR (any runtime: SteamVR, Meta, Monado, a simulator).
//
// Each frame it waits for the headset, locates the head and sends its orientation with E_VR_HEAD_POSE: the game's VR
// code (Vr.cpp) turns it into the original head tracker's angles, which steer the camera as the original's tracker did.
// While the headset tracks, GameRenderHW draws the two eyes with the original VR mode's camera code and sends them as a
// 1 x 2 grid. Those two cells are drawn into a side by side stereo image (each eye the whole frame: its view and the HUD
// at the game resolution), copied (scaled) into the headset's left and right images, and shown in the window as a mirror.
// Every other frame (the menus, briefings, the map: one view) goes the same way into a flat screen floating in front of
// the player (a quad layer), so the headset always shows the game rather than the runtime's own home or dashboard.
//
// Without an OpenXR runtime or a headset it renders to the window as the base renderer does.
class OpenXRRenderer : public VulkanPolygonRenderer
{
public:
	OpenXRRenderer(std::string shaderFolder, bool enableValidation = false);
	~OpenXRRenderer() override;

	bool Init(SDL_Window* window, int windowWidth, int windowHeight) override;
	void Shutdown() override;

protected:
	std::vector<std::string> GetRequiredInstanceExtensions() override;
	std::vector<std::string> GetRequiredDeviceExtensions() override;
	VkPhysicalDevice GetRequiredPhysicalDevice(VkInstance instance) override;
	bool CreateExtraResources() override;
	void OnBeginFrame() override;
	void OnFrameSubmitted() override;
	FrameCells GetFrameCells(const SDL_Rect& srcRect, const SDL_Rect& destRect) override;
	void BeginCellsRenderPass(VkCommandBuffer commandBuffer, const VkClearValue& clearColor) override;
	void EndCellsRenderPass(VkCommandBuffer commandBuffer, const VkClearValue& clearColor) override;

private:
	static constexpr uint32_t kEyeCount = 2;

	// One headset image per eye
	struct EyeSwapchain
	{
		XrSwapchain handle = XR_NULL_HANDLE;
		int32_t width = 0;
		int32_t height = 0;
		std::vector<XrSwapchainImageVulkanKHR> images;
		uint32_t acquiredIndex = 0;
		bool acquired = false;
	};

	// OpenXR
	bool CreateXrInstance();
	bool CreateXrSession();
	bool CreateEyeSwapchains();
	bool CreateSwapchain(EyeSwapchain& swapchain, int64_t format);
	void DestroyXrSession();
	void DestroyXrInstance();
	void PollXrEvents();
	void EndXrFrame(bool submit);
	void SendHeadPose(bool active, const XrPosef* pose = nullptr);
	std::vector<std::string> GetXrVulkanExtensions(const char* functionName);

	// The stereo image the eyes' cells are drawn into (or, for a flat frame, its one cell)
	bool CreateStereoRenderPass();
	bool EnsureStereoTarget(uint32_t width, uint32_t height);
	void CopyEyesToHeadset(VkCommandBuffer commandBuffer);
	void CopyScreenToHeadset(VkCommandBuffer commandBuffer);
	void DrawMirror(VkCommandBuffer commandBuffer);

	XrInstance m_xrInstance = XR_NULL_HANDLE;
	XrSystemId m_systemId = XR_NULL_SYSTEM_ID;
	XrSession m_session = XR_NULL_HANDLE;
	XrSpace m_localSpace = XR_NULL_HANDLE;   // where the head was when the session started: the game's straight ahead
	XrSpace m_viewSpace = XR_NULL_HANDLE;    // the head
	XrSessionState m_sessionState = XR_SESSION_STATE_UNKNOWN;
	bool m_sessionRunning = false;
	bool m_xrFrameBegun = false;
	XrFrameState m_frameState{ XR_TYPE_FRAME_STATE };
	XrView m_views[kEyeCount]{};           // located this frame: the game draws them next
	XrView m_renderedViews[kEyeCount]{};   // located the frame before: the ones the eyes being drawn were made for
	bool m_viewsValid = false;
	bool m_renderedViewsValid = false;
	EyeSwapchain m_eyes[kEyeCount];
	EyeSwapchain m_screen;   // the flat screen's image (menus and the other one-view frames)
	XrPosef m_headPose{};      // the head this frame (in m_localSpace)
	bool m_headPoseValid = false;
	XrPosef m_screenPose{};    // where the flat screen floats: in front of the head when the flat frames began
	bool m_screenPlaced = false;
	bool m_headPoseActive = false;

	VkRenderPass m_stereoRenderPass = VK_NULL_HANDLE;
	ColourTarget m_stereoTarget;
	VkExtent2D m_stereoExtent{ 0, 0 };

	// This frame
	bool m_frameStereo = false;    // the game sent the eyes (1 x 2) and they go into the stereo image
	bool m_frameScreen = false;    // the game sent one view (a menu...): it goes into the stereo image, then the flat screen
	bool m_eyesCopied = false;     // the eyes are in the headset's images
	bool m_screenCopied = false;   // the frame is in the flat screen's image
	float m_screenAspect = 4.0f / 3.0f;   // the flat frame's width / height: the screen's shape
	FrameCells m_frameCells;
	SDL_Rect m_frameSrcRect{ 0, 0, 0, 0 };
	SDL_Rect m_frameDestRect{ 0, 0, 0, 0 };
};

#endif //OPENXRRENDERER_H
