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
