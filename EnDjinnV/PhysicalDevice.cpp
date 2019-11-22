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
