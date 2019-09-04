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
    VkPhysicalDevice Physical() const { return infos[preferredGpuIdx].gpu; }
    VkDevice Logical() const { return device; }
    unsigned int GraphicsQueueIdx() const { return graphicsQueueIdx; }
    unsigned int PresentQueueIdx() const { return presentQueueIdx; }
private:
    VkDevice device;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    std::vector<DeviceInfo> infos;
    unsigned int preferredGpuIdx;
    unsigned int graphicsQueueIdx;
    unsigned int presentQueueIdx;
};

