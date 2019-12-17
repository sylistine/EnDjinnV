#pragma once

#include <vulkan/vulkan.hpp>

namespace Djn
{
    class Platform
    {
    public:
        Platform(const char* appName);
        ~Platform();
        vk::Instance GetVkInstance() const { return vkInstance; }
        vk::SurfaceKHR GetSurface() const { return surface; }
    private:
        vk::Instance vkInstance;
        vk::SurfaceKHR surface;
        VkDebugReportCallbackEXT callback;
        VkDebugUtilsMessengerEXT messenger;
        //vk::DebugReportCallbackEXT callback;
        bool debugReportCallbackInited = false;
        bool debugUtilsMessengerInited = false;
    };
}