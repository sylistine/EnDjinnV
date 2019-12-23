#include "EnDjinnV.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "MathUtil.h"

using namespace Djn;

Camera::Camera() : Camera(vec3())
{}


Camera::Camera(vec3 pos) :
    transform(pos, quat()),
    fovy(90.0f),
    aspect(1.0f),
    nearClip(0.1f),
    farClip(100.0f)
{}


mat4 Camera::ProjectionMatrix() const
{
    return glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

    auto proj = glm::perspective(glm::radians(fovy), aspect, nearClip, farClip);
    return proj;
}


mat4 Camera::ViewMatrix() const
{
    return glm::lookAt(glm::vec3(-5, 3, -10),  // Camera is at (-5,3,-10), in World Space
        glm::vec3(0, 0, 0),     // and looks at the origin
        glm::vec3(0, -1, 0)     // Head is up (set to 0,-1,0 to look upside-down)
    );

    auto pos = transform.Position();
    mat4 tMat = glm::translate(mat4(1.0f), -transform.Position());
    mat4 rMat = glm::toMat4(transform.Rotation());
    return rMat * tMat;
}


mat4 Camera::ClipMatrix() const
{
    return mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
}
