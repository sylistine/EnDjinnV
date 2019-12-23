#pragma once

#include <vulkan/vulkan.hpp>

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
        static void SetViewProjectionMatrices(mat4 mvp);
        static void SetVertices(std::vector<Vertex> vertices);
        static void TempBuildAndRunPipeline();
    private:
        static Manager* gfxInstance;
        Manager(vk::Instance vkInstance, vk::SurfaceKHR surface);
        ~Manager();

        vk::Instance instance;
        PhysicalDevice primaryGPU;
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
        uint32_t vertexCount;
        Buffer vertexBuffer; // buffer for just the one set of verts.
        vk::VertexInputBindingDescription viBindingDesc;
        Buffer viewProjectionBuffer; // buffer for the main camera view/projection matrices.
        vk::DescriptorSetLayout primaryDescriptorSetLayout;
        vk::DescriptorPool primaryDescriptorPool;
        vk::DescriptorSet primaryDescriptorSet;
        vk::PipelineLayout primaryPipelineLayout;
        vk::Pipeline primaryPipeline;

        // Creates render pass, frame buffers, and descriptor sets.
        void SetupPrimaryRenderPass();
        void TeardownPrimaryRenderPass();
        // Updates the VBO.
        void SetPrimaryVertexBuffer(std::vector<Vertex> vertices);
        // Updates the view/projection buffer. Requires SetupPrimaryRenderPass to be called.
        void SetPrimaryViewProjectionMatrices(mat4 mvp);

        // Constructs a render pipeline. Requires renderpass, framebuffer...
        void TempPipelineStuff();
        void TempCommandBuffer();
    };
}