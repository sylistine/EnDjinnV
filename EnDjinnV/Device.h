#pragma once

#include <vulkan/vulkan.h>

#include "PhysicalDevice.h"

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
        Device(uint32_t gfxQueueFamilyIdx, PhysicalDevice physicalDevice);
        ~Device();
        Device(Device&& other)
        {
            this->FreeDeviceMemory();
            this->logicalDevice = other.logicalDevice;
            this->inited = true;
            other.inited = false;
        }
        Device& operator=(Device&& other)
        {
            if (this != &other) {
                this->FreeDeviceMemory();
                this->logicalDevice = other.logicalDevice;
                this->inited = true;
                other.inited = false;
            }
            return *this;
        }
        VkDevice GetLogical() const { return logicalDevice; }
        PhysicalDevice GetPhysical() const { return physicalDevice; }
        bool GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags flags, uint32_t& index) const;
    private:
        bool inited = false;
        VkDevice logicalDevice;
        PhysicalDevice physicalDevice;
        void FreeDeviceMemory();
    };
}
