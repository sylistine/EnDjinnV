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
        ~Buffer();
        Buffer(const Buffer& other) = delete;
        Buffer(Buffer&& other) = delete;
        Buffer& operator=(const Buffer& other) = delete;
        Buffer& operator=(Buffer&& other) = delete;
    private:
        VkDevice vkDevice;
        VkBuffer buffer;
        VkDeviceMemory memory;
        VkMemoryAllocateInfo memoryInfo;
    };
}
