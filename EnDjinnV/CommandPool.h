#pragma once

#include <vulkan/vulkan.h>

namespace Djn::Gfx
{
    class CommandPool
    {
    public:
        CommandPool() = default;
        CommandPool(VkDevice device, uint32_t gfxQueueFamilyIdx);
        ~CommandPool();
        CommandPool(CommandPool&& other)
        {
            this->FreeDeviceMemory();
            this->vkDevice = other.vkDevice;
            this->vkCommandPool = other.vkCommandPool;
            this->inited = true;
            other.inited = false;
        }
        CommandPool& operator=(CommandPool&& other)
        {
            if (this != &other) {
                this->FreeDeviceMemory();
                this->vkDevice = other.vkDevice;
                this->vkCommandPool = other.vkCommandPool;
                this->inited = true;
                other.inited = false;
            }
            return *this;
        }
        VkCommandPool Get() const { return vkCommandPool; }
    private:
        bool inited = false;
        VkDevice vkDevice;
        VkCommandPool vkCommandPool;
        void FreeDeviceMemory();
    };
}
