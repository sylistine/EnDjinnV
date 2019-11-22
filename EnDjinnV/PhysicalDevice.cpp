#include "EnDjinnV.h"

#include "PhysicalDevice.h"

using namespace Djn::Gfx;

void PhysicalDevice::GetGfxAndPresentQueueFamilyIndicies(VkSurfaceKHR surface, uint32_t & gfxQueueFamilyIdx, uint32_t & presentQueueFamilyIdx) const
{
    gfxQueueFamilyIdx = UINT32_MAX;
    presentQueueFamilyIdx = UINT32_MAX;
    for (auto i = 0u; i < queueFamilyProperties.size(); i++) {
        VkBool32 supportsPresent;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresent);
        auto graphicsFlag = queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

        if (graphicsFlag != 0 && supportsPresent == VK_TRUE) {
            gfxQueueFamilyIdx = i;
            presentQueueFamilyIdx = i;
            break;
        } else {
            if (gfxQueueFamilyIdx == UINT32_MAX && graphicsFlag != 0) {

                if (presentQueueFamilyIdx == UINT32_MAX && supportsPresent == VK_TRUE) {
                    presentQueueFamilyIdx = i;
                }
            }
        } gfxQueueFamilyIdx = i;
    }
}


bool PhysicalDevice::GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags flags, uint32_t& index) const
{
    for (auto i = 0u; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            if ((memoryProperties.memoryTypes[i].propertyFlags & flags) == flags) {
                index = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    return false;
}


bool PhysicalDevice::GetSurfaceCapabilities(VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR& capabilities) const
{
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
    if (result != VK_SUCCESS) return false;
    return true;
}


std::vector<VkQueueFamilyProperties> PhysicalDevice::GetQueueFamilyProperties() const
{
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, NULL);
    std::vector<VkQueueFamilyProperties> list(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, list.data());
    return list;
}


std::vector<VkPresentModeKHR> PhysicalDevice::GetSurfacePresentModes(VkSurfaceKHR surface) const
{
    uint32_t count;
    VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, NULL);
    if (result != VK_SUCCESS) throw std::exception("Unable to get physical device surface present modes.");
    std::vector<VkPresentModeKHR> list(count);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, list.data());
    if (result != VK_SUCCESS) throw std::exception("Unable to get physical device surface present modes.");
    return list;
}


std::vector<VkSurfaceFormatKHR> PhysicalDevice::GetSurfaceFormats(VkSurfaceKHR surface) const
{
    uint32_t count;
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, NULL);
    if (result != VK_SUCCESS) throw std::exception("Unable to get physical device surface formats.");
    std::vector<VkSurfaceFormatKHR> list(count);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, list.data());
    if (result != VK_SUCCESS) throw new std::exception("Unable to get physical device surface formats.");
    return list;
}
