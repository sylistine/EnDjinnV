#include "SwapChain.h"

#include <exception>
#include <vector>

SwapChain::SwapChain(GfxDevice device, Platform platform)
{
    auto physicalDevice = device.Physical();
    auto surface = platform.GetSurface();

    VkResult result;
    // Get the list of VkFormats that are supported:
    uint32_t formatCount;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice,
        surface,
        &formatCount,
        NULL);
    if (result != VK_SUCCESS) throw std::exception("Unable to retrieve surface format count.");

    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    surfaceFormats.resize(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(
        physicalDevice,
        surface,
        &formatCount,
        surfaceFormats.data());
    if (result != VK_SUCCESS)  throw std::exception("Unable to retrieve surface formats.");

    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
        swapchainFormat = VK_FORMAT_B8G8R8A8_UNORM;
    } else {
        if (formatCount < 1) throw std::exception("Unable to determine swapchain format.");
        // TODO: Smarter swapchain format picking.
        swapchainFormat = surfaceFormats[0].format;
    }

    // Get all capabilities.
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
    if (result != VK_SUCCESS) throw std::exception("Unable to get physical device surface capabilities.");

    // Get the present mode count.
    uint32_t presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
    if (result != VK_SUCCESS) throw std::exception("Unable to get physical device surface present mode count.");

    std::vector<VkPresentModeKHR> presentModes;
    presentModes.resize(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
    if (result != VK_SUCCESS) throw std::exception("Unable to get physical device surface present modes.");

    VkExtent2D swapchainExtent;
    // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapchainExtent.width = platform.GetWindowRect().w;
        swapchainExtent.height = platform.GetWindowRect().h;
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

    uint32_t desiredNumberOfSwapChainImages = surfaceCapabilities.minImageCount;

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        preTransform = surfaceCapabilities.currentTransform;
    }

    // Find a supported composite alpha mode - one of these is guaranteed to be set
    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
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

    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = NULL;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = desiredNumberOfSwapChainImages;
    swapchainCreateInfo.imageFormat = swapchainFormat;
    swapchainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchainCreateInfo.imageExtent = swapchainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = NULL;
    swapchainCreateInfo.preTransform = preTransform;
    swapchainCreateInfo.compositeAlpha = compositeAlpha;
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // Default, always supported.
    swapchainCreateInfo.clipped = true;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    if (device.GraphicsQueueIdx() != device.PresentQueueIdx()) {
        // If the graphics and present queues are from different queue families,
        // we either have to explicitly transfer ownership of images between
        // the queues, or we have to create the swapchain with imageSharingMode
        // as VK_SHARING_MODE_CONCURRENT
        uint32_t queueFamilyIndices[2] = {
            device.GraphicsQueueIdx(),
            device.PresentQueueIdx() };
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
}


SwapChain::~SwapChain()
{}
