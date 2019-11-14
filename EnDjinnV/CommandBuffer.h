#pragma once

#include <vulkan/vulkan.h>

namespace Djn::Gfx
{
    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        CommandBuffer(VkDevice device, VkCommandPool cmdPool);
        ~CommandBuffer();
        CommandBuffer(CommandBuffer&& other)
        {
            this->FreeDeviceMemory();
            this->vkDevice = other.vkDevice;
            this->vkCmdPool = other.vkCmdPool;
            this->vkCommandBuffer = other.vkCommandBuffer;
            this->inited = true;
            other.inited = false;
        }
        CommandBuffer& operator=(CommandBuffer&& other)
        {
            if (this != &other) {
                this->FreeDeviceMemory();
                this->vkDevice = other.vkDevice;
                this->vkCmdPool = other.vkCmdPool;
                this->vkCommandBuffer = other.vkCommandBuffer;
                this->inited = true;
                other.inited = false;
            }
            return *this;
        }
        uint32_t Count() const { return count; }
        VkCommandBuffer Get() const { return vkCommandBuffer; }
    private:
        bool inited = false;
        VkDevice vkDevice;
        VkCommandPool vkCmdPool;
        uint32_t count;
        VkCommandBuffer vkCommandBuffer;
        void FreeDeviceMemory();
    };
}
