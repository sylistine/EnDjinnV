#include "EnDjinnV.h"
#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "MathUtil.h"

using namespace Djn;

Camera::Camera() : fovy(45.0f), nearClip(0.1f), farClip(100.0f)
{}


Camera::Camera(float fovy, float zNear, float zFar) :
    fovy(fovy),
    nearClip(zNear),
    farClip(zFar)
{}
