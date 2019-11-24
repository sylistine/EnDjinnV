#pragma once

#include "EnDjinnV.h"

namespace Djn::Gfx
{
    class Vertex
    {
    public:
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
