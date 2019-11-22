#pragma once

#include "Math.h"

namespace Djn::Gfx
{
    class Vertex
    {
    public:
        Vertex();
        Vertex(double4 position, double4 color);
        ~Vertex();
    private:
        double4 position;
        double4 color;
    };
}
