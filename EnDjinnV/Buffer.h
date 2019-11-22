#pragma once

#include <vulkan/vulkan.h>

#include "VulkanUtil.h"
#include "Device.h"
#include "Vertex.h"

namespace Djn::Gfx
{
    class Buffer
    {
    public:
        ~Buffer();
    protected:
        Buffer();
        VkDevice vkDevice;
        VkBuffer buffer;
        VkDeviceMemory memory;
    };

    class VertexBuffer : public Buffer
    {
    public:
        VertexBuffer(const Device& device, std::vector<Vertex> vertex);
        ~VertexBuffer();
    };
}
