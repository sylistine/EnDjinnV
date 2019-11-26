#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace Djn::Gfx
{
    class CommandPool
    {
    public:
        CommandPool() = default;
        CommandPool(VkDevice device, uint32_t queueFamilyIndex, uint32_t initialBufferCount = 0);
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
        VkCommandPool Get() const { return pool; }
        size_t BufferCount() const { return buffers.size(); }
        VkCommandBuffer operator[](size_t idx)
        {
            if (idx < 0 || idx >= BufferCount()) {
                throw std::exception("Argument out of range exception.");
            }
            return buffers[idx];
        }
        VkCommandBuffer CreateCommandBuffer(uint32_t count = 1);
    private:
        bool inited = false;
        VkDevice vkDevice;
        VkCommandPool pool;
        std::vector<VkCommandBuffer> buffers;
        void FreeDeviceMemory() noexcept;
    };
}
