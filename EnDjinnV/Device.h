#pragma once

#include <vulkan/vulkan.h>

namespace Djn::Gfx
{
    /// Device and other graphics objects own Gpu allocations.
    /// When destroyed, they are required to free anything they own.
    /// On the rhs of a move, they relenquish ownership without freeing.
    /// On the lhs of a move, they free any owned memory before receiving new ownership.
    class Device
    {
    public:
        Device() = default;
        Device(uint32_t gfxQueueFamilyIdx, VkPhysicalDevice physicalDevice);
        ~Device();
        Device(Device&& other)
        {
            this->FreeDeviceMemory();
            this->vkDevice = other.vkDevice;
            this->inited = true;
            other.inited = false;
        }
        Device& operator=(Device&& other)
        {
            if (this != &other) {
                this->FreeDeviceMemory();
                this->vkDevice = other.vkDevice;
                this->inited = true;
                other.inited = false;
            }
            return *this;
        }
        VkDevice Get() const { return vkDevice; }
    private:
        bool inited = false;
        VkDevice vkDevice;
        void FreeDeviceMemory();
    };
}
