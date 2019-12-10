#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

namespace Djn::Gfx
{
    class CommandPool
    {
    public:
        CommandPool() = default;
        CommandPool(vk::Device device, uint32_t queueFamilyIndex, uint32_t initialBufferCount = 1);
        ~CommandPool();
        CommandPool(CommandPool&& other) noexcept
        {
            this->vkDevice = other.vkDevice;
            this->pool = other.pool;
            this->buffers = other.buffers;
            this->inited = true;
            other.inited = false;
        }
        CommandPool& operator=(CommandPool&& other) noexcept
        {
            if (this != &other) {
                this->FreeDeviceMemory();
                this->vkDevice = other.vkDevice;
                this->pool = other.pool;
                this->buffers = other.buffers;
                this->inited = true;
                other.inited = false;
            }
            return *this;
        }
        vk::CommandPool Get() const { return pool; }
        size_t BufferCount() const { return buffers.size(); }
        vk::CommandBuffer operator[](size_t idx)
        {
            if (idx < 0 || idx >= BufferCount()) {
                throw Exception("Argument out of range exception.");
            }
            return buffers[idx];
        }
        void CreateCommandBuffers(uint32_t count = 1);
    private:
        bool inited = false;
        vk::Device vkDevice;
        vk::CommandPool pool;
        std::vector<vk::CommandBuffer> buffers;
        void FreeBuffers();
        void FreeDeviceMemory() noexcept;
    };
}
