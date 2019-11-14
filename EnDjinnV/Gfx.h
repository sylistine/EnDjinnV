#pragma once

#include <vulkan/vulkan.h>
#include <shaderc/shaderc.hpp>

#include "VulkanUtil.h"
#include "Shader.h"

#include "DepthTexture.h"

namespace Djn::Gfx
{
    class Device
    {
    public:
        Device() = default;
        Device(uint32_t gfxQueueFamilyIdx, VkPhysicalDevice physicalDevice);
        ~Device();
        VkDevice Get() const { return vkDevice; }
    private:
        bool inited = false;
        VkDevice vkDevice;
    };

    class CommandPool
    {
    public:
        CommandPool() = default;
        CommandPool(VkDevice device, uint32_t gfxQueueFamilyIdx);
        ~CommandPool();
        VkCommandPool Get() const { return vkCommandPool; }
    private:
        bool inited = false;
        VkDevice vkDevice;
        VkCommandPool vkCommandPool;
    };

    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        CommandBuffer(VkDevice device, VkCommandPool cmdPool);
        ~CommandBuffer();
        uint32_t Count() const { return count; }
        VkCommandBuffer Get() const { return vkCommandBuffer; }
    private:
        bool inited = false;
        VkDevice vkDevice;
        VkCommandPool vkCmdPool;
        uint32_t count;
        VkCommandBuffer vkCommandBuffer;
    };

    class Swapchain
    {
    public:
        Swapchain() = default;
        Swapchain(VkDevice device, VkSurfaceKHR surface, VkFormat format,
            VkSurfaceCapabilitiesKHR surfaceCapabilities,
            std::vector<uint32_t> queueFamilyIndices);
        ~Swapchain();
        VkSwapchainKHR Get() const { return vkSwapchain; }
    private:
        bool inited = false;
        VkDevice vkDevice;
        VkSwapchainKHR vkSwapchain;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
    };

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