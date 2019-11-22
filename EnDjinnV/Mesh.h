#pragma once

#include "Vertex.h"

static const Djn::Gfx::Vertex g_vb_solid_face_colors_Data[] = {
    // red face
    {Djn::double4(-1, -1, 1, 1), Djn::double4(1, 0, 0, 1)},
    {Djn::double4(-1, 1, 1), Djn::double4(1.f, 0.f, 0.f)},
    {Djn::double4(1, -1, 1), Djn::double4(1.f, 0.f, 0.f)},
    {Djn::double4(1, -1, 1), Djn::double4(1.f, 0.f, 0.f)},
    {Djn::double4(-1, 1, 1), Djn::double4(1.f, 0.f, 0.f)},
    {Djn::double4(1, 1, 1), Djn::double4(1.f, 0.f, 0.f)},
    // green face
    {Djn::double4(-1, -1, -1), Djn::double4(0.f, 1.f, 0.f)},
    {Djn::double4(1, -1, -1), Djn::double4(0.f, 1.f, 0.f)},
    {Djn::double4(-1, 1, -1), Djn::double4(0.f, 1.f, 0.f)},
    {Djn::double4(-1, 1, -1), Djn::double4(0.f, 1.f, 0.f)},
    {Djn::double4(1, -1, -1), Djn::double4(0.f, 1.f, 0.f)},
    {Djn::double4(1, 1, -1), Djn::double4(0.f, 1.f, 0.f)},
    // blue face
    {Djn::double4(-1, 1, 1), Djn::double4(0.f, 0.f, 1.f)},
    {Djn::double4(-1, -1, 1), Djn::double4(0.f, 0.f, 1.f)},
    {Djn::double4(-1, 1, -1), Djn::double4(0.f, 0.f, 1.f)},
    {Djn::double4(-1, 1, -1), Djn::double4(0.f, 0.f, 1.f)},
    {Djn::double4(-1, -1, 1), Djn::double4(0.f, 0.f, 1.f)},
    {Djn::double4(-1, -1, -1), Djn::double4(0.f, 0.f, 1.f)},
    // yellow face
    {Djn::double4(1, 1, 1), Djn::double4(1.f, 1.f, 0.f)},
    {Djn::double4(1, 1, -1), Djn::double4(1.f, 1.f, 0.f)},
    {Djn::double4(1, -1, 1), Djn::double4(1.f, 1.f, 0.f)},
    {Djn::double4(1, -1, 1), Djn::double4(1.f, 1.f, 0.f)},
    {Djn::double4(1, 1, -1), Djn::double4(1.f, 1.f, 0.f)},
    {Djn::double4(1, -1, -1), Djn::double4(1.f, 1.f, 0.f)},
    // magenta face
    {Djn::double4(1, 1, 1), Djn::double4(1.f, 0.f, 1.f)},
    {Djn::double4(-1, 1, 1), Djn::double4(1.f, 0.f, 1.f)},
    {Djn::double4(1, 1, -1), Djn::double4(1.f, 0.f, 1.f)},
    {Djn::double4(1, 1, -1), Djn::double4(1.f, 0.f, 1.f)},
    {Djn::double4(-1, 1, 1), Djn::double4(1.f, 0.f, 1.f)},
    {Djn::double4(-1, 1, -1), Djn::double4(1.f, 0.f, 1.f)},
    // cyan face
    {Djn::double4(1, -1, 1), Djn::double4(0.f, 1.f, 1.f)},
    {Djn::double4(1, -1, -1), Djn::double4(0.f, 1.f, 1.f)},
    {Djn::double4(-1, -1, 1), Djn::double4(0.f, 1.f, 1.f)},
    {Djn::double4(-1, -1, 1), Djn::double4(0.f, 1.f, 1.f)},
    {Djn::double4(1, -1, -1), Djn::double4(0.f, 1.f, 1.f)},
    {Djn::double4(-1, -1, -1), Djn::double4(0.f, 1.f, 1.f)},
};

class Mesh
{
public:
    Mesh();
    ~Mesh();
};

