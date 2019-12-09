#pragma once

#include "EnDjinnV.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace Djn::Gfx
{
    class Swapchain
    {
    public:
        Swapchain() = default;
        Swapchain(vk::Device device, vk::SurfaceKHR surface, vk::Format format,
            vk::SurfaceCapabilitiesKHR surfaceCapabilities,
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
        vk::SwapchainKHR GetSwapchainKHR() const { return vkSwapchain; }
        size_t GetImageCount() const { return swapchainImages.size(); }
        vk::Image GetImage(uint32_t idx) const
        {
            if (idx < 0 || idx >= GetImageCount()) throw Exception("Argument out of range exception.");
            return swapchainImages[idx];
        }
        vk::ImageView GetImageView(uint32_t idx) const
        {
            if (idx < 0 || idx >= GetImageCount()) throw Exception("Argument out of range exception.");
            return swapchainImageViews[idx];
        }
    private:
        bool inited = false;
        vk::Device vkDevice;
        vk::SwapchainKHR vkSwapchain;
        std::vector<vk::Image> swapchainImages;
        std::vector<vk::ImageView> swapchainImageViews;
        void FreeDeviceMemory();
    };
}
