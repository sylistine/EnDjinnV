#pragma once

#include <vector>
#include "Vertex.h"

static const Djn::Gfx::Vertex g_vb_solid_face_colors_Data[] = {
    // red face
    {vec4(-1, -1, 1, 1), vec4(1, 0, 0, 1)},
    {vec4(-1, 1, 1, 1), vec4(1.f, 0.5f, 0.f, 1)},
    {vec4(1, -1, 1, 1), vec4(1.f, 0.f, 0.f, 1)},
    {vec4(1, -1, 1, 1.f), vec4(1.f, 0.f, 0.f, 1.f)},
    {vec4(-1, 1, 1, 1.f), vec4(1.f, 0.f, 0.f, 1.f)},
    {vec4(1, 1, 1, 1.f), vec4(1.f, 0.f, 0.f, 1.f)},
    // green face
    {vec4(-1, -1, -1, 1.f), vec4(0.f, 1.f, 0.f, 1.f)},
    {vec4(1, -1, -1, 1.f), vec4(0.f, 1.f, 0.f, 1.f)},
    {vec4(-1, 1, -1, 1.f), vec4(0.f, 1.f, 0.f, 1.f)},
    {vec4(-1, 1, -1, 1.f), vec4(0.f, 1.f, 0.f, 1.f)},
    {vec4(1, -1, -1, 1.f), vec4(0.f, 1.f, 0.f, 1.f)},
    {vec4(1, 1, -1, 1.f), vec4(0.f, 1.f, 0.f, 1.f)},
    // blue face
    {vec4(-1, 1, 1, 1.f), vec4(0.f, 0.f, 1.f, 1.f)},
    {vec4(-1, -1, 1, 1.f), vec4(0.f, 0.f, 1.f, 1.f)},
    {vec4(-1, 1, -1, 1.f), vec4(0.f, 0.f, 1.f, 1.f)},
    {vec4(-1, 1, -1, 1.f), vec4(0.f, 0.f, 1.f, 1.f)},
    {vec4(-1, -1, 1, 1.f), vec4(0.f, 0.f, 1.f, 1.f)},
    {vec4(-1, -1, -1, 1.f), vec4(0.f, 0.f, 1.f, 1.f)},
    // yellow face
    {vec4(1, 1, 1, 1.f), vec4(1.f, 1.f, 0.f, 1.f)},
    {vec4(1, 1, -1, 1.f), vec4(1.f, 1.f, 0.f, 1.f)},
    {vec4(1, -1, 1, 1.f), vec4(1.f, 1.f, 0.f, 1.f)},
    {vec4(1, -1, 1, 1.f), vec4(1.f, 1.f, 0.f, 1.f)},
    {vec4(1, 1, -1, 1.f), vec4(1.f, 1.f, 0.f, 1.f)},
    {vec4(1, -1, -1, 1.f), vec4(1.f, 1.f, 0.f, 1.f)},
    // magenta face
    {vec4(1, 1, 1, 1.f), vec4(1.f, 0.f, 1.f, 1.f)},
    {vec4(-1, 1, 1, 1.f), vec4(1.f, 0.f, 1.f, 1.f)},
    {vec4(1, 1, -1, 1.f), vec4(1.f, 0.f, 1.f, 1.f)},
    {vec4(1, 1, -1, 1.f), vec4(1.f, 0.f, 1.f, 1.f)},
    {vec4(-1, 1, 1, 1.f), vec4(1.f, 0.f, 1.f, 1.f)},
    {vec4(-1, 1, -1, 1.f), vec4(1.f, 0.f, 1.f, 1.f)},
    // cyan face
    {vec4(1, -1, 1, 1.f), vec4(0.f, 1.f, 1.f, 1.f)},
    {vec4(1, -1, -1, 1.f), vec4(0.f, 1.f, 1.f, 1.f)},
    {vec4(-1, -1, 1, 1.f), vec4(0.f, 1.f, 1.f, 1.f)},
    {vec4(-1, -1, 1, 1.f), vec4(0.f, 1.f, 1.f, 1.f)},
    {vec4(1, -1, -1, 1.f), vec4(0.f, 1.f, 1.f, 1.f)},
    {vec4(-1, -1, -1, 1.f), vec4(0.f, 1.f, 1.f, 1.f)},
};

namespace Djn::Gfx {
class Mesh {
 public:
  Mesh();
  ~Mesh();

 private:
  std::vector<Vertex> verts;
  std::vector<size_t> tris;
};
}  // namespace Djn::Gfx
