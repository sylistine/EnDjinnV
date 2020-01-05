#include "EnDjinnV.h"
#include "SwapChain.h"

using namespace Djn::Gfx;

Swapchain::Swapchain(vk::Device device, vk::SurfaceKHR surface, vk::Format format,
    vk::SurfaceCapabilitiesKHR surfaceCapabilities,
    std::vector<uint32_t> queueFamilyIndices) :
    vkDevice(device)
{
    // Do swapchain setup.

    VkExtent2D swapchainExtent;
    // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapchainExtent = surfaceCapabilities.currentExtent;
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

    vk::SurfaceTransformFlagBitsKHR surfaceTransformFlagBits;
    if (surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
        surfaceTransformFlagBits = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    } else {
        surfaceTransformFlagBits = surfaceCapabilities.currentTransform;
    }

    vk::CompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        vk::CompositeAlphaFlagBitsKHR::ePreMultiplied,
        vk::CompositeAlphaFlagBitsKHR::ePostMultiplied,
        vk::CompositeAlphaFlagBitsKHR::eInherit
    };
    uint32_t compositeAlphaIdx = UINT32_MAX;
    for (uint32_t i = 0; i < sizeof(compositeAlphaFlags) / sizeof(compositeAlphaFlags[0]); i++) {
        if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
            compositeAlphaIdx = i;
            break;
        }
    }
    if (compositeAlphaIdx == UINT32_MAX) throw Exception("Unable to select correct composite alpha type.");

    // create swapchain
    vk::SwapchainKHR oldSwapchain;
    vk::SwapchainCreateInfoKHR swapchainCI;
    swapchainCI.surface = surface;
    swapchainCI.minImageCount = surfaceCapabilities.minImageCount;
    swapchainCI.imageFormat = format;
    swapchainCI.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    swapchainCI.imageExtent = swapchainExtent;
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapchainCI.preTransform = surfaceTransformFlagBits;
    swapchainCI.compositeAlpha = compositeAlphaFlags[compositeAlphaIdx];
    swapchainCI.presentMode = vk::PresentModeKHR::eFifo; // Default, always supported.
    swapchainCI.clipped = true;
    swapchainCI.oldSwapchain = oldSwapchain;

    if (queueFamilyIndices.size() > 0) {
        swapchainCI.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCI.queueFamilyIndexCount = (uint32_t)queueFamilyIndices.size();
        swapchainCI.pQueueFamilyIndices = queueFamilyIndices.data();
    } else {
        swapchainCI.imageSharingMode = vk::SharingMode::eExclusive;
        swapchainCI.queueFamilyIndexCount = 0;
        swapchainCI.pQueueFamilyIndices = NULL;
    }

    vk::Result result = vkDevice.createSwapchainKHR(&swapchainCI, NULL, &vkSwapchain);
    if (result != vk::Result::eSuccess) throw Exception("Unable to create swapchain");

    swapchainImages = vkDevice.getSwapchainImagesKHR(vkSwapchain);
    swapchainImageViews.resize(swapchainImages.size());
    for (auto i = 0u; i < swapchainImages.size(); i++) {
        vk::ImageViewCreateInfo swapchainImageViewCI;
        swapchainImageViewCI.image = swapchainImages[i];
        swapchainImageViewCI.viewType = vk::ImageViewType::e2D;
        swapchainImageViewCI.format = format;
        swapchainImageViewCI.components.r = vk::ComponentSwizzle::eR;
        swapchainImageViewCI.components.g = vk::ComponentSwizzle::eG;
        swapchainImageViewCI.components.b = vk::ComponentSwizzle::eB;
        swapchainImageViewCI.components.a = vk::ComponentSwizzle::eA;
        swapchainImageViewCI.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        swapchainImageViewCI.subresourceRange.baseMipLevel = 0;
        swapchainImageViewCI.subresourceRange.levelCount = 1;
        swapchainImageViewCI.subresourceRange.baseArrayLayer = 0;
        swapchainImageViewCI.subresourceRange.layerCount = 1;
        result = vkDevice.createImageView(&swapchainImageViewCI, NULL, &swapchainImageViews[i]);
        if (result != vk::Result::eSuccess) {
            for (auto j = i; j > 0; j--) {
                vkDevice.destroyImageView(swapchainImageViews[i - 1]);
            }
            vkDevice.destroySwapchainKHR(vkSwapchain);
            throw Exception("Unable to create views to swapchain images.");
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

    std::cout << "Deleting swapchain." << std::endl;
    for (auto i = 0u; i < swapchainImages.size(); i++) {
        vkDestroyImageView(vkDevice, swapchainImageViews[i], NULL);
    }
    vkDestroySwapchainKHR(vkDevice, vkSwapchain, NULL);
    inited = false;
}
