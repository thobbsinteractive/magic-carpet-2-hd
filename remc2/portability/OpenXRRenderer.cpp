#include "OpenXRRenderer.h"
#include "OpenXRDevice.h"
#include "../shared/VrHeadPose.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <sstream>

namespace
{
	struct Vec3
	{
		float x, y, z;
	};

	// v rotated by the unit quaternion q
	Vec3 Rotate(const XrQuaternionf& q, const Vec3& v)
	{
		// t = 2 * cross(q.xyz, v); v' = v + q.w * t + cross(q.xyz, t)
		const Vec3 t{ 2.0f * (q.y * v.z - q.z * v.y), 2.0f * (q.z * v.x - q.x * v.z), 2.0f * (q.x * v.y - q.y * v.x) };
		return Vec3{
			v.x + q.w * t.x + (q.y * t.z - q.z * t.y),
			v.y + q.w * t.y + (q.z * t.x - q.x * t.z),
			v.z + q.w * t.z + (q.x * t.y - q.y * t.x) };
	}

	template <typename T>
	bool GetXrFunction(XrInstance instance, const char* name, T& function)
	{
		return XR_SUCCEEDED(xrGetInstanceProcAddr(instance, name, reinterpret_cast<PFN_xrVoidFunction*>(&function))) && function;
	}

	// The flat screen (menus...): its image, and its size and distance: about 60 degrees wide, in front of the head
	// (EndXrFrame places it)
	constexpr uint32_t kScreenImageWidth = 1920;
	constexpr uint32_t kScreenImageHeight = 1440;
	constexpr float kScreenDistance = 2.0f;   // metres
	constexpr float kScreenWidth = 2.4f;      // metres

	void ImageBarrier(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
		VkPipelineStageFlags srcStage, VkAccessFlags srcAccess, VkPipelineStageFlags dstStage, VkAccessFlags dstAccess)
	{
		VkImageMemoryBarrier imageBarrier{};
		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageBarrier.oldLayout = oldLayout;
		imageBarrier.newLayout = newLayout;
		imageBarrier.srcAccessMask = srcAccess;
		imageBarrier.dstAccessMask = dstAccess;
		imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageBarrier.image = image;
		imageBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier);
	}

	// src (the stereo image, readable) scaled into an OpenXR swapchain image (handed over as a colour attachment)
	void BlitIntoXrImage(VkCommandBuffer commandBuffer, VkImage src, const SDL_Rect& srcArea, VkImage dst, int32_t dstWidth, int32_t dstHeight)
	{
		ImageBarrier(commandBuffer, dst, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);

		VkImageBlit blit{};
		blit.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		blit.srcOffsets[0] = { srcArea.x, srcArea.y, 0 };
		blit.srcOffsets[1] = { srcArea.x + srcArea.w, srcArea.y + srcArea.h, 1 };
		blit.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { dstWidth, dstHeight, 1 };
		vkCmdBlitImage(commandBuffer, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

		// OpenXR takes it back as a colour attachment
		ImageBarrier(commandBuffer, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
	}
}

// A short-lived instance (no graphics needed) to ask for the headset and its eyes' size
OpenXREyeSize QueryOpenXREyeSize()
{
	OpenXREyeSize result;
	uint32_t extensionCount = 0;
	if (XR_FAILED(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr)))
		return result;   // no runtime

	XrInstanceCreateInfo createInfo{ XR_TYPE_INSTANCE_CREATE_INFO };
	std::strncpy(createInfo.applicationInfo.applicationName, "remc2", XR_MAX_APPLICATION_NAME_SIZE - 1);
	createInfo.applicationInfo.applicationVersion = 1;
	std::strncpy(createInfo.applicationInfo.engineName, "remc2", XR_MAX_ENGINE_NAME_SIZE - 1);
	createInfo.applicationInfo.engineVersion = 1;
	createInfo.applicationInfo.apiVersion = XR_MAKE_VERSION(1, 0, 0);
	XrInstance instance = XR_NULL_HANDLE;
	if (XR_FAILED(xrCreateInstance(&createInfo, &instance)))
		return result;

	XrSystemGetInfo systemInfo{ XR_TYPE_SYSTEM_GET_INFO };
	systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	XrSystemId systemId = XR_NULL_SYSTEM_ID;
	uint32_t viewCount = 0;
	if (XR_SUCCEEDED(xrGetSystem(instance, &systemInfo, &systemId)) &&
		XR_SUCCEEDED(xrEnumerateViewConfigurationViews(instance, systemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 0, &viewCount, nullptr)) &&
		viewCount > 0)
	{
		std::vector<XrViewConfigurationView> views(viewCount, XrViewConfigurationView{ XR_TYPE_VIEW_CONFIGURATION_VIEW });
		if (XR_SUCCEEDED(xrEnumerateViewConfigurationViews(instance, systemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
			viewCount, &viewCount, views.data())) && views[0].recommendedImageRectWidth > 0 && views[0].recommendedImageRectHeight > 0)
		{
			result.found = true;
			result.width = (int)views[0].recommendedImageRectWidth;
			result.height = (int)views[0].recommendedImageRectHeight;
		}
	}
	xrDestroyInstance(instance);
	return result;
}

OpenXRRenderer::OpenXRRenderer(std::string shaderFolder, bool enableValidation)
	: VulkanPolygonRenderer(shaderFolder, enableValidation)
{
}

OpenXRRenderer::~OpenXRRenderer()
{
	// The base destructor can't reach this class's resources (it would call its own Shutdown), so free them here
	Shutdown();
	DestroyXrSession();   // Init may have stopped after creating it
	DestroyXrInstance();
}

// ── Set up ───────────────────────────────────────────────────────────────────────────────────────────────────
bool OpenXRRenderer::Init(SDL_Window* window, int windowWidth, int windowHeight)
{
	// The OpenXR instance first: it says which Vulkan extensions and GPU the headset needs
	if (!CreateXrInstance())
		Logger->warn("OpenXR: no headset, rendering to the window only");
	return VulkanPolygonRenderer::Init(window, windowWidth, windowHeight);
}

bool OpenXRRenderer::CreateXrInstance()
{
	// Is there a runtime (and does it do Vulkan)?
	uint32_t extensionCount = 0;
	if (XR_FAILED(xrEnumerateInstanceExtensionProperties(nullptr, 0, &extensionCount, nullptr)))
	{
		Logger->warn("OpenXR: no runtime installed or active");
		return false;
	}
	std::vector<XrExtensionProperties> extensions(extensionCount, XrExtensionProperties{ XR_TYPE_EXTENSION_PROPERTIES });
	xrEnumerateInstanceExtensionProperties(nullptr, extensionCount, &extensionCount, extensions.data());
	const bool hasVulkan = std::any_of(extensions.begin(), extensions.end(), [](const XrExtensionProperties& extension) {
		return std::strcmp(extension.extensionName, XR_KHR_VULKAN_ENABLE_EXTENSION_NAME) == 0;
		});
	if (!hasVulkan)
	{
		Logger->warn("OpenXR: the runtime does not support Vulkan ({})", XR_KHR_VULKAN_ENABLE_EXTENSION_NAME);
		return false;
	}

	const char* enabledExtensions[] = { XR_KHR_VULKAN_ENABLE_EXTENSION_NAME };
	XrInstanceCreateInfo createInfo{ XR_TYPE_INSTANCE_CREATE_INFO };
	std::strncpy(createInfo.applicationInfo.applicationName, "remc2", XR_MAX_APPLICATION_NAME_SIZE - 1);
	createInfo.applicationInfo.applicationVersion = 1;
	std::strncpy(createInfo.applicationInfo.engineName, "remc2", XR_MAX_ENGINE_NAME_SIZE - 1);
	createInfo.applicationInfo.engineVersion = 1;
	createInfo.applicationInfo.apiVersion = XR_MAKE_VERSION(1, 0, 0);   // 1.0: every runtime takes it
	createInfo.enabledExtensionCount = 1;
	createInfo.enabledExtensionNames = enabledExtensions;
	if (XR_FAILED(xrCreateInstance(&createInfo, &m_xrInstance)))
	{
		Logger->warn("OpenXR: could not create an instance");
		m_xrInstance = XR_NULL_HANDLE;
		return false;
	}

	XrInstanceProperties properties{ XR_TYPE_INSTANCE_PROPERTIES };
	if (XR_SUCCEEDED(xrGetInstanceProperties(m_xrInstance, &properties)))
		Logger->info("OpenXR: runtime {}", properties.runtimeName);

	XrSystemGetInfo systemInfo{ XR_TYPE_SYSTEM_GET_INFO };
	systemInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	if (XR_FAILED(xrGetSystem(m_xrInstance, &systemInfo, &m_systemId)))
	{
		Logger->warn("OpenXR: no headset connected");
		DestroyXrInstance();
		return false;
	}

	// The runtime must be asked for its Vulkan requirements before a session is created
	PFN_xrGetVulkanGraphicsRequirementsKHR getRequirements = nullptr;
	XrGraphicsRequirementsVulkanKHR requirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR };
	if (!GetXrFunction(m_xrInstance, "xrGetVulkanGraphicsRequirementsKHR", getRequirements) ||
		XR_FAILED(getRequirements(m_xrInstance, m_systemId, &requirements)))
	{
		Logger->warn("OpenXR: could not get the headset's Vulkan requirements");
		DestroyXrInstance();
		return false;
	}

	// Each eye's recommended image size
	uint32_t viewCount = 0;
	xrEnumerateViewConfigurationViews(m_xrInstance, m_systemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, 0, &viewCount, nullptr);
	std::vector<XrViewConfigurationView> views(viewCount, XrViewConfigurationView{ XR_TYPE_VIEW_CONFIGURATION_VIEW });
	if (viewCount < kEyeCount ||
		XR_FAILED(xrEnumerateViewConfigurationViews(m_xrInstance, m_systemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
			viewCount, &viewCount, views.data())))
	{
		Logger->warn("OpenXR: the headset has no stereo view");
		DestroyXrInstance();
		return false;
	}
	for (uint32_t eye = 0; eye < kEyeCount; eye++)
	{
		m_eyes[eye].width = (int32_t)views[eye].recommendedImageRectWidth;
		m_eyes[eye].height = (int32_t)views[eye].recommendedImageRectHeight;
	}

	XrSystemProperties systemProperties{ XR_TYPE_SYSTEM_PROPERTIES };
	if (XR_SUCCEEDED(xrGetSystemProperties(m_xrInstance, m_systemId, &systemProperties)))
		Logger->info("OpenXR: headset {}, {}x{} per eye", systemProperties.systemName, m_eyes[0].width, m_eyes[0].height);
	return true;
}

// The runtime lists its Vulkan extensions as one space separated string
std::vector<std::string> OpenXRRenderer::GetXrVulkanExtensions(const char* functionName)
{
	std::vector<std::string> result;
	if (m_xrInstance == XR_NULL_HANDLE)
		return result;

	PFN_xrGetVulkanInstanceExtensionsKHR getExtensions = nullptr;   // the device one has the same signature
	uint32_t size = 0;
	if (!GetXrFunction(m_xrInstance, functionName, getExtensions) ||
		XR_FAILED(getExtensions(m_xrInstance, m_systemId, 0, &size, nullptr)) || size == 0)
		return result;
	std::string list(size, '\0');
	if (XR_FAILED(getExtensions(m_xrInstance, m_systemId, size, &size, list.data())))
		return result;

	std::istringstream stream(list.c_str());
	std::string extension;
	while (stream >> extension)
		result.push_back(extension);
	return result;
}

std::vector<std::string> OpenXRRenderer::GetRequiredInstanceExtensions()
{
	return GetXrVulkanExtensions("xrGetVulkanInstanceExtensionsKHR");
}

std::vector<std::string> OpenXRRenderer::GetRequiredDeviceExtensions()
{
	return GetXrVulkanExtensions("xrGetVulkanDeviceExtensionsKHR");
}

// The GPU the headset is connected to
VkPhysicalDevice OpenXRRenderer::GetRequiredPhysicalDevice(VkInstance instance)
{
	VkPhysicalDevice device = VK_NULL_HANDLE;
	PFN_xrGetVulkanGraphicsDeviceKHR getDevice = nullptr;
	if (m_xrInstance != XR_NULL_HANDLE && GetXrFunction(m_xrInstance, "xrGetVulkanGraphicsDeviceKHR", getDevice))
		getDevice(m_xrInstance, m_systemId, instance, &device);
	return device;
}

// Once the Vulkan device exists: the stereo image's render pass, then the session and the headset's images
bool OpenXRRenderer::CreateExtraResources()
{
	if (!CreateStereoRenderPass())
		return false;
	if (m_xrInstance != XR_NULL_HANDLE && (!CreateXrSession() || !CreateEyeSwapchains()))
	{
		Logger->warn("OpenXR: could not start a session, rendering to the window only");
		DestroyXrSession();
	}
	return true;
}

bool OpenXRRenderer::CreateXrSession()
{
	XrGraphicsBindingVulkanKHR binding{ XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR };
	binding.instance = m_instance;
	binding.physicalDevice = m_physicalDevice;
	binding.device = m_device;
	binding.queueFamilyIndex = m_graphicsQueueFamily;
	binding.queueIndex = 0;

	XrSessionCreateInfo sessionInfo{ XR_TYPE_SESSION_CREATE_INFO };
	sessionInfo.next = &binding;
	sessionInfo.systemId = m_systemId;
	if (XR_FAILED(xrCreateSession(m_xrInstance, &sessionInfo, &m_session)))
	{
		m_session = XR_NULL_HANDLE;
		return false;
	}

	XrReferenceSpaceCreateInfo spaceInfo{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
	spaceInfo.poseInReferenceSpace.orientation.w = 1.0f;
	spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
	if (XR_FAILED(xrCreateReferenceSpace(m_session, &spaceInfo, &m_localSpace)))
		return false;
	spaceInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
	return XR_SUCCEEDED(xrCreateReferenceSpace(m_session, &spaceInfo, &m_viewSpace));
}

bool OpenXRRenderer::CreateEyeSwapchains()
{
	// The format the eyes are copied into: 8 bit, UNORM first (the game's colours are sRGB already: an SRGB image would
	// encode them twice and wash them out)
	uint32_t formatCount = 0;
	xrEnumerateSwapchainFormats(m_session, 0, &formatCount, nullptr);
	std::vector<int64_t> formats(formatCount);
	if (formatCount == 0 || XR_FAILED(xrEnumerateSwapchainFormats(m_session, formatCount, &formatCount, formats.data())))
		return false;
	const VkFormat preferred[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_R8G8B8A8_SRGB };
	int64_t format = 0;
	for (VkFormat candidate : preferred)
	{
		if (std::find(formats.begin(), formats.end(), (int64_t)candidate) != formats.end())
		{
			format = candidate;
			break;
		}
	}
	if (format == 0)
	{
		Logger->warn("OpenXR: the headset takes none of the 8 bit colour formats");
		return false;
	}
	if (format == VK_FORMAT_B8G8R8A8_SRGB || format == VK_FORMAT_R8G8B8A8_SRGB)
		Logger->warn("OpenXR: the headset only takes sRGB images: the colours will look washed out");

	for (EyeSwapchain& eye : m_eyes)
	{
		if (!CreateSwapchain(eye, format))
			return false;
	}

	// The flat screen: without it the menus aren't in the headset, but the game still plays
	m_screen.width = (int32_t)kScreenImageWidth;
	m_screen.height = (int32_t)kScreenImageHeight;
	if (!CreateSwapchain(m_screen, format))
	{
		Logger->warn("OpenXR: could not create the menu screen's image: the menus show in the window only");
		if (m_screen.handle != XR_NULL_HANDLE)
			xrDestroySwapchain(m_screen.handle);
		m_screen = EyeSwapchain{};
	}
	return true;
}

bool OpenXRRenderer::CreateSwapchain(EyeSwapchain& swapchain, int64_t format)
{
	XrSwapchainCreateInfo swapchainInfo{ XR_TYPE_SWAPCHAIN_CREATE_INFO };
	swapchainInfo.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT | XR_SWAPCHAIN_USAGE_TRANSFER_DST_BIT;
	swapchainInfo.format = format;
	swapchainInfo.sampleCount = 1;
	swapchainInfo.width = (uint32_t)swapchain.width;
	swapchainInfo.height = (uint32_t)swapchain.height;
	swapchainInfo.faceCount = 1;
	swapchainInfo.arraySize = 1;
	swapchainInfo.mipCount = 1;
	if (XR_FAILED(xrCreateSwapchain(m_session, &swapchainInfo, &swapchain.handle)))
	{
		swapchain.handle = XR_NULL_HANDLE;
		return false;
	}

	uint32_t imageCount = 0;
	xrEnumerateSwapchainImages(swapchain.handle, 0, &imageCount, nullptr);
	swapchain.images.assign(imageCount, XrSwapchainImageVulkanKHR{ XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR });
	return imageCount != 0 && XR_SUCCEEDED(xrEnumerateSwapchainImages(swapchain.handle, imageCount, &imageCount,
		reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchain.images.data())));
}

// The stereo image's render pass: as the window's (its format, one sample), read afterwards (the copy and the mirror)
bool OpenXRRenderer::CreateStereoRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapchainFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkAttachmentReference colorRef{ 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef;

	// In: the previous frame's copy and mirror have finished reading it. Out: the mirror reads what was drawn (the copy
	// has its own barrier)
	VkSubpassDependency dependencies[2]{};
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT;
	dependencies[0].srcAccessMask = 0;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	VkRenderPassCreateInfo rpInfo{};
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rpInfo.attachmentCount = 1;
	rpInfo.pAttachments = &colorAttachment;
	rpInfo.subpassCount = 1;
	rpInfo.pSubpasses = &subpass;
	rpInfo.dependencyCount = 2;
	rpInfo.pDependencies = dependencies;
	return vkCreateRenderPass(m_device, &rpInfo, nullptr, &m_stereoRenderPass) == VK_SUCCESS;
}

bool OpenXRRenderer::EnsureStereoTarget(uint32_t width, uint32_t height)
{
	if (m_stereoTarget.framebuffer && m_stereoExtent.width == width && m_stereoExtent.height == height)
		return true;
	// The other frame in flight may still be reading the old image
	vkDeviceWaitIdle(m_device);
	DestroyColourTarget(m_stereoTarget);
	m_stereoExtent = { 0, 0 };
	if (!CreateColourTarget(width, height, m_stereoRenderPass, m_stereoTarget, VK_NULL_HANDLE, VK_IMAGE_USAGE_TRANSFER_SRC_BIT))
	{
		DestroyColourTarget(m_stereoTarget);
		return false;
	}
	m_stereoExtent = { width, height };
	return true;
}

// ── Shut down ────────────────────────────────────────────────────────────────────────────────────────────────
void OpenXRRenderer::Shutdown()
{
	if (!m_initialized)
		return;

	vkDeviceWaitIdle(m_device);
	EndXrFrame(false);
	DestroyXrSession();   // before the Vulkan device it uses
	DestroyColourTarget(m_stereoTarget);
	m_stereoExtent = { 0, 0 };
	vkDestroyRenderPass(m_device, m_stereoRenderPass, nullptr);
	m_stereoRenderPass = VK_NULL_HANDLE;
	VulkanPolygonRenderer::Shutdown();
}

void OpenXRRenderer::DestroyXrSession()
{
	SendHeadPose(false);
	for (EyeSwapchain* swapchain : { &m_eyes[0], &m_eyes[1], &m_screen })
	{
		if (swapchain->handle != XR_NULL_HANDLE)
			xrDestroySwapchain(swapchain->handle);
		swapchain->handle = XR_NULL_HANDLE;
		swapchain->images.clear();
		swapchain->acquired = false;
	}
	if (m_viewSpace != XR_NULL_HANDLE)
		xrDestroySpace(m_viewSpace);
	if (m_localSpace != XR_NULL_HANDLE)
		xrDestroySpace(m_localSpace);
	m_viewSpace = m_localSpace = XR_NULL_HANDLE;
	if (m_session != XR_NULL_HANDLE)
	{
		if (m_sessionRunning)
			xrEndSession(m_session);
		xrDestroySession(m_session);
	}
	m_session = XR_NULL_HANDLE;
	m_sessionRunning = false;
	m_xrFrameBegun = false;
}

void OpenXRRenderer::DestroyXrInstance()
{
	if (m_xrInstance != XR_NULL_HANDLE)
		xrDestroyInstance(m_xrInstance);
	m_xrInstance = XR_NULL_HANDLE;
	m_systemId = XR_NULL_SYSTEM_ID;
}

// ── Frame ────────────────────────────────────────────────────────────────────────────────────────────────────
void OpenXRRenderer::PollXrEvents()
{
	XrEventDataBuffer event{ XR_TYPE_EVENT_DATA_BUFFER };
	while (xrPollEvent(m_xrInstance, &event) == XR_SUCCESS)
	{
		if (event.type == XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED)
		{
			const auto& stateChanged = *reinterpret_cast<const XrEventDataSessionStateChanged*>(&event);
			m_sessionState = stateChanged.state;
			if (m_sessionState == XR_SESSION_STATE_READY)
			{
				XrSessionBeginInfo beginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
				beginInfo.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
				m_sessionRunning = XR_SUCCEEDED(xrBeginSession(m_session, &beginInfo));
				Logger->info("OpenXR: session {}", m_sessionRunning ? "started" : "could not start");
			}
			else if (m_sessionState == XR_SESSION_STATE_STOPPING)
			{
				xrEndSession(m_session);
				m_sessionRunning = false;
				Logger->info("OpenXR: session stopped");
			}
			else if (m_sessionState == XR_SESSION_STATE_EXITING || m_sessionState == XR_SESSION_STATE_LOSS_PENDING)
			{
				m_sessionRunning = false;
			}
		}
		else if (event.type == XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING)
		{
			m_sessionRunning = false;
		}
		event = XrEventDataBuffer{ XR_TYPE_EVENT_DATA_BUFFER };
	}
}

// Waits for the headset's next frame, sends where the head is looking, and takes this frame's headset images
void OpenXRRenderer::OnBeginFrame()
{
	if (m_session == XR_NULL_HANDLE)
		return;

	// The previous frame stopped before it was submitted (the window's image could not be taken): end it empty
	EndXrFrame(false);

	PollXrEvents();
	if (!m_sessionRunning)
	{
		SendHeadPose(false);
		return;
	}

	m_frameState = XrFrameState{ XR_TYPE_FRAME_STATE };
	XrFrameWaitInfo waitInfo{ XR_TYPE_FRAME_WAIT_INFO };
	if (XR_FAILED(xrWaitFrame(m_session, &waitInfo, &m_frameState)))
		return;
	XrFrameBeginInfo beginInfo{ XR_TYPE_FRAME_BEGIN_INFO };
	if (XR_FAILED(xrBeginFrame(m_session, &beginInfo)))
		return;
	m_xrFrameBegun = true;

	// The eyes the game drew for this frame were made with the head pose sent the frame before
	std::copy(std::begin(m_views), std::end(m_views), std::begin(m_renderedViews));
	m_renderedViewsValid = m_viewsValid;

	XrViewLocateInfo locateInfo{ XR_TYPE_VIEW_LOCATE_INFO };
	locateInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	locateInfo.displayTime = m_frameState.predictedDisplayTime;
	locateInfo.space = m_localSpace;
	XrViewState viewState{ XR_TYPE_VIEW_STATE };
	XrView views[kEyeCount] = { { XR_TYPE_VIEW }, { XR_TYPE_VIEW } };
	uint32_t viewCount = 0;
	m_viewsValid = XR_SUCCEEDED(xrLocateViews(m_session, &locateInfo, &viewState, kEyeCount, &viewCount, views)) &&
		viewCount == kEyeCount && (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT);
	if (m_viewsValid)
		std::copy(std::begin(views), std::end(views), std::begin(m_views));

	// The head (between the eyes) for the game's camera
	XrSpaceLocation head{ XR_TYPE_SPACE_LOCATION };
	m_headPoseValid = XR_SUCCEEDED(xrLocateSpace(m_viewSpace, m_localSpace, m_frameState.predictedDisplayTime, &head)) &&
		(head.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT);
	if (m_headPoseValid)
	{
		m_headPose = head.pose;
		if (!(head.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT))
			m_headPose.position = { 0.0f, 0.0f, 0.0f };
		SendHeadPose(true, &head.pose);
	}
	else
		SendHeadPose(false);

	if (!m_frameState.shouldRender || !m_renderedViewsValid)
		return;
	for (EyeSwapchain& eye : m_eyes)
	{
		XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
		if (XR_FAILED(xrAcquireSwapchainImage(eye.handle, &acquireInfo, &eye.acquiredIndex)))
			continue;
		eye.acquired = true;
		XrSwapchainImageWaitInfo imageWaitInfo{ XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
		imageWaitInfo.timeout = XR_INFINITE_DURATION;
		xrWaitSwapchainImage(eye.handle, &imageWaitInfo);
	}
}

// After the frame's commands are submitted: hand the headset its images (they were copied into in this submission)
void OpenXRRenderer::OnFrameSubmitted()
{
	EndXrFrame(true);
}

// Ends the headset's frame with what this frame copied (the eyes or the flat screen); submit false: nothing (the
// frame stopped before it was submitted)
void OpenXRRenderer::EndXrFrame(bool submit)
{
	for (EyeSwapchain* swapchain : { &m_eyes[0], &m_eyes[1], &m_screen })
	{
		if (!swapchain->acquired)
			continue;
		XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
		xrReleaseSwapchainImage(swapchain->handle, &releaseInfo);
		swapchain->acquired = false;
	}
	const bool submitEyes = submit && m_eyesCopied;
	const bool submitScreen = submit && m_screenCopied;
	m_eyesCopied = false;
	m_screenCopied = false;
	if (!m_xrFrameBegun)
		return;

	XrCompositionLayerProjectionView projectionViews[kEyeCount]{};
	XrCompositionLayerProjection layer{ XR_TYPE_COMPOSITION_LAYER_PROJECTION };
	XrCompositionLayerQuad screenLayer{ XR_TYPE_COMPOSITION_LAYER_QUAD };
	const XrCompositionLayerBaseHeader* layers[] = { reinterpret_cast<const XrCompositionLayerBaseHeader*>(&layer) };
	const XrCompositionLayerBaseHeader* screenLayers[] = { reinterpret_cast<const XrCompositionLayerBaseHeader*>(&screenLayer) };

	XrFrameEndInfo endInfo{ XR_TYPE_FRAME_END_INFO };
	endInfo.displayTime = m_frameState.predictedDisplayTime;
	endInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
	if (submitScreen && m_frameState.shouldRender)
	{
		// Where the screen floats: set once, when the flat frames begin, in front of where the head looks then (its
		// direction and tilt up or down, not its roll), so it is in view wherever the runtime's centre is. It then
		// stays there while the head looks around
		if (!m_screenPlaced)
		{
			m_screenPose = XrPosef{};
			m_screenPose.orientation.w = 1.0f;
			m_screenPose.position = { 0.0f, 0.0f, -kScreenDistance };
			if (m_headPoseValid)
			{
				const Vec3 forward = Rotate(m_headPose.orientation, Vec3{ 0.0f, 0.0f, -1.0f });
				const float yaw = std::atan2(-forward.x, -forward.z);
				const float pitch = std::asin((std::clamp)(forward.y, -1.0f, 1.0f));
				const float sy = std::sin(yaw / 2.0f), cy = std::cos(yaw / 2.0f);
				const float sp = std::sin(pitch / 2.0f), cp = std::cos(pitch / 2.0f);
				m_screenPose.orientation = { cy * sp, sy * cp, -sy * sp, cy * cp };   // the yaw, then the pitch
				m_screenPose.position = {
					m_headPose.position.x + forward.x * kScreenDistance,
					m_headPose.position.y + forward.y * kScreenDistance,
					m_headPose.position.z + forward.z * kScreenDistance };
			}
			m_screenPlaced = true;
		}

		// The flat frame on that screen, its shape the frame's
		screenLayer.space = m_localSpace;
		screenLayer.eyeVisibility = XR_EYE_VISIBILITY_BOTH;
		screenLayer.subImage.swapchain = m_screen.handle;
		screenLayer.subImage.imageRect.offset = { 0, 0 };
		screenLayer.subImage.imageRect.extent = { m_screen.width, m_screen.height };
		screenLayer.subImage.imageArrayIndex = 0;
		screenLayer.pose = m_screenPose;
		screenLayer.size = { kScreenWidth, kScreenWidth / m_screenAspect };
		endInfo.layerCount = 1;
		endInfo.layers = screenLayers;
	}
	else if (submitEyes && m_frameState.shouldRender && m_renderedViewsValid)
	{
		// The eyes at the pose and field of view they were drawn for, so the runtime can correct for the head since
		for (uint32_t eye = 0; eye < kEyeCount; eye++)
		{
			projectionViews[eye].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
			projectionViews[eye].pose = m_renderedViews[eye].pose;
			projectionViews[eye].fov = m_renderedViews[eye].fov;
			projectionViews[eye].subImage.swapchain = m_eyes[eye].handle;
			projectionViews[eye].subImage.imageRect.offset = { 0, 0 };
			projectionViews[eye].subImage.imageRect.extent = { m_eyes[eye].width, m_eyes[eye].height };
			projectionViews[eye].subImage.imageArrayIndex = 0;
		}
		layer.space = m_localSpace;
		layer.viewCount = kEyeCount;
		layer.views = projectionViews;
		endInfo.layerCount = 1;
		endInfo.layers = layers;
	}
	xrEndFrame(m_session, &endInfo);
	m_xrFrameBegun = false;
}

// The head's orientation to the game (Vr.cpp), as yaw, pitch and roll; sends the end of tracking once
void OpenXRRenderer::SendHeadPose(bool active, const XrPosef* pose)
{
	if (!EventDispatcher::I || (!active && !m_headPoseActive))
		return;

	VrHeadPose headPose;
	headPose.active = active && pose;
	if (headPose.active)
	{
		// OpenXR: x right, y up, the view looks along -z
		const Vec3 forward = Rotate(pose->orientation, Vec3{ 0.0f, 0.0f, -1.0f });
		const Vec3 right = Rotate(pose->orientation, Vec3{ 1.0f, 0.0f, 0.0f });
		headPose.yaw = std::atan2(-forward.x, -forward.z);
		headPose.pitch = std::asin((std::clamp)(forward.y, -1.0f, 1.0f));
		headPose.roll = std::asin((std::clamp)(right.y, -1.0f, 1.0f));
		// The runtime maps each eye's image across its field of view, tan(angleLeft) at the left edge to tan(angleRight)
		// at the right: straight ahead (tan 0) is at -tanLeft / (tanRight - tanLeft) of the width
		if (m_viewsValid)
		{
			for (uint32_t eye = 0; eye < kEyeCount; eye++)
			{
				const float tanLeft = std::tan(m_views[eye].fov.angleLeft);
				const float tanRight = std::tan(m_views[eye].fov.angleRight);
				const float tanUp = std::tan(m_views[eye].fov.angleUp);
				const float tanDown = std::tan(m_views[eye].fov.angleDown);
				if (tanRight - tanLeft > 0.0f && tanUp - tanDown > 0.0f)
				{
					headPose.eyeCentreX[eye] = -tanLeft / (tanRight - tanLeft) - 0.5f;
					headPose.eyeFovTan[eye][0] = tanLeft;
					headPose.eyeFovTan[eye][1] = tanRight;
					headPose.eyeFovTan[eye][2] = tanUp;
					headPose.eyeFovTan[eye][3] = tanDown;
				}
			}
		}
	}
	m_headPoseActive = headPose.active;
	EventDispatcher::I->DispatchEvent<VrHeadPose>(EventType::E_VR_HEAD_POSE, headPose);
}

// ── Cells: the eyes into the stereo image ────────────────────────────────────────────────────────────────────
// While the headset runs and the game sends the eyes (1 x 2), the cells go into the stereo image: each the whole frame
// at 1:1, the left eye on the left. One view (the menus...) goes into it the same way as one cell, for the flat screen
VulkanPolygonRenderer::FrameCells OpenXRRenderer::GetFrameCells(const SDL_Rect& srcRect, const SDL_Rect& destRect)
{
	const bool hasFrame = m_sessionRunning && srcRect.w > 0 && srcRect.h > 0;
	m_frameStereo = hasFrame && m_viewRows == 1 && m_viewColumns == (int)kEyeCount;
	m_frameScreen = hasFrame && m_viewRows == 1 && m_viewColumns == 1 && m_screen.handle != XR_NULL_HANDLE;
	if (!m_frameScreen)
		m_screenPlaced = false;   // the next menu goes in front of the head again
	const uint32_t cellCount = m_frameStereo ? kEyeCount : 1;
	if ((m_frameStereo || m_frameScreen) && !EnsureStereoTarget((uint32_t)srcRect.w * cellCount, (uint32_t)srcRect.h))
	{
		Logger->error("OpenXR: could not create the {}x{} stereo image", srcRect.w * cellCount, srcRect.h);
		m_frameStereo = m_frameScreen = false;
	}
	if (!m_frameStereo && !m_frameScreen)
		return VulkanPolygonRenderer::GetFrameCells(srcRect, destRect);

	m_frameSrcRect = srcRect;
	m_frameDestRect = destRect;
	m_frameCells.count = cellCount;
	m_frameCells.rect = SDL_Rect{ 0, 0, (int)m_stereoExtent.width, (int)m_stereoExtent.height };
	m_frameCells.extent = m_stereoExtent;
	return m_frameCells;
}

void OpenXRRenderer::BeginCellsRenderPass(VkCommandBuffer commandBuffer, const VkClearValue& clearColor)
{
	if (!m_frameStereo && !m_frameScreen)
	{
		VulkanPolygonRenderer::BeginCellsRenderPass(commandBuffer, clearColor);
		return;
	}

	VkRenderPassBeginInfo stereoBegin{};
	stereoBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	stereoBegin.renderPass = m_stereoRenderPass;
	stereoBegin.framebuffer = m_stereoTarget.framebuffer;
	stereoBegin.renderArea.extent = m_stereoExtent;
	stereoBegin.clearValueCount = 1;
	stereoBegin.pClearValues = &clearColor;
	vkCmdBeginRenderPass(commandBuffer, &stereoBegin, VK_SUBPASS_CONTENTS_INLINE);
}

// The eyes (or the flat frame) into the headset, then the window (EndFrame ends its pass) shows the stereo image as a mirror
void OpenXRRenderer::EndCellsRenderPass(VkCommandBuffer commandBuffer, const VkClearValue& clearColor)
{
	if (!m_frameStereo && !m_frameScreen)
		return;

	vkCmdEndRenderPass(commandBuffer);
	if (m_frameStereo)
		CopyEyesToHeadset(commandBuffer);
	else
		CopyScreenToHeadset(commandBuffer);
	BeginWindowRenderPass(commandBuffer, clearColor);
	DrawMirror(commandBuffer);
}

void OpenXRRenderer::CopyEyesToHeadset(VkCommandBuffer commandBuffer)
{
	if (!m_xrFrameBegun || !m_eyes[0].acquired || !m_eyes[1].acquired)
		return;

	// The stereo image, as its render pass left it, to read from
	ImageBarrier(commandBuffer, m_stereoTarget.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT);

	// Each eye's cell scaled into the headset's image
	for (uint32_t eye = 0; eye < kEyeCount; eye++)
	{
		const EyeSwapchain& swapchain = m_eyes[eye];
		BlitIntoXrImage(commandBuffer, m_stereoTarget.image, GetCellRect(eye, m_frameCells, m_frameSrcRect),
			swapchain.images[swapchain.acquiredIndex].image, swapchain.width, swapchain.height);
	}

	// Back for the mirror
	ImageBarrier(commandBuffer, m_stereoTarget.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT);
	m_eyesCopied = true;
}

// The flat frame (its one cell, the whole stereo image) into the screen's image
void OpenXRRenderer::CopyScreenToHeadset(VkCommandBuffer commandBuffer)
{
	if (!m_xrFrameBegun || !m_frameState.shouldRender || m_screen.handle == XR_NULL_HANDLE)
		return;

	XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
	if (XR_FAILED(xrAcquireSwapchainImage(m_screen.handle, &acquireInfo, &m_screen.acquiredIndex)))
		return;
	m_screen.acquired = true;
	XrSwapchainImageWaitInfo imageWaitInfo{ XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
	imageWaitInfo.timeout = XR_INFINITE_DURATION;
	if (XR_FAILED(xrWaitSwapchainImage(m_screen.handle, &imageWaitInfo)))
		return;   // released (not submitted) when the frame ends

	ImageBarrier(commandBuffer, m_stereoTarget.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT);
	const SDL_Rect whole{ 0, 0, (int)m_stereoExtent.width, (int)m_stereoExtent.height };
	BlitIntoXrImage(commandBuffer, m_stereoTarget.image, whole,
		m_screen.images[m_screen.acquiredIndex].image, m_screen.width, m_screen.height);
	ImageBarrier(commandBuffer, m_stereoTarget.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT, 0, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT);

	m_screenAspect = (float)m_stereoExtent.width / (float)m_stereoExtent.height;
	m_screenCopied = true;
}

// The stereo image (both eyes side by side) in the window, where the frame would be
void OpenXRRenderer::DrawMirror(VkCommandBuffer commandBuffer)
{
	const SDL_Rect& area = m_frameDestRect;
	const int x0 = (std::max)(area.x, 0);
	const int y0 = (std::max)(area.y, 0);
	const int x1 = (std::min)(area.x + area.w, (int)m_swapchainExtent.width);
	const int y1 = (std::min)(area.y + area.h, (int)m_swapchainExtent.height);
	if (area.w <= 0 || area.h <= 0 || x1 <= x0 || y1 <= y0)
		return;

	VkViewport viewport{ (float)area.x, (float)area.y, (float)area.w, (float)area.h, 0.0f, 1.0f };
	VkRect2D scissor{ { x0, y0 }, { (uint32_t)(x1 - x0), (uint32_t)(y1 - y0) } };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_viewPortBlitPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_overlayPipelineLayout, 0, 1,
		&m_stereoTarget.descriptorSet, 0, nullptr);
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}
