#pragma once

#include <vulkan/vulkan.h>

class GfxDevice
{
public:
    GfxDevice(VkPhysicalDevice gpuIdx, unsigned int gpuPropertyFamilyIdx);
    ~GfxDevice();
private:
    VkDevice device;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
};

