#pragma once

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "VulkanUtil.h"
#include "Shader.h"

#include "Device.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "SwapChain.h"
#include "DepthTexture.h"

namespace Djn::Gfx
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

    class Manager
    {
    public:
        static void Initialize(VkInstance vkInstance, VkSurfaceKHR surface);
    private:
        static Manager* gfxInstance;
        Manager(VkInstance vkInstance, VkSurfaceKHR surface);
        ~Manager();

        VkInstance instance;
        VkSurfaceKHR surface;
        // Physical device data.
        PhysicalDevice primaryGPU;
        // Logical device data.
        Device device;
        CommandPool cmdPool;
        CommandBuffer cmdBuffer;
        Swapchain swapchain;
        VkImage depthImage;
        DepthTexture* depthTexture;
        VkRenderPass renderPass;
        VkShaderModule vertexShaderModule;
        VkShaderModule fragmentShaderModule;

        std::vector<unsigned int> CompileShader(
            shaderc::Compiler& compiler,
            Shader shader);
    };
}