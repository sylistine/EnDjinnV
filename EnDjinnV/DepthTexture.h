#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"

namespace Djn::Gfx
{
    class DepthTexture
    {
    public:
        DepthTexture(const Device& device, uint32_t initialWidth, uint32_t initialHeight);
        ~DepthTexture();
        VkImageView GetView() const { return view; }
    private:
        VkDevice vkDevice;
        VkImage image;
        VkDeviceMemory memory;
        VkImageView view;
    };
}
