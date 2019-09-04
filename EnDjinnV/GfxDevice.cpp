#include "GfxDevice.h"

#include <exception>

GfxDevice::GfxDevice(VkInstance instance, VkSurfaceKHR surface)
{
    // DO PHYSICAL DEVICE STUFF.
    uint32_t gpuc;
    std::vector<VkPhysicalDevice> physicalDevices;
    VkResult result = vkEnumeratePhysicalDevices(instance, &gpuc, NULL);
    if (gpuc < 1) throw std::exception();

    physicalDevices.resize(gpuc);
    result = vkEnumeratePhysicalDevices(instance, &gpuc, physicalDevices.data());

    
    for (int i = 0; i < gpuc; i++) {
        infos[i].gpu = physicalDevices[i];
    }

    // TODO: smarter GPU selection.
    preferredGpuIdx = 0;

    // Cache all GPUs' families' properties.
    for (int i = 0; i < infos.size(); i++) {
        uint32_t propertyFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(
            infos[i].gpu,
            &propertyFamilyCount,
            NULL);
        infos[i].queueFamilies.resize(propertyFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            infos[i].gpu,
            &propertyFamilyCount,
            infos[i].queueFamilies.data());
    }

    // Search for which queue families support Graphics and Present.
    auto queueFamilyCount = infos[preferredGpuIdx].queueFamilies.size();
    VkBool32 *pSupportsPresent = (VkBool32 *)malloc(queueFamilyCount * sizeof(VkBool32));
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(
            infos[preferredGpuIdx].gpu,
            i,
            surface,
            &pSupportsPresent[i]);
    }

    graphicsQueueIdx = UINT32_MAX;
    presentQueueIdx = UINT32_MAX;
    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        if ((infos[preferredGpuIdx].queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (graphicsQueueIdx == UINT32_MAX) graphicsQueueIdx = i;

            if (pSupportsPresent[i] == VK_TRUE) {
                graphicsQueueIdx = i;
                presentQueueIdx = i;
                break;
            }
        }
    }

    if (presentQueueIdx == UINT32_MAX) {
        for (size_t i = 0; i < queueFamilyCount; ++i)
            if (pSupportsPresent[i] == VK_TRUE) {
                presentQueueIdx = i;
                break;
            }
    }
    free(pSupportsPresent);

    // Now that we have a graphics queue, we can create a logical device.
    // Any logical device in Vulkan is bound to a queue, which is bound to a queue family.
    float queuePriorities[1] = { 0.0 };
    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = NULL;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = graphicsQueueIdx;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

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

    result = vkCreateDevice(infos[preferredGpuIdx].gpu, &createInfo, NULL, &device);
    if (result != VK_SUCCESS) throw std::exception("Unable to create device.");

    // Create command queue pool.
    VkCommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.pNext = NULL;
    commandPoolCreateInfo.flags = 0;
    commandPoolCreateInfo.queueFamilyIndex = graphicsQueueIdx;

    result = vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &commandPool);
    if (result != VK_SUCCESS) throw std::exception("Unable to create command pool.");

    VkCommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = NULL;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);
    if (result != VK_SUCCESS) throw std::exception("Unable to create command buffer.");
}


GfxDevice::~GfxDevice()
{
    vkDestroyCommandPool(device, commandPool, NULL);
    vkDestroyDevice(device, NULL);
}
