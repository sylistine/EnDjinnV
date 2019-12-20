#pragma once

#include "EnDjinnV.h"

#include <vulkan/vulkan.hpp>
#include <vector>

namespace Djn::Gfx
{
    class Vertex
    {
    public:
        static std::vector<vk::VertexInputAttributeDescription> AttributeDescs();
        Vertex();
        Vertex(vec4 position, vec4 color);
        ~Vertex();
    private:
        vec4 position;
        vec4 color;
        vec4 normal;
        vec4 uv;
    };
}
