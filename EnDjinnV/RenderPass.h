#pragma once

#include <vulkan/vulkan.h>

namespace Djn::Gfx
{
    class RenderPass
    {
    public:
        RenderPass(VkDevice device, VkRenderPass renderPass);
        ~RenderPass();
        // Copy
        RenderPass(const RenderPass& other) = delete;
        RenderPass& operator=(const RenderPass& other) = delete;
        // Move
        RenderPass(RenderPass&& other) = delete;
        RenderPass& operator=(RenderPass&& other) = delete;
    private:
        VkDevice device;
        VkRenderPass renderPass;
    };
}
