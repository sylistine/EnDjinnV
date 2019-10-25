#pragma once

#include <vulkan/vulkan.h>
#include "VulkanUtil.h"

#include "DepthTexture.h"

namespace Djn
{
    typedef struct PhysicalDevice
    {
        PhysicalDevice() {}
        PhysicalDevice(VkPhysicalDevice dev) :
            device(dev)
        {
            queueFamilyProperties = VkUtil::GetPhysicalDeviceQueueFamilyProperties(dev);
            vkGetPhysicalDeviceMemoryProperties(dev, &memoryProperties);
        }
        VkPhysicalDevice device;
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        VkPhysicalDeviceMemoryProperties memoryProperties;

    } PhysicalDevice;

    class Gfx
    {
    public:
        static void Initialize(VkInstance vkInstance, VkSurfaceKHR surface);
    private:
        static Gfx* gfxInstance;
        Gfx(VkInstance vkInstance, VkSurfaceKHR surface);
        ~Gfx();

        VkInstance instance;
        VkSurfaceKHR surface;
        // Physical device data.
        PhysicalDevice primaryGPU;
        // Logical device data.
        VkDevice device;
        VkCommandPool cmdPool;
        VkCommandBuffer cmdBuffer;
        VkSwapchainKHR swapchain;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        VkImage depthImage;
        DepthTexture* depthTexture;
    };
}