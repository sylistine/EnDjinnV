#include "EnDjinnV.h"

#include "Gfx.h"

#include <vector>

using namespace Djn;

void Gfx::Initialize(VkInstance vkInstance, VkSurfaceKHR surface)
{
	if (gfxInstance != NULL) throw std::exception("Gfx is already initialized.");

	gfxInstance = new Gfx(vkInstance, surface);
}


Gfx* Gfx::gfxInstance = NULL;


Gfx::Gfx(VkInstance vkInstance, VkSurfaceKHR surface) : instance(vkInstance), surface(surface)
{
    VkResult result;

	// todo: better physical device selection.
	primaryGPU = PhysicalDevice(VkUtil::GetDefaultPhysicalDevice(instance));

	// get surface capabilities against chosen device
	uint32_t gfxQueueFamilyIdx;
	uint32_t presentQueueFamilyIdx;
	VkUtil::GetGfxAndPresentQueueFamilyIndex(
		primaryGPU.device,
		primaryGPU.queueFamilyProperties,
		surface,
		gfxQueueFamilyIdx,
		presentQueueFamilyIdx);
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(primaryGPU.device, surface, &surfaceCapabilities);
    if (result != VK_SUCCESS) throw new std::exception("Unable to get surface capabilities.");
    auto physicalDeviceSurfacePresentModes = VkUtil::GetPhysicalDeviceSurfacePresentModes(primaryGPU.device, surface);
    auto physicalDeviceSurfaceFormats = VkUtil::GetPhysicalDeviceSurfaceFormats(primaryGPU.device, surface);
    if (physicalDeviceSurfaceFormats.size() < 1) throw new std::exception("Unable to determine surface formats.");

	// create logical device
	float queuePriorities[1] = { 0.0 };
	
    auto queueCreateInfo = VkUtil::DeviceQueueCreateInfo();
	queueCreateInfo.queueFamilyIndex = gfxQueueFamilyIdx;
	queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePriorities;

	std::vector<const char*> deviceExtensions;
	deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	
    auto deviceCreateInfo = VkUtil::DeviceCreateInfo();
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = NULL;
	deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = NULL;
	result = vkCreateDevice(primaryGPU.device, &deviceCreateInfo, NULL, &device);
	if (result != VK_SUCCESS) throw new std::exception("Unable to create logical device.");

	// create command pools
	auto commandPoolCreateInfo = VkUtil::CommandPoolCreateInfo();
	commandPoolCreateInfo.queueFamilyIndex = gfxQueueFamilyIdx;
	result = vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &cmdPool);
	if (result != VK_SUCCESS) throw new std::exception("Unable to create command pool.");

	// create a command buffer.
	auto commandBufferAllocInfo = VkUtil::CommandBufferAllocateInfo();
	commandBufferAllocInfo.commandPool = cmdPool;
	commandBufferAllocInfo.commandBufferCount = 1;
	result = vkAllocateCommandBuffers(device, &commandBufferAllocInfo, &cmdBuffer);
	if (result != VK_SUCCESS) throw new std::exception("Unable to allocate command buffer.");

    // Do swapchain setup.
    VkFormat swapchainFormat = physicalDeviceSurfaceFormats[0].format;
    if (physicalDeviceSurfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
        swapchainFormat = VK_FORMAT_B8G8R8A8_UNORM;
    }

    VkExtent2D swapchainExtent;
    // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapchainExtent.width = 512;
        swapchainExtent.height = 512;
        if (swapchainExtent.width < surfaceCapabilities.minImageExtent.width) {
            swapchainExtent.width = surfaceCapabilities.minImageExtent.width;
        } else if (swapchainExtent.width > surfaceCapabilities.maxImageExtent.width) {
            swapchainExtent.width = surfaceCapabilities.maxImageExtent.width;
        }

        if (swapchainExtent.height < surfaceCapabilities.minImageExtent.height) {
            swapchainExtent.height = surfaceCapabilities.minImageExtent.height;
        } else if (swapchainExtent.height > surfaceCapabilities.maxImageExtent.height) {
            swapchainExtent.height = surfaceCapabilities.maxImageExtent.height;
        }
    } else {
        // If the surface size is defined, the swap chain size must match
        swapchainExtent = surfaceCapabilities.currentExtent;
    }

    VkSurfaceTransformFlagBitsKHR surfaceTransformFlagBits;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        surfaceTransformFlagBits = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        surfaceTransformFlagBits = surfaceCapabilities.currentTransform;
    }
    
    VkCompositeAlphaFlagBitsKHR compositeAlpha;
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (uint32_t i = 0; i < sizeof(compositeAlphaFlags) / sizeof(compositeAlphaFlags[0]); i++) {
        if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
            compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }

	// create swapchain
	auto swapchainCreateInfo = VkUtil::SwapChainCreateInfo();
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount;
    swapchainCreateInfo.imageFormat = swapchainFormat;
    swapchainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchainCreateInfo.imageExtent = swapchainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.preTransform = surfaceTransformFlagBits;
    swapchainCreateInfo.compositeAlpha = compositeAlpha;
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // Default, always supported.
    swapchainCreateInfo.clipped = true;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = NULL;
    if (gfxQueueFamilyIdx != presentQueueFamilyIdx) {
        uint32_t queueFamilyIndices[2] = {
            gfxQueueFamilyIdx,
            presentQueueFamilyIdx };
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, NULL, &swapchain);
    if (result != VK_SUCCESS) throw new std::exception("Unable to create swapchain");
    swapchainImages = VkUtil::GetSwapchainImages(device, swapchain);

	// create depth buffer
    auto depthImageCreateInfo = VkUtil::ImageCreateInfo();
    depthImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    depthImageCreateInfo.format = VK_FORMAT_D16_UNORM;
    depthImageCreateInfo.extent.width = 512;
    depthImageCreateInfo.extent.height = 512;
    depthImageCreateInfo.extent.depth = 1;
    depthImageCreateInfo.mipLevels = 1;
    depthImageCreateInfo.arrayLayers = 1;
    depthImageCreateInfo.samples = 1;
    depthImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthImageCreateInfo.queueFamilyIndexCount = 0;
    depthImageCreateInfo.pQueueFamilyIndices = NULL;
    depthImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    depthImageCreateInfo.flags = 0;
}


Gfx::~Gfx()
{

}
