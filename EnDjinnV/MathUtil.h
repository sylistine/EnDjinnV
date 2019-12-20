#pragma once

#include "EnDjinnV.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Djn::Math
{
    static std::string to_string(vec4 v)
    {
        char buff[64];
        sprintf_s(
            buff,
            "(%f, %f, %f, %f)",
            v.x, v.y, v.z, v.w
        );
        return buff;
    }
    static std::string to_string(mat4 m)
    {
        char buff[256];
        sprintf_s(
            buff,
            "| %f, %f, %f, %f |\n| %f, %f, %f, %f |\n| %f, %f, %f, %f |\n| %f, %f, %f, %f |\n",
            m[0][0], m[0][1], m[0][2], m[0][3],
            m[1][0], m[1][1], m[1][2], m[1][3],
            m[2][0], m[2][1], m[2][2], m[2][3],
            m[3][0], m[3][1], m[3][2], m[3][3]);
        return buff;
    }
}
