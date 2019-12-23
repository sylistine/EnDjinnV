#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"

namespace Djn::Gfx
{
    class DepthTexture
    {
    public:
        DepthTexture(const Device& device, vk::Extent2D extents) :
            DepthTexture(device, extents.width, extents.height) {}
        DepthTexture(const Device& device, uint32_t width, uint32_t height);
        ~DepthTexture();
        vk::ImageView GetView() const { return view; }
    private:
        vk::Device vkDevice;
        vk::Image image;
        vk::DeviceMemory memory;
        vk::ImageView view;
    };
}
