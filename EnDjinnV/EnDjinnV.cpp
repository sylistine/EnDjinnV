#include "EnDjinnV.h"

#include <iostream>

#include "Platform.h"
#include "Gfx.h"

// Temp. These should be moved out later.
#include "Camera.h"
#include "Vertex.h"

#include "MathUtil.h"

std::vector<Djn::Gfx::Vertex> GetDefaultVertices();
std::vector<Djn::Gfx::Vertex> CubeMesh();

int main()
{
#if defined(_WIN32) && defined(NDEBUG)
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    std::cout << "Starting EnDjinn." << std::endl;
    using namespace Djn;
    auto appName = "Vulkan Exploration";
    try {
        std::cout << "Constructing platform." << std::endl;
        Platform platform(appName);

        std::cout << "Initializing graphics." << std::endl;
        Gfx::Manager::Initialize(platform.GetVkInstance(), platform.GetSurface());

        Djn::Camera mainCamera(vec3(0.f, 0.f, -5.f));
        auto triMeshVerts = CubeMesh();
        auto vPos = mainCamera.ViewMatrix() * triMeshVerts[0].Position();
        std::cout << Djn::Math::to_string(vPos) << std::endl;

        Gfx::Manager::SetViewProjectionMatrices(mainCamera.ViewMatrix(), mainCamera.ProjectionMatrix());
        Gfx::Manager::SetVertices(CubeMesh());
        Gfx::Manager::TempBuildAndRunPipeline();

        std::cout << "Looping forever." << std::endl;
        while (true) {}
    } catch (Exception& e) {
        std::cout << std::endl << "******** Djn::Exception ********" << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << e.StackTrace() << std::endl;
        std::cout << "************************" << std::endl;
    } catch (std::exception & e) {
        std::cout << std::endl << "******** std::exception ********" << std::endl;
        std::cout << e.what() << std::endl;
        std::cout << "************************" << std::endl;
    }
    std::cout << "Terminating EnDjinn" << std::endl;
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
