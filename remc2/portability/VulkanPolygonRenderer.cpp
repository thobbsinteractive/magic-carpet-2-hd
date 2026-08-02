#include "VulkanPolygonRenderer.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

// ---------------------------------------------------------------------
// NOTE: shader bytecode is loaded from .spv files at runtime (see
// shaders/polygon.vert / polygon.frag + INTEGRATION.md for the CMake
// step that compiles them with glslc). Adjust ShaderPath() if you'd
// rather embed the bytes at build time instead.
// ---------------------------------------------------------------------

namespace
{
	std::string ShaderPath(const char* filename)
	{
		return std::string("shaders/") + filename;
	}

	std::vector<char> ReadFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open())
			throw std::runtime_error("Failed to open shader file: " + path);

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		return buffer;
	}

	VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule module;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &module) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module");
		return module;
	}

	// Triangulates a convex polygon (fan from vertex 0) into a flat index list.
	void FanTriangulate(uint32_t baseVertex, uint32_t vertexCount, std::vector<uint32_t>& outIndices)
	{
		for (uint32_t i = 1; i + 1 < vertexCount; i++)
		{
			outIndices.push_back(baseVertex);
			outIndices.push_back(baseVertex + i);
			outIndices.push_back(baseVertex + i + 1);
		}
	}
}

VulkanPolygonRenderer::~VulkanPolygonRenderer()
{
	if (m_initialized)
		Shutdown();
}

void VulkanPolygonRenderer::SetFrontFace(VkFrontFace frontFace)
{
	m_frontFace = frontFace;
	// Pipeline must be rebuilt for this to take effect if called after Init().
}

void VulkanPolygonRenderer::SetCullMode(VkCullModeFlags cullMode)
{
	m_cullMode = cullMode;
}

bool VulkanPolygonRenderer::Init(SDL_Window* window, int windowWidth, int windowHeight)
{
	m_window = window;
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	vkb::InstanceBuilder instanceBuilder;
	auto instRet = instanceBuilder
		.set_app_name("remc2")
		.request_validation_layers(
#ifdef _DEBUG
			true
#else
			false
#endif
		)
		.use_default_debug_messenger()
		.require_api_version(1, 2, 0)
		.build();

	if (!instRet)
		throw std::runtime_error("Failed to create Vulkan instance: " + instRet.error().message());

	m_vkbInstance = instRet.value();
	m_instance = m_vkbInstance.instance;
	m_debugMessenger = m_vkbInstance.debug_messenger;

	if (!SDL_Vulkan_CreateSurface(window, m_instance, &m_surface))
	{
		auto error = std::string(SDL_GetError());
		throw std::runtime_error("SDL_Vulkan_CreateSurface failed: " + error);
	}

	vkb::PhysicalDeviceSelector selector{ m_vkbInstance };
	auto physRet = selector
		.set_surface(m_surface)
		.set_minimum_version(1, 2)
		.select();

	if (!physRet)
		throw std::runtime_error("Failed to select Vulkan physical device: " + physRet.error().message());

	vkb::DeviceBuilder deviceBuilder{ physRet.value() };
	auto devRet = deviceBuilder.build();
	if (!devRet)
		throw std::runtime_error("Failed to create Vulkan device: " + devRet.error().message());

	m_vkbDevice = devRet.value();
	m_device = m_vkbDevice.device;
	m_physicalDevice = physRet.value().physical_device;

	auto gq = m_vkbDevice.get_queue(vkb::QueueType::graphics);
	if (!gq)
		throw std::runtime_error("Failed to get graphics queue: " + gq.error().message());
	m_graphicsQueue = gq.value();
	m_graphicsQueueFamily = m_vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	VmaAllocatorCreateInfo allocatorInfo{};
	allocatorInfo.physicalDevice = m_physicalDevice;
	allocatorInfo.device = m_device;
	allocatorInfo.instance = m_instance;
	allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
	if (vmaCreateAllocator(&allocatorInfo, &m_allocator) != VK_SUCCESS)
		throw std::runtime_error("Failed to create VMA allocator");

	if (!CreateSwapchain(windowWidth, windowHeight))
		return false;
	if (!CreateRenderPass())
		return false;

	// Framebuffers depend on render pass + swapchain image views.
	m_framebuffers.resize(m_swapchainImageViews.size());
	for (size_t i = 0; i < m_swapchainImageViews.size(); i++)
	{
		VkImageView attachments[] = { m_swapchainImageViews[i] };
		VkFramebufferCreateInfo fbInfo{};
		fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbInfo.renderPass = m_renderPass;
		fbInfo.attachmentCount = 1;
		fbInfo.pAttachments = attachments;
		fbInfo.width = m_swapchainExtent.width;
		fbInfo.height = m_swapchainExtent.height;
		fbInfo.layers = 1;
		if (vkCreateFramebuffer(m_device, &fbInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create framebuffer");
	}

	if (!CreateDescriptorResources())
		return false;
	if (!CreatePaletteTexture())
		return false;
	if (!CreatePipeline())
		return false;
	if (!CreateFrameData())
		return false;

	m_initialized = true;
	return true;
}

bool VulkanPolygonRenderer::CreateSwapchain(int width, int height)
{
	vkb::SwapchainBuilder swapchainBuilder{ m_physicalDevice, m_device, m_surface };
	auto swapRet = swapchainBuilder
		.set_desired_extent(width, height)
		.set_desired_format({ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
		.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) // vsync; use MAILBOX_KHR if you want uncapped
		.build();

	if (!swapRet)
		throw std::runtime_error("Failed to create swapchain: " + swapRet.error().message());

	m_vkbSwapchain = swapRet.value();
	m_swapchain = m_vkbSwapchain.swapchain;
	m_swapchainFormat = m_vkbSwapchain.image_format;
	m_swapchainExtent = m_vkbSwapchain.extent;
	m_swapchainImages = m_vkbSwapchain.get_images().value();
	m_swapchainImageViews = m_vkbSwapchain.get_image_views().value();
	return true;
}

void VulkanPolygonRenderer::DestroySwapchain()
{
	for (auto fb : m_framebuffers)
		vkDestroyFramebuffer(m_device, fb, nullptr);
	m_framebuffers.clear();

	m_vkbSwapchain.destroy_image_views(m_swapchainImageViews);
	m_swapchainImageViews.clear();
	m_swapchainImages.clear();

	vkb::destroy_swapchain(m_vkbSwapchain);
	m_swapchain = VK_NULL_HANDLE;
}

void VulkanPolygonRenderer::Resize(int windowWidth, int windowHeight)
{
	vkDeviceWaitIdle(m_device);

	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;

	DestroySwapchain();
	CreateSwapchain(windowWidth, windowHeight);

	m_framebuffers.resize(m_swapchainImageViews.size());
	for (size_t i = 0; i < m_swapchainImageViews.size(); i++)
	{
		VkImageView attachments[] = { m_swapchainImageViews[i] };
		VkFramebufferCreateInfo fbInfo{};
		fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbInfo.renderPass = m_renderPass;
		fbInfo.attachmentCount = 1;
		fbInfo.pAttachments = attachments;
		fbInfo.width = m_swapchainExtent.width;
		fbInfo.height = m_swapchainExtent.height;
		fbInfo.layers = 1;
		vkCreateFramebuffer(m_device, &fbInfo, nullptr, &m_framebuffers[i]);
	}
}

bool VulkanPolygonRenderer::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapchainFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorRef{};
	colorRef.attachment = 0;
	colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo rpInfo{};
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rpInfo.attachmentCount = 1;
	rpInfo.pAttachments = &colorAttachment;
	rpInfo.subpassCount = 1;
	rpInfo.pSubpasses = &subpass;
	rpInfo.dependencyCount = 1;
	rpInfo.pDependencies = &dependency;

	return vkCreateRenderPass(m_device, &rpInfo, nullptr, &m_renderPass) == VK_SUCCESS;
}

bool VulkanPolygonRenderer::CreateDescriptorResources()
{
	// binding 0: palette (256x1 RGBA8), binding 1: current indexed texture (R8_UINT)
	VkDescriptorSetLayoutBinding bindings[2]{};
	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	bindings[1].binding = 1;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings[1].descriptorCount = 1;
	bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 2;
	layoutInfo.pBindings = bindings;
	if (vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
		return false;

	// Pool sized for a generous number of distinct textures; grow if you
	// expect more than 256 live textures at once.
	const uint32_t kMaxTextures = 256;
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize.descriptorCount = kMaxTextures * 2;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets = kMaxTextures;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		return false;

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_NEAREST; // indexed textures must not be filtered
	samplerInfo.minFilter = VK_FILTER_NEAREST;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.unnormalizedCoordinates = VK_TRUE; // sample with texel coords (U/V as-is)
	if (vkCreateSampler(m_device, &samplerInfo, nullptr, &m_nearestSampler) != VK_SUCCESS)
		return false;

	return true;
}

bool VulkanPolygonRenderer::CreatePaletteTexture()
{
	VkImageCreateInfo imgInfo{};
	imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imgInfo.imageType = VK_IMAGE_TYPE_2D;
	imgInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imgInfo.extent = { 256, 1, 1 };
	imgInfo.mipLevels = 1;
	imgInfo.arrayLayers = 1;
	imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imgInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	if (vmaCreateImage(m_allocator, &imgInfo, &allocInfo, &m_paletteImage, &m_paletteAllocation, nullptr) != VK_SUCCESS)
		return false;

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = m_paletteImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	if (vkCreateImageView(m_device, &viewInfo, nullptr, &m_paletteView) != VK_SUCCESS)
		return false;

	// Persistently-mapped staging buffer we re-upload from on every SetPalette().
	VkBufferCreateInfo bufInfo{};
	bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufInfo.size = 256 * 4;
	bufInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo stagingAllocInfo{};
	stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VmaAllocationInfo outInfo{};
	if (vmaCreateBuffer(m_allocator, &bufInfo, &stagingAllocInfo, &m_paletteStagingBuffer, &m_paletteStagingAllocation, &outInfo) != VK_SUCCESS)
		return false;
	m_paletteStagingMapped = outInfo.pMappedData;

	return true;
}

void VulkanPolygonRenderer::SetPalette(const uint8_t* rgb768)
{
	uint8_t* dst = static_cast<uint8_t*>(m_paletteStagingMapped);
	for (int i = 0; i < 256; i++)
	{
		dst[i * 4 + 0] = rgb768[i * 3 + 0];
		dst[i * 4 + 1] = rgb768[i * 3 + 1];
		dst[i * 4 + 2] = rgb768[i * 3 + 2];
		dst[i * 4 + 3] = 255;
	}

	// One-shot copy + layout transition. Simple and correct; if you call
	// SetPalette() every frame, consider batching this into your normal
	// frame command buffer instead of a dedicated submit.
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = m_graphicsQueueFamily;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	VkCommandPool pool;
	vkCreateCommandPool(m_device, &poolInfo, nullptr, &pool);

	VkCommandBufferAllocateInfo cbInfo{};
	cbInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbInfo.commandPool = pool;
	cbInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cbInfo.commandBufferCount = 1;
	VkCommandBuffer cmd;
	vkAllocateCommandBuffers(m_device, &cbInfo, &cmd);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(cmd, &beginInfo);

	VkImageMemoryBarrier toTransfer{};
	toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	toTransfer.image = m_paletteImage;
	toTransfer.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	toTransfer.srcAccessMask = 0;
	toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0, 0, nullptr, 0, nullptr, 1, &toTransfer);

	VkBufferImageCopy copy{};
	copy.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	copy.imageExtent = { 256, 1, 1 };
	vkCmdCopyBufferToImage(cmd, m_paletteStagingBuffer, m_paletteImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

	VkImageMemoryBarrier toShaderRead = toTransfer;
	toShaderRead.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	toShaderRead.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	toShaderRead.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	toShaderRead.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0, 0, nullptr, 0, nullptr, 1, &toShaderRead);

	vkEndCommandBuffer(cmd);

	VkSubmitInfo submit{};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;
	vkQueueSubmit(m_graphicsQueue, 1, &submit, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphicsQueue);

	vkDestroyCommandPool(m_device, pool, nullptr);
}

VkDescriptorSet VulkanPolygonRenderer::AllocateTextureDescriptorSet(VkImageView view)
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &m_descriptorSetLayout;

	VkDescriptorSet set;
	if (vkAllocateDescriptorSets(m_device, &allocInfo, &set) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate descriptor set (out of pool space? raise kMaxTextures)");

	VkDescriptorImageInfo paletteInfo{};
	paletteInfo.sampler = m_nearestSampler;
	paletteInfo.imageView = m_paletteView;
	paletteInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkDescriptorImageInfo texInfo{};
	texInfo.sampler = m_nearestSampler;
	texInfo.imageView = view;
	texInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	VkWriteDescriptorSet writes[2]{};
	writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writes[0].dstSet = set;
	writes[0].dstBinding = 0;
	writes[0].descriptorCount = 1;
	writes[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writes[0].pImageInfo = &paletteInfo;

	writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writes[1].dstSet = set;
	writes[1].dstBinding = 1;
	writes[1].descriptorCount = 1;
	writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writes[1].pImageInfo = &texInfo;

	vkUpdateDescriptorSets(m_device, 2, writes, 0, nullptr);
	return set;
}

uint32_t VulkanPolygonRenderer::UploadTexture(const uint8_t* indexedPixels, uint32_t width, uint32_t height)
{
	VkDeviceSize size = (VkDeviceSize)width * height;

	VkBufferCreateInfo stagingInfo{};
	stagingInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	stagingInfo.size = size;
	stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo stagingAllocInfo{};
	stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
	stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	VmaAllocationInfo stagingOut{};
	vmaCreateBuffer(m_allocator, &stagingInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, &stagingOut);
	std::memcpy(stagingOut.pMappedData, indexedPixels, (size_t)size);

	Texture tex{};
	tex.width = width;
	tex.height = height;

	VkImageCreateInfo imgInfo{};
	imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imgInfo.imageType = VK_IMAGE_TYPE_2D;
	imgInfo.format = VK_FORMAT_R8_UINT; // raw palette index, resolved in the fragment shader
	imgInfo.extent = { width, height, 1 };
	imgInfo.mipLevels = 1;
	imgInfo.arrayLayers = 1;
	imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imgInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	vmaCreateImage(m_allocator, &imgInfo, &allocInfo, &tex.image, &tex.allocation, nullptr);

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = tex.image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8_UINT;
	viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	vkCreateImageView(m_device, &viewInfo, nullptr, &tex.view);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = m_graphicsQueueFamily;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	VkCommandPool pool;
	vkCreateCommandPool(m_device, &poolInfo, nullptr, &pool);

	VkCommandBufferAllocateInfo cbInfo{};
	cbInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbInfo.commandPool = pool;
	cbInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cbInfo.commandBufferCount = 1;
	VkCommandBuffer cmd;
	vkAllocateCommandBuffers(m_device, &cbInfo, &cmd);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(cmd, &beginInfo);

	VkImageMemoryBarrier toTransfer{};
	toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	toTransfer.image = tex.image;
	toTransfer.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	toTransfer.srcAccessMask = 0;
	toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0, 0, nullptr, 0, nullptr, 1, &toTransfer);

	VkBufferImageCopy copy{};
	copy.imageSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
	copy.imageExtent = { width, height, 1 };
	vkCmdCopyBufferToImage(cmd, stagingBuffer, tex.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

	VkImageMemoryBarrier toShaderRead = toTransfer;
	toShaderRead.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	toShaderRead.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	toShaderRead.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	toShaderRead.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0, 0, nullptr, 0, nullptr, 1, &toShaderRead);

	vkEndCommandBuffer(cmd);

	VkSubmitInfo submit{};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;
	vkQueueSubmit(m_graphicsQueue, 1, &submit, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_graphicsQueue);

	vkDestroyCommandPool(m_device, pool, nullptr);
	vmaDestroyBuffer(m_allocator, stagingBuffer, stagingAllocation);

	tex.descriptorSet = AllocateTextureDescriptorSet(tex.view);

	uint32_t id = m_nextTextureId++;
	m_textures[id] = tex;
	return id;
}

void VulkanPolygonRenderer::FreeTexture(uint32_t textureId)
{
	auto it = m_textures.find(textureId);
	if (it == m_textures.end())
		return;

	vkDeviceWaitIdle(m_device); // simplest-correct; batch frees if this shows up in profiling
	vkFreeDescriptorSets(m_device, m_descriptorPool, 1, &it->second.descriptorSet);
	vkDestroyImageView(m_device, it->second.view, nullptr);
	vmaDestroyImage(m_allocator, it->second.image, it->second.allocation);
	m_textures.erase(it);
}

bool VulkanPolygonRenderer::CreatePipeline()
{
	auto vertCode = ReadFile(ShaderPath("polygon.vert.spv"));
	auto fragCode = ReadFile(ShaderPath("polygon.frag.spv"));
	VkShaderModule vertModule = CreateShaderModule(m_device, vertCode);
	VkShaderModule fragModule = CreateShaderModule(m_device, fragCode);

	if (vertModule == VK_NULL_HANDLE || fragModule == VK_NULL_HANDLE)
		return false;

	VkPipelineShaderStageCreateInfo vertStage{};
	vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStage.module = vertModule;
	vertStage.pName = "main";

	VkPipelineShaderStageCreateInfo fragStage{};
	fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStage.module = fragModule;
	fragStage.pName = "main";

	VkPipelineShaderStageCreateInfo stages[] = { vertStage, fragStage };

	VkVertexInputBindingDescription binding{};
	binding.binding = 0;
	binding.stride = sizeof(Vertex);
	binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription attrs[3]{};
	attrs[0] = { 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, x) };
	attrs[1] = { 1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, u) };
	attrs[2] = { 2, 0, VK_FORMAT_R32_SFLOAT, offsetof(Vertex, brightness) };

	VkPipelineVertexInputStateCreateInfo vertexInput{};
	vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInput.vertexBindingDescriptionCount = 1;
	vertexInput.pVertexBindingDescriptions = &binding;
	vertexInput.vertexAttributeDescriptionCount = 3;
	vertexInput.pVertexAttributeDescriptions = attrs;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo raster{};
	raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	raster.polygonMode = VK_POLYGON_MODE_FILL;
	raster.cullMode = m_cullMode;
	raster.frontFace = m_frontFace;
	raster.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisample{};
	multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState blendAttachment{};
	blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	blendAttachment.blendEnable = VK_FALSE; // no transparency in the base pass; enable + set factors if you need it

	VkPipelineColorBlendStateCreateInfo blend{};
	blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blend.attachmentCount = 1;
	blend.pAttachments = &blendAttachment;

	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	// Push constant: screen size, used by the vertex shader to map pixel
	// coords -> NDC (orthographic, no matrix needed since input is already
	// screen-space).
	VkPushConstantRange pushConstant{};
	pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstant.offset = 0;
	pushConstant.size = sizeof(float) * 2;

	VkPipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &m_descriptorSetLayout;
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.pPushConstantRanges = &pushConstant;
	if (vkCreatePipelineLayout(m_device, &layoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		return false;

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = stages;
	pipelineInfo.pVertexInputState = &vertexInput;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &raster;
	pipelineInfo.pMultisampleState = &multisample;
	pipelineInfo.pColorBlendState = &blend;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = m_renderPass;
	pipelineInfo.subpass = 0;

	VkResult result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);

	vkDestroyShaderModule(m_device, vertModule, nullptr);
	vkDestroyShaderModule(m_device, fragModule, nullptr);

	return result == VK_SUCCESS;
}

bool VulkanPolygonRenderer::CreateFrameData()
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = m_graphicsQueueFamily;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkSemaphoreCreateInfo semInfo{};
	semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for (auto& frame : m_frames)
	{
		if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &frame.commandPool) != VK_SUCCESS)
			return false;

		VkCommandBufferAllocateInfo cbInfo{};
		cbInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbInfo.commandPool = frame.commandPool;
		cbInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cbInfo.commandBufferCount = 1;
		if (vkAllocateCommandBuffers(m_device, &cbInfo, &frame.commandBuffer) != VK_SUCCESS)
			return false;

		vkCreateSemaphore(m_device, &semInfo, nullptr, &frame.imageAvailable);
		vkCreateSemaphore(m_device, &semInfo, nullptr, &frame.renderFinished);
		vkCreateFence(m_device, &fenceInfo, nullptr, &frame.inFlight);

		// Reasonable starting size; EnsureFrameBufferCapacity grows these
		// on demand if a frame submits more geometry.
		EnsureFrameBufferCapacity(frame, sizeof(Vertex) * 4096, sizeof(uint32_t) * 8192);
	}

	return true;
}

void VulkanPolygonRenderer::EnsureFrameBufferCapacity(FrameData& frame, size_t vertexBytes, size_t indexBytes)
{
	if (frame.vertexBufferSize < vertexBytes)
	{
		if (frame.vertexBuffer)
			vmaDestroyBuffer(m_allocator, frame.vertexBuffer, frame.vertexAllocation);

		VkBufferCreateInfo bufInfo{};
		bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufInfo.size = vertexBytes;
		bufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VmaAllocationInfo outInfo{};
		vmaCreateBuffer(m_allocator, &bufInfo, &allocInfo, &frame.vertexBuffer, &frame.vertexAllocation, &outInfo);
		frame.vertexMapped = outInfo.pMappedData;
		frame.vertexBufferSize = vertexBytes;
	}

	if (frame.indexBufferSize < indexBytes)
	{
		if (frame.indexBuffer)
			vmaDestroyBuffer(m_allocator, frame.indexBuffer, frame.indexAllocation);

		VkBufferCreateInfo bufInfo{};
		bufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufInfo.size = indexBytes;
		bufInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VmaAllocationInfo outInfo{};
		vmaCreateBuffer(m_allocator, &bufInfo, &allocInfo, &frame.indexBuffer, &frame.indexAllocation, &outInfo);
		frame.indexMapped = outInfo.pMappedData;
		frame.indexBufferSize = indexBytes;
	}
}

void VulkanPolygonRenderer::DestroyFrameData()
{
	for (auto& frame : m_frames)
	{
		if (frame.vertexBuffer) vmaDestroyBuffer(m_allocator, frame.vertexBuffer, frame.vertexAllocation);
		if (frame.indexBuffer) vmaDestroyBuffer(m_allocator, frame.indexBuffer, frame.indexAllocation);
		vkDestroySemaphore(m_device, frame.imageAvailable, nullptr);
		vkDestroySemaphore(m_device, frame.renderFinished, nullptr);
		vkDestroyFence(m_device, frame.inFlight, nullptr);
		vkDestroyCommandPool(m_device, frame.commandPool, nullptr);
	}
}

bool VulkanPolygonRenderer::BeginFrame()
{
	FrameData& frame = m_frames[m_currentFrame];
	vkWaitForFences(m_device, 1, &frame.inFlight, VK_TRUE, UINT64_MAX);

	VkResult acquireResult = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX,
		frame.imageAvailable, VK_NULL_HANDLE, &m_currentImageIndex);

	if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		Resize(m_windowWidth, m_windowHeight);
		return false; // skip this frame; caller should just try again next tick
	}

	vkResetFences(m_device, 1, &frame.inFlight);
	vkResetCommandBuffer(frame.commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(frame.commandBuffer, &beginInfo);

	VkClearValue clearColor{};
	clearColor.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

	VkRenderPassBeginInfo rpBegin{};
	rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpBegin.renderPass = m_renderPass;
	rpBegin.framebuffer = m_framebuffers[m_currentImageIndex];
	rpBegin.renderArea.extent = m_swapchainExtent;
	rpBegin.clearValueCount = 1;
	rpBegin.pClearValues = &clearColor;
	vkCmdBeginRenderPass(frame.commandBuffer, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

	VkViewport viewport{ 0, 0, (float)m_swapchainExtent.width, (float)m_swapchainExtent.height, 0.0f, 1.0f };
	VkRect2D scissor{ {0, 0}, m_swapchainExtent };
	vkCmdSetViewport(frame.commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);

	float screenSize[2] = { (float)m_swapchainExtent.width, (float)m_swapchainExtent.height };
	vkCmdPushConstants(frame.commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(screenSize), screenSize);

	return true;
}

void VulkanPolygonRenderer::DrawPolygons(const std::vector<RenderPolygon>& polygons)
{
	FrameData& frame = m_frames[m_currentFrame];

	// Build the full vertex/index list for the frame up front, grouped so
	// each contiguous run of polygons sharing a TextureId becomes one draw
	// call (minimizes descriptor set binds; this is a CPU-side sort, cheap
	// relative to typical retro polygon counts per frame).
	std::vector<RenderPolygon> sorted = polygons;
	std::stable_sort(sorted.begin(), sorted.end(),
		[](const RenderPolygon& a, const RenderPolygon& b) { return a.TextureId < b.TextureId; });

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	vertices.reserve(polygons.size() * 4);
	indices.reserve(polygons.size() * 6);

	struct Batch { uint32_t textureId; uint32_t indexOffset; uint32_t indexCount; };
	std::vector<Batch> batches;

	uint32_t currentTexture = sorted.empty() ? 0 : sorted[0].TextureId;
	uint32_t batchIndexStart = 0;

	for (const auto& poly : sorted)
	{
		if (poly.Vertices.size() < 3)
			continue;

		if (poly.TextureId != currentTexture)
		{
			batches.push_back({ currentTexture, batchIndexStart, (uint32_t)indices.size() - batchIndexStart });
			batchIndexStart = (uint32_t)indices.size();
			currentTexture = poly.TextureId;
		}

		uint32_t baseVertex = (uint32_t)vertices.size();
		for (const auto& v : poly.Vertices)
		{
			Vertex vert{};
			vert.x = (float)v.X;
			vert.y = (float)v.Y;
			vert.u = (float)v.U;
			vert.v = (float)v.V;
			vert.brightness = (float)v.Brightness / 255.0f;
			vertices.push_back(vert);
		}
		FanTriangulate(baseVertex, (uint32_t)poly.Vertices.size(), indices);
	}
	if (!sorted.empty())
		batches.push_back({ currentTexture, batchIndexStart, (uint32_t)indices.size() - batchIndexStart });

	if (vertices.empty() || indices.empty())
		return;

	EnsureFrameBufferCapacity(frame, vertices.size() * sizeof(Vertex), indices.size() * sizeof(uint32_t));
	std::memcpy(frame.vertexMapped, vertices.data(), vertices.size() * sizeof(Vertex));
	std::memcpy(frame.indexMapped, indices.data(), indices.size() * sizeof(uint32_t));

	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(frame.commandBuffer, 0, 1, &frame.vertexBuffer, &offset);
	vkCmdBindIndexBuffer(frame.commandBuffer, frame.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

	for (const auto& batch : batches)
	{
		auto it = m_textures.find(batch.textureId);
		if (it == m_textures.end())
			continue; // unknown texture id - skip rather than crash

		vkCmdBindDescriptorSets(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_pipelineLayout, 0, 1, &it->second.descriptorSet, 0, nullptr);
		vkCmdDrawIndexed(frame.commandBuffer, batch.indexCount, 1, batch.indexOffset, 0, 0);
	}
}

void VulkanPolygonRenderer::EndFrame()
{
	FrameData& frame = m_frames[m_currentFrame];

	vkCmdEndRenderPass(frame.commandBuffer);
	vkEndCommandBuffer(frame.commandBuffer);

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submit{};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &frame.imageAvailable;
	submit.pWaitDstStageMask = &waitStage;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &frame.commandBuffer;
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &frame.renderFinished;
	vkQueueSubmit(m_graphicsQueue, 1, &submit, frame.inFlight);

	VkPresentInfoKHR present{};
	present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present.waitSemaphoreCount = 1;
	present.pWaitSemaphores = &frame.renderFinished;
	present.swapchainCount = 1;
	present.pSwapchains = &m_swapchain;
	present.pImageIndices = &m_currentImageIndex;
	VkResult presentResult = vkQueuePresentKHR(m_graphicsQueue, &present);

	if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR)
		Resize(m_windowWidth, m_windowHeight);

	m_currentFrame = (m_currentFrame + 1) % kFramesInFlight;
}

void VulkanPolygonRenderer::Shutdown()
{
	if (!m_initialized)
		return;

	vkDeviceWaitIdle(m_device);

	for (auto& [id, tex] : m_textures)
	{
		vkFreeDescriptorSets(m_device, m_descriptorPool, 1, &tex.descriptorSet);
		vkDestroyImageView(m_device, tex.view, nullptr);
		vmaDestroyImage(m_allocator, tex.image, tex.allocation);
	}
	m_textures.clear();

	DestroyFrameData();

	vmaDestroyBuffer(m_allocator, m_paletteStagingBuffer, m_paletteStagingAllocation);
	vkDestroyImageView(m_device, m_paletteView, nullptr);
	vmaDestroyImage(m_allocator, m_paletteImage, m_paletteAllocation);

	vkDestroyPipeline(m_device, m_pipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	vkDestroySampler(m_device, m_nearestSampler, nullptr);
	vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
	vkDestroyRenderPass(m_device, m_renderPass, nullptr);

	DestroySwapchain();

	vmaDestroyAllocator(m_allocator);

	vkb::destroy_surface(m_vkbInstance, m_surface);
	vkb::destroy_device(m_vkbDevice);
	vkb::destroy_instance(m_vkbInstance);

	m_initialized = false;
}
