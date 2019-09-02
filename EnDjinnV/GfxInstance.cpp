#include "GfxInstance.h"

#include <exception>

GfxInstance::GfxInstance(const char* appName)
{
    VkApplicationInfo applicationInfo;
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = NULL;
    applicationInfo.pApplicationName = appName;
    applicationInfo.applicationVersion = 0;
    applicationInfo.pEngineName = appName;
    applicationInfo.engineVersion = 0;
    applicationInfo.apiVersion = VK_API_VERSION_1_1;

    std::vector<const char*> extensions;
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = NULL;
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkResult result;
    result = vkCreateInstance(&createInfo, NULL, &instance);
    if (result != VK_SUCCESS) throw std::exception("Unable to create vulkan instance.");

    uint32_t gpuc;
    result = vkEnumeratePhysicalDevices(instance, &gpuc, NULL);
    gpus.resize(gpuc);
    gpuPropertyFamily.resize(gpuc);
    result = vkEnumeratePhysicalDevices(instance, &gpuc, gpus.data());
    if (gpuc < 1) throw std::exception();

    // TODO: smarter GPU selection.
    preferredGpuIdx = 0;

    // Just go ahead and enumerate all GPUs' properties.
    for (int i = 0; i < gpus.size(); i++) {
        uint32_t propertyFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(gpus[i], &propertyFamilyCount, NULL);
        gpuPropertyFamily[i].resize(propertyFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(gpus[i], &propertyFamilyCount, gpuPropertyFamily[i].data());
    }

    int queueCount = 0;
    for (int i = 0; i < gpuPropertyFamily[0].size(); i++) {
        if (gpuPropertyFamily[0][i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
            gpuPropertyFamily[0][i].queueCount > queueCount) {
            preferredGpuPropertyFamilyIdx = i;
            queueCount = gpuPropertyFamily[0][i].queueCount;
        }
    }
}


GfxInstance::~GfxInstance()
{
    vkDestroyInstance(instance, NULL);
}
