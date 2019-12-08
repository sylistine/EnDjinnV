#include "EnDjinnV.h"

#include "PhysicalDevice.h"


using namespace Djn::Gfx;


PhysicalDevice::PhysicalDevice(vk::PhysicalDevice dev, VkSurfaceKHR outputSurface) : device(dev), outputSurface(outputSurface)
{
    VkResult result;
    uint32_t tmpListSize;

    // Update queue family properties list.
    vkGetPhysicalDeviceQueueFamilyProperties(device, &tmpListSize, NULL);
    queueFamilyProperties.resize(tmpListSize);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &tmpListSize, queueFamilyProperties.data());

    // Update memory properties for device.
    vkGetPhysicalDeviceMemoryProperties(dev, &memoryProperties);

    // Get surface capabilities against chosen device
    gfxQueueFamilyIdx = UINT32_MAX;
    presentQueueFamilyIdx = UINT32_MAX;
    for (auto i = 0u; i < queueFamilyProperties.size(); i++) {
        VkBool32 supportsPresent;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, outputSurface, &supportsPresent);
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
        }
        gfxQueueFamilyIdx = i;
    }

    // Update surface capabilities 
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, outputSurface, &surfaceCapabilities);
    if (result != VK_SUCCESS) {
        throw Exception("Unable to determine surface capabilities on selected GPU.");
    }

    // Update present modes.
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, outputSurface, &tmpListSize, NULL);
    if (result != VK_SUCCESS) throw Exception("Unable to get physical device surface present modes.");
    presentModes.resize(tmpListSize);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, outputSurface, &tmpListSize, presentModes.data());
    if (result != VK_SUCCESS) throw Exception("Unable to get physical device surface present modes.");

    // Update supported output formats.
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, outputSurface, &tmpListSize, NULL);
    if (result != VK_SUCCESS) throw Exception("Unable to get physical device surface formats.");
    surfaceFormats.resize(tmpListSize);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, outputSurface, &tmpListSize, surfaceFormats.data());
    if (result != VK_SUCCESS) throw new std::exception("Unable to get physical device surface formats.");
    if (surfaceFormats.size() < 1) throw Exception("Unable to determine surface formats.");

    // We choose the 0th surface format as default.
    outputFormat = surfaceFormats[0].format;
    if (outputFormat == VK_FORMAT_UNDEFINED) {
        outputFormat = VK_FORMAT_B8G8R8A8_UNORM;
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
