#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class GfxInstance
{
public:
    GfxInstance(const char* appName);
    ~GfxInstance();
    VkInstance Get() { return instance; }
private:
    VkInstance instance;
};

