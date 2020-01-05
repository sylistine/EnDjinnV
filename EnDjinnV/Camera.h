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
        Camera(float fovy, float zNear, float zFar);
        vec3 getPosition() const { return transform.getPosition(); }
        void setPosition(vec3 position) { transform.setPosition(position); }
        quat getRotation() const { return transform.getRotation(); }
        void setRotation(quat rotation) { transform.setRotation(rotation); }
        float getFovY() const { return fovy; }
        float getNearClip() const { return nearClip; }
        float getFarClip() const { return farClip; }
    private:
        Transform transform;
        float fovy;
        float nearClip;
        float farClip;
    };
}
