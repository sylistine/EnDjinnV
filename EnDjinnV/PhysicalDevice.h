#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "VulkanUtil.h"

namespace Djn::Gfx
{
    class PhysicalDevice
    {
    public:
        PhysicalDevice() {}
        PhysicalDevice(VkPhysicalDevice dev) : physicalDevice(dev)
        {
            queueFamilyProperties = GetQueueFamilyProperties();
            vkGetPhysicalDeviceMemoryProperties(dev, &memoryProperties);
        }

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
        // Returns the queue family indices that support graphics and present.
        void GetGfxAndPresentQueueFamilyIndicies(VkSurfaceKHR surface, uint32_t& gfxQueueFamilyIdx, uint32_t& presentQueueFamilyIdx) const;
        bool GetSurfaceCapabilities(VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR& capabilities) const;
        std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties() const;
        std::vector<VkPresentModeKHR> GetSurfacePresentModes(VkSurfaceKHR surface) const;
        std::vector<VkSurfaceFormatKHR> GetSurfaceFormats(VkSurfaceKHR surface) const;
    private:
        VkPhysicalDevice physicalDevice;
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        VkPhysicalDeviceMemoryProperties memoryProperties;
    };
}
