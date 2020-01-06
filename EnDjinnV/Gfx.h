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
    typedef struct MVPBuffer
    {
        mat4 clip;
        mat4 proj;
        mat4 view;
        mat4 model;
        mat4 mvp;
    } MVPBuffer;

    static VkInstance CreateVulkanInstance(const char* appName)
    {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName;
        appInfo.applicationVersion = 1;
        appInfo.pEngineName = appName;
        appInfo.engineVersion = 1;
        appInfo.apiVersion = VK_API_VERSION_1_0;

        std::vector<const char*> extensions;
        std::vector<const char*> layers;

        extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
        extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef _DEBUG
        //std::cout << "Available extensions: " << std::endl;
        //VkUtil::PrintInstanceExtensions();
        //std::cout << "Available layers: " << std::endl;
        //VkUtil::PrintInstanceLayers();

        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        layers.push_back(VkUtil::VK_LAYER_FULL_VALIDATION);
        layers.push_back(VkUtil::VK_LAYER_RENDERDOC_CAPTURE);
#endif

    // Validate requested layers.
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, NULL);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (auto& layer : layers) {
            bool foundLayer = false;
            for (auto& availableLayer : availableLayers) {
                if (strcmp(availableLayer.layerName, layer) == 0) {
                    foundLayer = true;
                    break;
                }
            }
            if (!foundLayer) {
                std::cout << "Unable to find required layer " << layer << std::endl;
                throw Exception("Unable to find required vulkan layer.");
            }
        }

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
        createInfo.ppEnabledLayerNames = layers.data();

        VkInstance instance;
        VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
        if (result != VK_SUCCESS) throw Exception("Unable to create vulkan instance.");

        return instance;
    }

    class Manager
    {
    public:
        static Manager* Initialize(vk::Instance vkInstance, vk::SurfaceKHR surface);
        static void Terminate();
        static Manager* GetInstance();
    private:
        static Manager* instance;
        Manager(vk::Instance vkInstance, vk::SurfaceKHR surface);
        ~Manager();

        vk::Instance vulkan;
        PhysicalDevice primaryGPU;
        Device device;
        CommandPool gfxCommandPool;
        Swapchain swapchain;
        DepthTexture* depthTexture;
        vk::ShaderModule vertexShaderModule;
        vk::ShaderModule fragmentShaderModule;
        std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStages;

        // Primary render pass steps.
        float timeData;
        Buffer timeBuffer;
        vk::RenderPass primaryRenderPass; // draws to the main output surface.
        vk::Framebuffer* primaryFramebuffer; // framebuffers for primary swapchain.
        uint32_t vertexCount;
        Buffer vertexBuffer; // buffer for just the one set of verts.
        vk::VertexInputBindingDescription viBindingDesc;
        MVPBuffer viewProjectionData;
        Buffer viewProjectionBuffer; // buffer for the main camera view/projection matrices.
        vk::DescriptorPool primaryDescriptorPool;
        vk::DescriptorSet primaryDescriptorSet;
        vk::DescriptorSetLayout primaryDescriptorSetLayout;
        vk::PipelineLayout primaryPipelineLayout;
        vk::PipelineCache pipelineCache;
        vk::Pipeline primaryPipeline;
    public:
        // Creates render pass, frame buffers, and descriptor sets.
        void setupPrimaryRenderPass();
        // Updates the VBO.
        void setPrimaryVertexBuffer(std::vector<Vertex> vertices);
        // Updates the view/projection buffer. Requires SetupPrimaryRenderPass to be called.
        void tempSetCameraParameters(float fovy, float nearClip, float farClip, vec3 pos, quat rot);
        void resize();
        // Constructs a render pipeline. Requires renderpass, framebuffer...
        void tempPipelineStuff();
        void draw(float deltaTime);
    private:
        void updateTimeBuffer(float deltaTime);
    };
}