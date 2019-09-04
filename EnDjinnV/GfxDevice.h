#pragma once

#include <vector>

#include <vulkan/vulkan.h>

typedef struct DeviceInfo
{
    VkPhysicalDevice gpu;
    std::vector<VkQueueFamilyProperties> queueFamilies;
} GpuInfo;

class GfxDevice
{
public:
    GfxDevice(VkInstance vkInstance, VkSurfaceKHR surface);
    ~GfxDevice();
private:
    VkDevice device;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    std::vector<DeviceInfo> infos;
    unsigned int preferredGpuIdx;
    unsigned int graphicsQueueIdx;
    unsigned int presentQueueIdx;
};

