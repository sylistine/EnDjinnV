#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "VulkanUtil.h"

namespace Djn::Gfx
{
    class PhysicalDevice
    {
    public:
        PhysicalDevice(VkPhysicalDevice dev, VkSurfaceKHR outputSurface);

    public:
        // Get the internal VkPhysicalDevice.
        VkPhysicalDevice Get() const { return physicalDevice; }

        // Get the number of queue families available on this physical device.
        size_t QueueFamilyPropertyCount() const { return queueFamilyProperties.size(); }

        // Get the i-th queue family's properties.
        VkQueueFamilyProperties QueueFamilyProperties(uint32_t idx) const
        {
            if (idx < 0 || idx >= queueFamilyProperties.size()) throw std::exception("Argument out of range exception.");
            return queueFamilyProperties[idx];
        }

        // Get memory properties for the device.
        VkPhysicalDeviceMemoryProperties MemoryProperties() const { return memoryProperties; }

        // Get the index of the memory type that supports `flags`.
        // `typeBits`, a value returned from the `vkGet*MemoryRequirements` class of functions, determines what indices can be tested.
        // Returns false if no valid index was found.
        bool GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags flags, uint32_t& index) const;

        // Get's the capabilities for `surface` against this physical device.
        // Returns false if capabilities couldn't be retrieved.
        VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() const { return surfaceCapabilities; }

        // Returns a list of all Queue Family Properties for this device.
        std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties() const;
        
        // Returns a list of available surface persent modes.
        std::vector<VkPresentModeKHR> GetSurfacePresentModes() const;

        // Returns a list of available surface formats.
        std::vector<VkSurfaceFormatKHR> GetSurfaceFormats() const;

        // Returns the graphics queue family index.
        uint32_t GetGraphicsQueueFamilyIndex() const { return gfxQueueFamilyIdx; }

        // Returns the present queue family index.
        uint32_t GetPresentQueueFamilyIndex() const { return presentQueueFamilyIdx; }
    private:
        VkPhysicalDevice physicalDevice;
        VkSurfaceKHR outputSurface;
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        VkPhysicalDeviceMemoryProperties memoryProperties;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        uint32_t gfxQueueFamilyIdx;
        uint32_t presentQueueFamilyIdx;
    };
}
