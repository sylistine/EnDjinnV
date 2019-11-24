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
        Buffer(
            const Device& device,
            VkBufferUsageFlags usage,
            void* data,
            VkDeviceSize size);
        virtual ~Buffer();
    protected:
        VkDevice vkDevice;
        VkBuffer buffer;
        VkDeviceMemory memory;
        VkMemoryAllocateInfo memoryInfo;
    };

    class VertexBuffer : public Buffer
    {
    public:
        VertexBuffer(const Device& device, std::vector<Vertex> vertex);
        ~VertexBuffer();
    };

    class UniformBuffer : public Buffer
    {
    public:
        UniformBuffer(const Device& device, void* data, VkDeviceSize size);
        ~UniformBuffer();
    };
}
