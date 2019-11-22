#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "VulkanUtil.h"

namespace Djn::Gfx
{
    class PhysicalDevice
    {
    public:
        // ctors
        PhysicalDevice() {}
        PhysicalDevice(VkPhysicalDevice dev) : physicalDevice(dev)
        {
            queueFamilyProperties = VkUtil::GetPhysicalDeviceQueueFamilyProperties(dev);
            vkGetPhysicalDeviceMemoryProperties(dev, &memoryProperties);
        }

        // accessors
        VkPhysicalDevice Get() const { return physicalDevice; }
        size_t QueueFamilyPropertyCount() const { return queueFamilyProperties.size(); }
        VkQueueFamilyProperties QueueFamilyProperties(uint32_t idx) const
        {
            if (idx < 0 || idx >= queueFamilyProperties.size()) throw std::exception("Argument out of range exception.");
            return queueFamilyProperties[idx];
        }
        VkPhysicalDeviceMemoryProperties MemoryProperties() const { return memoryProperties; }

        // utility
        void GetGfxAndPresentQueueFamilyIndicies(VkSurfaceKHR surface, uint32_t& gfxQueueFamilyIdx, uint32_t& presentQueueFamilyIdx) const;
        bool GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags flags, uint32_t& index) const;
    private:
        VkPhysicalDevice physicalDevice;
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        VkPhysicalDeviceMemoryProperties memoryProperties;
    };
}
