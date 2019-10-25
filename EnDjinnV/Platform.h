#pragma once

#include <vulkan/vulkan.h>

namespace Djn
{
    class Platform
    {
    public:
        Platform(const char* appName);
        ~Platform();
        VkInstance GetVkInstance() const { return vkInstance; }
        VkSurfaceKHR GetSurface() const { return surface; }
    private:
        VkInstance vkInstance;
        VkSurfaceKHR surface;
    };
}