#include "GfxDevice.h"

#include <exception>
#include <vector>

GfxDevice::GfxDevice(VkPhysicalDevice gpu, unsigned int gpuPropertyFamilyIdx)
{
    float queuePriorities[1] = { 0.0 };
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        NULL,
        0,
        gpuPropertyFamilyIdx,
        1,
        queuePriorities
    };

    std::vector<const char*> extensions;
    extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    VkDeviceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = NULL;
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.pEnabledFeatures = NULL;

    VkResult result;
    result = vkCreateDevice(gpu, &createInfo, NULL, &device);
    if (result != VK_SUCCESS) throw std::exception("Unable to create device.");

    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        NULL,
        0,
        gpuPropertyFamilyIdx
    };

    result = vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &commandPool);
    if (result != VK_SUCCESS) throw std::exception("Unable to create command pool.");

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        NULL,
        commandPool,
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        1
    };

    result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
    if (result != VK_SUCCESS) throw std::exception("Unable to create command buffer.");
}


GfxDevice::~GfxDevice()
{
    vkDestroyCommandPool(device, commandPool, NULL);
    vkDestroyDevice(device, NULL);
}
