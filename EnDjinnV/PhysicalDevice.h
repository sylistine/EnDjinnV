#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "VulkanUtil.h"

namespace Djn::Gfx
{
    class PhysicalDevice
    {
    public:
        PhysicalDevice(vk::PhysicalDevice dev, VkSurfaceKHR outputSurface);

    public:
        // Get the internal VkPhysicalDevice.
        vk::PhysicalDevice Get() const { return device; }

        // Get the number of queue families available on this physical device.
        size_t QueueFamilyPropertyCount() const { return queueFamilyProperties.size(); }

        // Get the i-th queue family's properties.
        vk::QueueFamilyProperties QueueFamilyProperties(uint32_t idx) const
        {
            if (idx < 0 || idx >= queueFamilyProperties.size()) throw Exception("Argument out of range exception.");
            return queueFamilyProperties[idx];
        }

        // Get memory properties for the device.
        vk::PhysicalDeviceMemoryProperties MemoryProperties() const { return memoryProperties; }

        // Get the index of the memory type that supports `flags`.
        // `typeBits`, a value returned from the `vkGet*MemoryRequirements` class of functions, determines what indices can be tested.
        // Returns false if no valid index was found.
        bool GetMemoryTypeIndex(uint32_t typeBits, vk::MemoryPropertyFlags flags, uint32_t& index) const;

        // Get's the capabilities for `surface` against this physical device.
        // Returns false if capabilities couldn't be retrieved.
        vk::SurfaceCapabilitiesKHR GetSurfaceCapabilities() const { return surfaceCapabilities; }

        // Returns the graphics queue family index.
        uint32_t GetGraphicsQueueFamilyIndex() const { return gfxQueueFamilyIdx; }

        // Returns the present queue family index.
        uint32_t GetPresentQueueFamilyIndex() const { return presentQueueFamilyIdx; }

        vk::Format GetOutputFormat() const { return outputFormat; }
    private:
        vk::PhysicalDevice device;
        vk::SurfaceKHR outputSurface;
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
        vk::PhysicalDeviceMemoryProperties memoryProperties;
        uint32_t gfxQueueFamilyIdx;
        uint32_t presentQueueFamilyIdx;
        vk::SurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<vk::PresentModeKHR> presentModes;
        std::vector<vk::SurfaceFormatKHR> surfaceFormats;
        vk::Format outputFormat;
    };
}
