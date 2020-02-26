#pragma once

#include "EnDjinnV.h"

#include <vulkan/vulkan.hpp>

#include <vector>

namespace Djn::Gfx
{
    class Swapchain
    {
    public:
        class ViewHelper
        {
        public:
            ViewHelper() {}
            ViewHelper(vk::Device device, vk::Format format) : device(device), format(format) {}
            ~ViewHelper() { destroyViews(); }
            vk::ImageView operator[](int i) const {
                return views[i];
            }
            void createViews(std::vector<vk::Image> images) {
                destroyViews();
                views.resize(images.size());
                for (auto i = 0; i < images.size(); i++) {
                    vk::ImageViewCreateInfo swapchainImageViewCI;
                    swapchainImageViewCI.image = images[i];
                    swapchainImageViewCI.viewType = vk::ImageViewType::e2D;
                    swapchainImageViewCI.format = format;
                    swapchainImageViewCI.components.r = vk::ComponentSwizzle::eR;
                    swapchainImageViewCI.components.g = vk::ComponentSwizzle::eG;
                    swapchainImageViewCI.components.b = vk::ComponentSwizzle::eB;
                    swapchainImageViewCI.components.a = vk::ComponentSwizzle::eA;
                    swapchainImageViewCI.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
                    swapchainImageViewCI.subresourceRange.baseMipLevel = 0;
                    swapchainImageViewCI.subresourceRange.levelCount = 1;
                    swapchainImageViewCI.subresourceRange.baseArrayLayer = 0;
                    swapchainImageViewCI.subresourceRange.layerCount = 1;
                    vk::Result result = device.createImageView(&swapchainImageViewCI, NULL, &views[i]);
                    if (result != vk::Result::eSuccess) {
                        throw Exception("Unable to create swapchain image view.");
                    }
                }
            }
            void destroyViews() noexcept {
                for (auto i = views.size(); i > 0; i--) {
                    device.destroyImageView(views[i - 1]);
                }
            }
        private:
            vk::Device device;
            vk::Format format;
            std::vector<vk::ImageView> views;
        };

        Swapchain() = default;
        Swapchain(
            vk::Device device,
            vk::SurfaceKHR surface,
            vk::Format format,
            vk::Extent2D extent,
            vk::SurfaceCapabilitiesKHR surfaceCapabilities,
            std::vector<uint32_t> queueFamilyIndices);
        ~Swapchain();
        vk::SwapchainKHR GetSwapchainKHR() const { return swapchain; }
        size_t GetImageCount() const { return swapchainImages.size(); }
        vk::Image GetImage(uint32_t idx) const
        {
            if (idx < 0 || idx >= GetImageCount()) throw Exception("Argument out of range exception.");
            return swapchainImages[idx];
        }
        vk::ImageView GetImageView(uint32_t idx) const
        {
            if (idx < 0 || idx >= GetImageCount()) throw Exception("Argument out of range exception.");
            return viewHelper[idx];
        }
        void resize(vk::Extent2D newSize);
        void createSwapchain();
        void destroySwapchain();
    private:
        vk::Device device;
        vk::SwapchainKHR swapchain;
        std::vector<vk::Image> swapchainImages;
        ViewHelper viewHelper;
        vk::SwapchainCreateInfoKHR swapchainCI;
    };
}
