#pragma once

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

#include "VulkanUtil.h"
#include "Shader.h"

#include "PhysicalDevice.h"
#include "Device.h"
#include "CommandPool.h"
#include "SwapChain.h"
#include "DepthTexture.h"
#include "Buffer.h"
#include "RenderPass.h"

namespace Djn::Gfx
{
    class Manager
    {
    public:
        static void Initialize(VkInstance vkInstance, VkSurfaceKHR surface);
        static void SetMVP(mat4 mvp);
    private:
        static Manager* gfxInstance;
        Manager(VkInstance vkInstance, VkSurfaceKHR surface);
        ~Manager();

        vk::Instance instance;
        // Physical device data.
        PhysicalDevice primaryGPU;
        // Logical device data.
        Device device;
        CommandPool gfxCommandPool;
        Swapchain swapchain;
        DepthTexture* depthTexture;
        VkShaderModule vertexShaderModule;
        VkShaderModule fragmentShaderModule;

        VkRenderPass primaryRenderPass; // Renderpass responsible for drawing and presenting to the surface.
        VkFramebuffer* primaryFramebuffer; // Framebuffer for the primary render pass: length should equal swapchain count.

        std::vector<unsigned int> CompileShader(
            shaderc::Compiler& compiler,
            Shader shader);
        void SetupPrimaryRenderPass();
        Buffer CreateUniformBuffer(void* data, size_t size);
    };
}