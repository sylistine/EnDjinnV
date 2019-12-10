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
        std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStages;

        // Primary render pass steps.
        vk::RenderPass primaryRenderPass; // draws to the main output surface.
        vk::Framebuffer* primaryFramebuffer; // framebuffers for primary swapchain.
        vk::Buffer vertexBuffer; // buffer for just the one set of verts.
        vk::VertexInputBindingDescription viBindingDesc;
        vk::Buffer viewProjectionBuffer; // buffer for the main camera view/projection matrices.
        vk::DescriptorSetLayout primaryDescriptorSetLayout;
        vk::Pipeline primaryPipeline;

        std::vector<unsigned int> CompileShader(
            shaderc::Compiler& compiler,
            Shader shader);
        void SetupPrimaryRenderPass();
        void TeardownPrimaryRenderPass();
        void SetPrimaryVertexBuffer(std::vector<Vertex> vertices);
        void SetPrimaryViewProjectionMatrices(mat4 viewMatrix, mat4 projectionMatrix);
        void TempPipelineStuff();
    };
}