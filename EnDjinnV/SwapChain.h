#pragma once

#include <vulkan/vulkan.h>

#include "GfxDevice.h"
#include "Platform.h"

class SwapChain
{
public:
    SwapChain(GfxDevice physicalDevice, Platform platform);
    ~SwapChain();
private:
    VkFormat swapchainFormat;
};

