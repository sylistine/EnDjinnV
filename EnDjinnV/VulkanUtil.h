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
}
