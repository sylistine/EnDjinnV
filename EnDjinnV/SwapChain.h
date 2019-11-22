#pragma once

#include "EnDjinnV.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Djn::Gfx
{
    class Swapchain
    {
    public:
        Swapchain() = default;
        Swapchain(VkDevice device, VkSurfaceKHR surface, VkFormat format,
            VkSurfaceCapabilitiesKHR surfaceCapabilities,
            std::vector<uint32_t> queueFamilyIndices);
        ~Swapchain();
        Swapchain(Swapchain&& other)
        {
            this->FreeDeviceMemory();
            this->vkDevice = other.vkDevice;
            this->vkSwapchain = other.vkSwapchain;
            this->swapchainImages = other.swapchainImages;
            this->swapchainImageViews = other.swapchainImageViews;
            this->inited = true;
            other.inited = false;
        }
        Swapchain& operator=(Swapchain&& other)
        {
            if (this != &other) {
                this->FreeDeviceMemory();
                this->vkDevice = other.vkDevice;
                this->vkSwapchain = other.vkSwapchain;
                this->swapchainImages = other.swapchainImages;
                this->swapchainImageViews = other.swapchainImageViews;
                this->inited = true;
                other.inited = false;
            }
            return *this;
        }
        VkSwapchainKHR GetSwapchainKHR() const { return vkSwapchain; }
        size_t GetImageCount() const { return swapchainImages.size(); }
        VkImage GetImage(uint32_t idx) const
        {
            if (idx < 0 || idx >= GetImageCount()) throw std::exception("Argument out of range exception.");
            return swapchainImages[idx];
        }
        VkImageView GetImageView(uint32_t idx) const
        {
            if (idx < 0 || idx >= GetImageCount()) throw std::exception("Argument out of range exception.");
            return swapchainImageViews[idx];
        }
    private:
        bool inited = false;
        VkDevice vkDevice;
        VkSwapchainKHR vkSwapchain;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        void FreeDeviceMemory();
    };
}
