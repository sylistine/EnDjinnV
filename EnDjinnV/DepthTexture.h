#pragma once

#include <vulkan/vulkan.h>

namespace Djn
{
    class DepthTexture
    {
    public:
        DepthTexture(
            VkDevice device,
            uint32_t initialWidth,
            uint32_t initialHeight,
            VkPhysicalDeviceMemoryProperties memoryTypeIdx);
        ~DepthTexture();
        VkImageView GetView() const { return view; }
    private:
        VkDevice device;
        VkImage image;
        VkDeviceMemory memory;
        VkImageView view;
    };
}
