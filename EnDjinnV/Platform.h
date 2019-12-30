#pragma once

#include <Windows.h>
#include <vulkan/vulkan.h>

#include "EndjinnV.h"

namespace Djn::Platform
{
    typedef struct Surface
    {
        enum class State
        {
            Windowed,
            Minimized,
            Maximized,
            Resizing
        };
        State state;
        uint32_t posX;
        uint32_t posY;
        uint32_t width;
        uint32_t height;
    } Surface;

    class PlatformHandler
    {
    public:
        static PlatformHandler* GetHandler();
        static void Initialize(const char* appName);

        LRESULT wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        bool getDidResize() const
        {
            return didResize;
        }
        void getSurfacePosition(uint32_t& x, uint32_t& y) const
        {
            x = surface.posX;
            y = surface.posY;
        }
        void getSurfaceSize(uint32_t& width, uint32_t& height) const
        {
            width = surface.width;
            height = surface.height;
        }
        bool getIsQuitting() const { return isQuitting; }
        VkSurfaceKHR getRenderSurface() const { return renderSurface; }
        void createSurface(VkInstance instance);
        void destroySurface();
        bool update();
    private:
        static PlatformHandler* instance;
        PlatformHandler(const char* appName);
        // Platform specific.
        const char* windowClassName;
        HINSTANCE moduleHandle;
        HWND windowHandle;
        // Non platform specific.
        VkInstance vkInstance;
        Surface surface;
        VkSurfaceKHR renderSurface;
        bool isQuitting;
        bool didResize;
    };
} // namespace Djn::Platform
