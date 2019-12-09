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
            vk::BufferUsageFlags usage,
            void* data,
            VkDeviceSize size);
        ~Buffer();
        // copy handlers
        Buffer(const Buffer& other) = delete;
        Buffer& operator=(const Buffer& other) = delete;
        // move handlers
        Buffer(Buffer&& other) = delete;
        Buffer& operator=(Buffer&& other) = delete;
    private:
        vk::Device d;
        vk::Buffer buffer;
        vk::DeviceMemory memory;
        vk::MemoryAllocateInfo memoryInfo;
    };
}
