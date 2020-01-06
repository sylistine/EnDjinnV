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
        Buffer() {}
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
        Buffer(Buffer&& other) noexcept
        {
            FreeMemory();
            this->d = other.d;
            this->buffer = other.buffer;
            this->memory = other.memory;
            this->memoryAllocationSize = other.memoryAllocationSize;
            this->inited = true;
            other.inited = false;
        }
        Buffer& operator=(Buffer&& other) noexcept
        {
            if (this != &other) {
                FreeMemory();
                this->d = other.d;
                this->buffer = other.buffer;
                this->memory = other.memory;
                this->memoryAllocationSize = other.memoryAllocationSize;
                this->inited = true;
                other.inited = false;
            }
            return *this;
        }
        vk::Buffer Get() const { return buffer; }
        void updateData(void* data, vk::DeviceSize size);
    private:
        bool inited = false;
        vk::Device d;
        vk::Buffer buffer;
        vk::DeviceMemory memory;
        vk::DeviceSize memoryAllocationSize;
        void FreeMemory();
    };
}
