#include "EnDjinnV.h"
#include "Device.h"

#include <vector>

#include "VulkanUtil.h"


using namespace Djn::Gfx;


Device::Device(uint32_t gfxQueueFamilyIdx, VkPhysicalDevice physicalDevice)
{
    // create logical device
    float queuePriorities[1] = { 0.0 };

    auto deviceQueueCI = VkUtil::DeviceQueueCI();
    deviceQueueCI.queueFamilyIndex = gfxQueueFamilyIdx;
    deviceQueueCI.queueCount = 1;
    deviceQueueCI.pQueuePriorities = queuePriorities;

    std::vector<const char*> deviceExtensions;
    deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    auto deviceCI = VkUtil::DeviceCI();
    deviceCI.queueCreateInfoCount = 1;
    deviceCI.pQueueCreateInfos = &deviceQueueCI;
    deviceCI.enabledLayerCount = 0;
    deviceCI.ppEnabledLayerNames = NULL;
    deviceCI.enabledExtensionCount = deviceExtensions.size();
    deviceCI.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCI.pEnabledFeatures = NULL;
    VkResult result = vkCreateDevice(physicalDevice, &deviceCI, NULL, &vkDevice);
    if (result != VK_SUCCESS) throw std::exception("Unable to create logical device.");

    inited = true;
}


Device::~Device()
{
    FreeDeviceMemory();
}


void Device::FreeDeviceMemory()
{
    if (!inited) return;

    vkDestroyDevice(vkDevice, NULL);
    inited = false;
}