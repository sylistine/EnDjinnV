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
    private:
        VkDevice vkDevice;
        VkBuffer buffer;
        VkDeviceMemory memory;
        VkMemoryAllocateInfo memoryInfo;
    };
}
