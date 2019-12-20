#pragma once


#include "EnDjinnV.h"
#include "Transform.h"

namespace Djn
{
    /// The camera simply provides the view and projection matrices for a render.
    class Camera
    {
    public:
        Camera();
        Camera(vec3 pos);
        mat4 ProjectionMatrix() const;
        mat4 ViewMatrix() const;
        mat4 ClipMatrix() const;
    private:
        Transform transform;
        float fovy;
        float aspect;
        float nearClip;
        float farClip;
    };
}
