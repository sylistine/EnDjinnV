#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Djn::VkUtil
{
    inline std::vector<VkPhysicalDevice> GetPhysicalDevices(const VkInstance& instance)
    {
        uint32_t count;
        VkResult result = vkEnumeratePhysicalDevices(instance, &count, NULL);
        if (result != VK_SUCCESS) throw new std::exception("Unable to enumerate physical devices.");
        std::vector<VkPhysicalDevice> list(count);
        result = vkEnumeratePhysicalDevices(instance, &count, list.data());
        if (result != VK_SUCCESS) throw new std::exception("Unable to enumerate physical devices.");
        return list;
    }


    inline VkPhysicalDevice GetDefaultPhysicalDevice(const VkInstance& instance)
    {
        return GetPhysicalDevices(instance)[0];
    }


    inline std::vector<VkQueueFamilyProperties> GetPhysicalDeviceQueueFamilyProperties(const VkPhysicalDevice& device)
    {
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, NULL);
        std::vector<VkQueueFamilyProperties> list(count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &count, list.data());
        return list;
    }

    inline void GetGfxAndPresentQueueFamilyIndex(
        VkPhysicalDevice device,
        std::vector<VkQueueFamilyProperties> qfp,
        VkSurfaceKHR surface,
        uint32_t& gfxQueueFamilyIdx,
        uint32_t& presentQueueFamilyIdx)
    {
        gfxQueueFamilyIdx = UINT32_MAX;
        presentQueueFamilyIdx = UINT32_MAX;
        for (auto i = 0u; i < qfp.size(); i++) {
            VkBool32 supportsPresent;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supportsPresent);
            auto graphicsFlag = qfp[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

            if (graphicsFlag != 0 && supportsPresent == VK_TRUE) {
                gfxQueueFamilyIdx = i;
                presentQueueFamilyIdx = i;
                break;
            } else {
                if (gfxQueueFamilyIdx == UINT32_MAX && graphicsFlag != 0) {
                    gfxQueueFamilyIdx = i;
                }
                if (presentQueueFamilyIdx == UINT32_MAX && supportsPresent == VK_TRUE) {
                    presentQueueFamilyIdx = i;
                }
            }
        }
    }


    inline std::vector<VkPresentModeKHR> GetPhysicalDeviceSurfacePresentModes(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        uint32_t count;
        VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, NULL);
        if (result != VK_SUCCESS) throw new std::exception("Unable to get physical device surface present modes.");
        std::vector<VkPresentModeKHR> list(count);
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, list.data());
        if (result != VK_SUCCESS) throw new std::exception("Unable to get physical device surface present modes.");
        return list;
    }


    inline std::vector<VkSurfaceFormatKHR> GetPhysicalDeviceSurfaceFormats(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        uint32_t count;
        VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, NULL);
        if (result != VK_SUCCESS) throw new std::exception("Unable to get physical device surface formats.");
        std::vector<VkSurfaceFormatKHR> list(count);
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &count, list.data());
        if (result != VK_SUCCESS) throw new std::exception("Unable to get physical device surface formats.");
        return list;
    }


    inline std::vector<VkImage> GetSwapchainImages(VkDevice device, VkSwapchainKHR swapchain)
    {
        uint32_t count;
        VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &count, NULL);
        if (result != VK_SUCCESS) throw std::exception("Unable to get swapchain image count.");
        std::vector<VkImage> list(count);
        result = vkGetSwapchainImagesKHR(device, swapchain, &count, list.data());
        if (result != VK_SUCCESS) throw std::exception("Unable to get swapchain images.");
        return list;
    }


    /*
     * Vulkan Initialization structures.
     */
    static VkDeviceQueueCreateInfo DeviceQueueCreateInfo(
        const void* pNext = NULL,
        VkDeviceQueueCreateFlags flags = 0)
    {
        VkDeviceQueueCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info.pNext = pNext;
        info.flags = flags;
        return info;
    }


    static VkDeviceCreateInfo DeviceCreateInfo(
        const void* pNext = NULL,
        VkDeviceCreateFlags flags = 0)
    {
        VkDeviceCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        info.pNext = pNext;
        info.flags = flags;
        return info;
    }


    static VkSwapchainCreateInfoKHR SwapChainCreateInfo(
        const void* pNext = NULL,
        VkSwapchainCreateFlagsKHR flags = 0)
    {
        VkSwapchainCreateInfoKHR info = {};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.pNext = pNext;
        info.flags = flags;
        return info;
    }


    static VkImageCreateInfo ImageCreateInfo(
        const void* pNext = NULL)
    {
        VkImageCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.pNext = pNext;
        return info;
    }


    static VkMemoryAllocateInfo MemoryAllocateInfo(
        const void* pNext = NULL)
    {
        VkMemoryAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        info.pNext = pNext;
        return info;
    }


    static VkImageViewCreateInfo ImageViewCreateInfo(
        const void* pNext = NULL,
        VkImageViewCreateFlags flags = 0)
    {
        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = pNext;
        info.flags = flags;
        return info;
    }


    /*
     * Command buffer structures.
     */
    static VkCommandPoolCreateInfo CommandPoolCreateInfo(
        const void* pNext = NULL,
        VkCommandPoolCreateFlags flags = 0)
    {
        VkCommandPoolCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        info.pNext = pNext;
        info.flags = flags;
        return info;
    }


    static VkCommandBufferAllocateInfo CommandBufferAllocateInfo(
        const void* pNext = NULL)
    {
        VkCommandBufferAllocateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext = pNext;
        return info;
    }


    static VkCommandBufferInheritanceInfo CommandBufferInheritanceInfo(
        const void* pNext = NULL)
    {
        VkCommandBufferInheritanceInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        info.pNext = pNext;
        return info;
    }


    static VkCommandBufferBeginInfo CmdBufferBeginInfo(
        const void* pNext = NULL,
        VkCommandBufferUsageFlags flags = 0)
    {
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.pNext = NULL;
        info.flags = 0;
        return info;
    }
}
