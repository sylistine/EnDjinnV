#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/spirv.h>
#include <shaderc/shaderc.h>

#include <vector>

namespace Djn::VkUtil
{
    /// When the shader pipeline is setup propertly, they will not be compiled internally.
    /// When that happens, the spirv assembly will be loaded here.
    static void LoadShader()
    {

    }


    inline std::vector<VkImage> GetSwapchainImages(VkDevice device, VkSwapchainKHR swapchain)
    {
        uint32_t count;
        VkResult result = vkGetSwapchainImagesKHR(device, swapchain, &count, NULL);
        if (result != VK_SUCCESS) throw Exception("Unable to get swapchain image count.");
        std::vector<VkImage> list(count);
        result = vkGetSwapchainImagesKHR(device, swapchain, &count, list.data());
        if (result != VK_SUCCESS) throw Exception("Unable to get swapchain images.");
        return list;
    }
}
