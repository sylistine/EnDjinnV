#include "EnDjinnV.h"
#include "SwapChain.h"

using namespace Djn::Gfx;

Swapchain::Swapchain(
    vk::Device device,
    vk::SurfaceKHR surface,
    vk::Format format,
    vk::Extent2D imageExtent,
    vk::SurfaceCapabilitiesKHR surfaceCapabilities,
    std::vector<uint32_t> queueFamilyIndices) :
    device(device)
{

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

    vk::SwapchainKHR oldSwapchain;
    swapchainCI.surface = surface;
    swapchainCI.minImageCount = surfaceCapabilities.minImageCount;
    swapchainCI.imageFormat = format;
    swapchainCI.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    swapchainCI.imageExtent = imageExtent;
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
}


Swapchain::~Swapchain()
{
    destroySwapchain();
}


void Swapchain::resize(vk::Extent2D newSize)
{
    destroySwapchain();
    swapchainCI.imageExtent = newSize;
    createSwapchain();
}


void Swapchain::createSwapchain()
{
    vk::Result result = device.createSwapchainKHR(&swapchainCI, NULL, &swapchain);
    if (result != vk::Result::eSuccess) throw Exception("Unable to create swapchain");

    swapchainImages = device.getSwapchainImagesKHR(swapchain);

    viewHelper = ViewHelper(device, swapchainCI.imageFormat);
    try {
        viewHelper.createViews(swapchainImages);
    } catch (Exception & e) {
        viewHelper.destroyViews();
        device.destroySwapchainKHR(swapchain);
        throw e;
    }
}


void Swapchain::destroySwapchain()
{
    viewHelper.destroyViews();
    vkDestroySwapchainKHR(device, swapchain, NULL);
}
