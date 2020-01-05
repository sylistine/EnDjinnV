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
        void update();
    private:
        Camera mainCamera;
        std::vector<SceneObject> sceneObjects;
    };
}
