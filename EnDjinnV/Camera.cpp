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
    fovy(45.0f),
    aspect(1.0f),
    nearClip(0.1f),
    farClip(100.0f)
{}


mat4 Camera::ProjectionMatrix() const
{
    auto proj = glm::perspective(glm::radians(fovy), aspect, nearClip, farClip);
    std::cout << Math::to_string(proj) << std::endl;
    return proj;
}


mat4 Camera::ViewMatrix() const
{
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
