#pragma once

#include "EnDjinnV.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Djn::Gfx
{
    class Vertex
    {
    public:
        static std::vector<VkVertexInputAttributeDescription> AttributeDescs();
        Vertex();
        Vertex(vec4 position, vec4 color);
        ~Vertex();
    private:
        vec4 position;
        vec4 normal;
        vec4 uv;
        vec4 color;
    };
}
