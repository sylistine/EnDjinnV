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
        static void Initialize(vk::Instance vkInstance, vk::SurfaceKHR surface);
        static void SetViewProjectionMatrices(mat4 viewMatrix, mat4 projectionMatrix);
        static void SetVertices(std::vector<Vertex> vertices);
    private:
        static Manager* gfxInstance;
        Manager(vk::Instance vkInstance, vk::SurfaceKHR surface);
        ~Manager();

        vk::Instance instance;
        // Physical device data.
        PhysicalDevice primaryGPU;
        // Logical device data.
        Device device;
        CommandPool gfxCommandPool;
        Swapchain swapchain;
        DepthTexture* depthTexture;
        vk::ShaderModule vertexShaderModule;
        vk::ShaderModule fragmentShaderModule;

        vk::RenderPass primaryRenderPass; // Renderpass responsible for drawing and presenting to the surface.
        vk::Framebuffer* primaryFramebuffer; // Framebuffer for the primary render pass: length should equal swapchain count.
        vk::Buffer vertexBuffer;
        vk::Buffer mvpBuffer;

        std::vector<unsigned int> CompileShader(
            shaderc::Compiler& compiler,
            Shader shader);
        void SetupPrimaryRenderPass();
        void TeardownPrimaryRenderPass();
        void SetPrimaryVertexBuffer(std::vector<Vertex> vertices);
        Buffer CreateUniformBuffer(void* data, size_t size);
    };
}