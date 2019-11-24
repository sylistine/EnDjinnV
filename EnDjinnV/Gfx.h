#pragma once

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

#include "VulkanUtil.h"
#include "Shader.h"

#include "PhysicalDevice.h"
#include "Device.h"
#include "CommandPool.h"
#include "CommandBuffer.h"
#include "SwapChain.h"
#include "DepthTexture.h"
#include "Buffer.h"

namespace Djn::Gfx
{
    class Manager
    {
    public:
        static void Initialize(VkInstance vkInstance, VkSurfaceKHR surface);
    private:
        static Manager* gfxInstance;
        Manager(VkInstance vkInstance, VkSurfaceKHR surface);
        ~Manager();

        VkInstance instance;
        // Physical device data.
        PhysicalDevice primaryGPU;
        // Logical device data.
        Device device;
        CommandPool cmdPool;
        CommandBuffer cmdBuffer;
        Swapchain swapchain;
        DepthTexture* depthTexture;
        VkSemaphore imageAcquiredSemaphore;
        VkRenderPass renderPass;
        VkShaderModule vertexShaderModule;
        VkShaderModule fragmentShaderModule;
        VkFramebuffer* framebuffer;

        std::vector<unsigned int> CompileShader(
            shaderc::Compiler& compiler,
            Shader shader);
    };
}