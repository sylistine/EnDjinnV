#pragma once


#include "EnDjinnV.h"

#include <vector>

#include "SceneObject.h"
#include "Camera.h"

namespace Djn
{
    class Scene
    {
    public:
        Scene();
        void update(float deltaTime);
    private:
        Camera mainCamera;
        std::vector<SceneObject> sceneObjects;
    };
}
