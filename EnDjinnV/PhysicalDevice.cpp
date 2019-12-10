#include "EnDjinnV.h"

#include "PhysicalDevice.h"


using namespace Djn::Gfx;


PhysicalDevice::PhysicalDevice(vk::PhysicalDevice dev, VkSurfaceKHR outputSurface) : device(dev), outputSurface(outputSurface)
{
    vk::Result result;

    // Update queue family properties list.
    queueFamilyProperties = device.getQueueFamilyProperties();

    // Update memory properties for device.
    device.getMemoryProperties(&memoryProperties);

    // Get surface capabilities against chosen device
    gfxQueueFamilyIdx = UINT32_MAX;
    presentQueueFamilyIdx = UINT32_MAX;
    for (auto i = 0u; i < queueFamilyProperties.size(); i++) {
        auto supportsPresent = device.getSurfaceSupportKHR(i, outputSurface);

        auto graphicsFlag = queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics;
        if (graphicsFlag == vk::QueueFlagBits::eGraphics && supportsPresent == VK_TRUE) {
            gfxQueueFamilyIdx = i;
            presentQueueFamilyIdx = i;
            break;
        } else {
            if (gfxQueueFamilyIdx == UINT32_MAX && graphicsFlag == vk::QueueFlagBits::eGraphics) {

                if (presentQueueFamilyIdx == UINT32_MAX && supportsPresent == VK_TRUE) {
                    presentQueueFamilyIdx = i;
                }
            }
        }
        gfxQueueFamilyIdx = i;
    }

    // Update surface capabilities 
    result = device.getSurfaceCapabilitiesKHR(outputSurface, &surfaceCapabilities);
    if (result != vk::Result::eSuccess) {
        throw Exception("Unable to determine surface capabilities on selected GPU.");
    }

    // Update present modes.
    presentModes = device.getSurfacePresentModesKHR(outputSurface);

    // Update surface formats.
    surfaceFormats = device.getSurfaceFormatsKHR(outputSurface);
    if (surfaceFormats.size() < 1) throw Exception("Unable to determine surface formats.");
    // TODO:: Smarter output format selection.
    outputFormat = surfaceFormats[0].format;
    if (outputFormat == vk::Format::eUndefined) {
        outputFormat = vk::Format::eB8G8R8A8Unorm;
    }
}


bool PhysicalDevice::GetMemoryTypeIndex(uint32_t typeBits, vk::MemoryPropertyFlags flags, uint32_t& index) const
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
