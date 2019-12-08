#include "EnDjinnV.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace Djn;

Camera::Camera() : transform()
{}


Camera::Camera(vec3 pos) : transform(pos, quat())
{}


mat4 Camera::ProjectionMatrix() const
{
    return glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
}


mat4 Camera::ViewMatrix() const
{
    mat4 tMat = glm::translate(mat4(), transform.Position());
    mat4 rMat = glm::toMat4(transform.Rotation());
    return tMat * rMat;
}


mat4 Camera::ClipMatrix() const
{
    return mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
}
