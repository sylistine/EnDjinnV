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
        /// Full constructor.
        Device(PhysicalDevice physicalDevice);

        /// Destructor.
        ~Device();

        /// Copy constructor.
        Device(Device&& other) noexcept;

        /// Move assignment operator.
        Device& operator=(Device&& other) noexcept;

        /// Returns the logical vulkan device.
        vk::Device GetLogical() const { return logicalDevice; }

        /// Returns the physical device.
        vk::PhysicalDevice GetPhysical() const { return physicalDevice.Get(); }

        /// Gets the first index of a memory type that matches typebits and flags.
        bool GetMemoryTypeIndex(uint32_t typeBits, vk::MemoryPropertyFlags flags, uint32_t& index) const;

    private:
        bool inited = false;
        vk::Device logicalDevice;
        PhysicalDevice physicalDevice;
        void FreeDeviceMemory();
    };
}
