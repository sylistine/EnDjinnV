#pragma once


#include "EnDjinnV.h"


/// The camera simply provides the view and projection matrices for a render.
class Camera
{
public:
    Camera();
    mat4 GetViewProjectionMatrix() const;
};

