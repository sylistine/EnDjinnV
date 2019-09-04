#include "Gfx.h"


Gfx::Gfx(const char* appName) :
    appName(appName),
    instance(appName),
    platform(appName, instance.Get()),
    device(instance.Get(), platform.GetSurface())
{
    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = NULL;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.surface = platform.GetSurface();
    swapchainCreateInfo.minImageCount = 2;
    swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
    swapchainCreateInfo.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchainCreateInfo.imageExtent.width = 256;
    swapchainCreateInfo.imageExtent.height = 256;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = NULL;
    swapchainCreateInfo.preTransform;
    swapchainCreateInfo.compositeAlpha;
    swapchainCreateInfo.presentMode;
    swapchainCreateInfo.clipped;
    swapchainCreateInfo.oldSwapchain;
}


Gfx::~Gfx()
{
}
