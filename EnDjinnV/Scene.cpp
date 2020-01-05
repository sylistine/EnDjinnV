#include "EnDjinnV.h"
#include "Scene.h"
#include "Gfx.h"

using namespace Djn;

std::vector<Djn::Gfx::Vertex> GetDefaultVertices();
std::vector<Djn::Gfx::Vertex> CubeMesh();

Scene::Scene() : mainCamera(45.0f, 0.1f, 100.0f)
{
    mainCamera.setPosition(vec3(2.0f, 2.0f, 5.0f));
    Gfx::Manager::SetCameraParameters(
        mainCamera.getFovY(),
        mainCamera.getNearClip(),
        mainCamera.getFarClip(),
        mainCamera.getPosition(),
        mainCamera.getRotation());
    Gfx::Manager::SetVertices(CubeMesh());
    Gfx::Manager::SetupPipeline();
}

void Scene::update() {
    // Update camera pos, vertex count
}

std::vector<Djn::Gfx::Vertex> GetDefaultVertices()
{
    std::vector<Djn::Gfx::Vertex> vertexList;
    vertexList.push_back(Djn::Gfx::Vertex(vec4(-1.f, 0.f, 0.f, 1.f), vec4(1.f, 0.f, 0.f, 1.f)));
    vertexList.push_back(Djn::Gfx::Vertex(vec4(0.0f, 1.f, 0.f, 1.f), vec4(0.f, 1.f, 0.f, 1.f)));
    vertexList.push_back(Djn::Gfx::Vertex(vec4(1.f, 0.f, 0.f, 1.f), vec4(0.f, 0.f, 1.f, 1.f)));
    vertexList.push_back(Djn::Gfx::Vertex(vec4(1.f, 0.f, 0.f, 1.f), vec4(0.f, 0.f, 1.f, 1.f)));
    vertexList.push_back(Djn::Gfx::Vertex(vec4(0.0f, 1.f, 0.f, 1.f), vec4(0.f, 1.f, 0.f, 1.f)));
    vertexList.push_back(Djn::Gfx::Vertex(vec4(-1.f, 0.f, 0.f, 1.f), vec4(1.f, 0.f, 0.f, 1.f)));
    return vertexList;
}


static std::vector<Djn::Gfx::Vertex> CubeMesh()
{
    std::vector<Djn::Gfx::Vertex> verts{
        // red face
        {vec4(-1, -1, 1, 1), vec4(1.f, 0.f, 0.f, 1)},
        {vec4(-1, 1, 1, 1), vec4(1.f, 0.f, 0.f, 1.f)},
        {vec4(1, -1, 1, 1), vec4(1.f, 0.f, 0.f, 1.f)},
        {vec4(1, -1, 1, 1), vec4(1.f, 0.f, 0.f, 1.f)},
        {vec4(-1, 1, 1, 1), vec4(1.f, 0.f, 0.f, 1.f)},
        {vec4(1, 1, 1, 1), vec4(1.f, 0.f, 0.f, 1.f)},
        // green face
        {vec4(-1, -1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        {vec4(1, -1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        {vec4(-1, 1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        {vec4(-1, 1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        {vec4(1, -1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        {vec4(1, 1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        // blue face
        {vec4(-1, 1, 1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        {vec4(-1, -1, 1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        {vec4(-1, 1, -1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        {vec4(-1, 1, -1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        {vec4(-1, -1, 1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        {vec4(-1, -1, -1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        // yellow face
        {vec4(1, 1, 1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        {vec4(1, 1, -1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        {vec4(1, -1, 1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        {vec4(1, -1, 1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        {vec4(1, 1, -1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        {vec4(1, -1, -1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        // magenta face
        {vec4(1, 1, 1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        {vec4(-1, 1, 1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        {vec4(1, 1, -1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        {vec4(1, 1, -1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        {vec4(-1, 1, 1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        {vec4(-1, 1, -1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        // cyan face
        {vec4(1, -1, 1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
        {vec4(1, -1, -1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
        {vec4(-1, -1, 1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
        {vec4(-1, -1, 1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
        {vec4(1, -1, -1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
        {vec4(-1, -1, -1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
    };
    return verts;
}
