#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class GfxInstance
{
public:
    GfxInstance(const char* appName);
    ~GfxInstance();
    VkPhysicalDevice GetPreferredGpu() const { return gpus[preferredGpuIdx]; }
    unsigned int GetPreferredGpuPropertyFamilyIdx() const { return preferredGpuPropertyFamilyIdx; }
private:
    VkInstance instance;
    unsigned int preferredGpuIdx;
    std::vector<VkPhysicalDevice> gpus;
    unsigned int preferredGpuPropertyFamilyIdx;
    std::vector<std::vector<VkQueueFamilyProperties>> gpuPropertyFamily;
};

