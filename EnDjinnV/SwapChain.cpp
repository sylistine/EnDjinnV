#include "EnDjinnV.h"
#include "SwapChain.h"

#include "VulkanUtil.h"

using namespace Djn::Gfx;

Swapchain::Swapchain(VkDevice device, VkSurfaceKHR surface, VkFormat format,
    VkSurfaceCapabilitiesKHR surfaceCapabilities,
    std::vector<uint32_t> queueFamilyIndices) :
    vkDevice(device)
{
    // Do swapchain setup.

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
    auto swapchainCI = VkUtil::SwapChainCI();
    swapchainCI.surface = surface;
    swapchainCI.minImageCount = surfaceCapabilities.minImageCount;
    swapchainCI.imageFormat = format;
    swapchainCI.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchainCI.imageExtent = swapchainExtent;
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCI.preTransform = surfaceTransformFlagBits;
    swapchainCI.compositeAlpha = compositeAlpha;
    swapchainCI.presentMode = VK_PRESENT_MODE_FIFO_KHR; // Default, always supported.
    swapchainCI.clipped = true;
    swapchainCI.oldSwapchain = VK_NULL_HANDLE;

    if (queueFamilyIndices.size() > 0) {
        swapchainCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCI.queueFamilyIndexCount = queueFamilyIndices.size();
        swapchainCI.pQueueFamilyIndices = queueFamilyIndices.data();
    } else {
        swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCI.queueFamilyIndexCount = 0;
        swapchainCI.pQueueFamilyIndices = NULL;
    }

    VkResult result = vkCreateSwapchainKHR(vkDevice, &swapchainCI, NULL, &vkSwapchain);
    if (result != VK_SUCCESS) throw std::exception("Unable to create swapchain");

    swapchainImages = VkUtil::GetSwapchainImages(vkDevice, vkSwapchain);
    swapchainImageViews.resize(swapchainImages.size());
    for (auto i = 0u; i < swapchainImages.size(); i++) {
        auto swapchainImageViewCI = VkUtil::ImageViewCI();
        swapchainImageViewCI.image = swapchainImages[i];
        swapchainImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
        swapchainImageViewCI.format = format;
        swapchainImageViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
        swapchainImageViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
        swapchainImageViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
        swapchainImageViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
        swapchainImageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        swapchainImageViewCI.subresourceRange.baseMipLevel = 0;
        swapchainImageViewCI.subresourceRange.levelCount = 1;
        swapchainImageViewCI.subresourceRange.baseArrayLayer = 0;
        swapchainImageViewCI.subresourceRange.layerCount = 1;
        result = vkCreateImageView(vkDevice, &swapchainImageViewCI, NULL, &swapchainImageViews[i]);
        if (result != VK_SUCCESS) {
            vkDestroySwapchainKHR(vkDevice, vkSwapchain, NULL);
            throw std::exception("Unable to create views to swapchain images.");
        }
    }

    inited = true;
}


Swapchain::~Swapchain()
{
    FreeDeviceMemory();
}


void Swapchain::FreeDeviceMemory()
{
    if (!inited) return;

    for (auto i = 0u; i < swapchainImages.size(); i++) {
        vkDestroyImageView(vkDevice, swapchainImageViews[i], NULL);
    }
    vkDestroySwapchainKHR(vkDevice, vkSwapchain, NULL);
    inited = false;
}
