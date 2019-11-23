#include "EnDjinnV.h"
#include "Device.h"

#include <vector>

#include "VulkanUtil.h"


using namespace Djn::Gfx;


Device::Device(PhysicalDevice physicalDevice) : physicalDevice(physicalDevice)
{
    float queuePriorities[1] = { 0.0 };

    auto deviceQueueCI = VkUtil::DeviceQueueCI();
    deviceQueueCI.queueFamilyIndex = physicalDevice.GetGraphicsQueueFamilyIndex();
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
    VkResult result = vkCreateDevice(physicalDevice.Get(), &deviceCI, NULL, &logicalDevice);
    if (result != VK_SUCCESS) throw std::exception("Unable to create logical device.");

    inited = true;
}


Device::~Device()
{
    FreeDeviceMemory();
}


Device::Device(Device&& other) noexcept : physicalDevice(other.physicalDevice)
{
    this->FreeDeviceMemory();
    this->logicalDevice = other.logicalDevice;
    this->inited = true;
    other.inited = false;
}


Device& Device::operator=(Device&& other) noexcept
{
    if (this != &other) {
        this->FreeDeviceMemory();
        this->logicalDevice = other.logicalDevice;
        this->physicalDevice = other.physicalDevice;
        this->inited = true;
        other.inited = false;
    }
    return *this;
}


bool Djn::Gfx::Device::GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags flags, uint32_t& index) const
{
    return physicalDevice.GetMemoryTypeIndex(typeBits, flags, index);
}


void Device::FreeDeviceMemory()
{
    if (!inited) return;

    vkDestroyDevice(logicalDevice, NULL);
    inited = false;
}
