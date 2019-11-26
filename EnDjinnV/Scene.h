#pragma once


#include "EnDjinnV.h"

#include <vector>

#include "SceneObject.h"


class Scene
{
public:
    Scene();
private:
    std::vector<SceneObject> sceneObjects;
};

