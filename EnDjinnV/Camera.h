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
        vec3 getPosition() const { return transform.Position(); }
        quat getRotation() const { return transform.Rotation(); }
        float getFovY() const { return fovy; }
        float getAspect() const { return aspect; }
        float getNearClip() const { return nearClip; }
        float getFarClip() const { return farClip; }
    private:
        Transform transform;
        float fovy;
        float aspect;
        float nearClip;
        float farClip;
    };
}
