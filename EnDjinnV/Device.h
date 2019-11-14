#pragma once

#include <vulkan/vulkan.h>

namespace Djn::Gfx
{
    class Device
    {
    public:
        Device() = default;
        Device(uint32_t gfxQueueFamilyIdx, VkPhysicalDevice physicalDevice);
        ~Device();
        Device(Device&& other)
        {
            this->FreeDeviceMemory();
            this->vkDevice = other.vkDevice;
            this->inited = true;
            other.inited = false;
        }
        Device& operator=(Device&& other)
        {
            if (this != &other) {
                this->FreeDeviceMemory();
                this->vkDevice = other.vkDevice;
                this->inited = true;
                other.inited = false;
            }
            return *this;
        }
        VkDevice Get() const { return vkDevice; }
    private:
        bool inited = false;
        VkDevice vkDevice;
        void FreeDeviceMemory();
    };
}
