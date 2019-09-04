#pragma once

#include <vulkan/vulkan.h>

typedef struct
{
    int x;
    int y;
    int w;
    int h;
} WindowRect;

class Platform
{
public:
    Platform(const char* appName, VkInstance instance);
    ~Platform();
    WindowRect GetWindowRect() const { return windowRect; }
    VkSurfaceKHR GetSurface() const { return surface; }
private:
    WindowRect windowRect;
    VkSurfaceKHR surface;
};