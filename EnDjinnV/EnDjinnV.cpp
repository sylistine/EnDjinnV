#include "EnDjinnV.h"

#include <iostream>

#include "Gfx.h"
#include "Platform.h"

// Temp. These should be moved out later.
#include "Camera.h"
#include "Vertex.h"

std::vector<Djn::Gfx::Vertex> GetDefaultVertices();

int main() {
#if defined(_WIN32) && defined(NDEBUG)
  // ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
  std::cout << "Starting EnDjinn." << std::endl;
  using namespace Djn;
  auto appName = "Vulkan Exploration";
  try {
    std::cout << "Constructing platform." << std::endl;
    Platform platform(appName);
    std::cout << "Initializing graphics." << std::endl;
    Gfx::Manager::Initialize(platform.GetVkInstance(), platform.GetSurface());

    Djn::Camera mainCamera;
    Gfx::Manager::SetViewProjectionMatrices(mainCamera.ViewMatrix(),
                                            mainCamera.ProjectionMatrix());
    Gfx::Manager::SetVertices(GetDefaultVertices());

    std::cout << "Looping forever." << std::endl;
    while (true) {
    }
  } catch (Exception& e) {
    std::cout << "Djn::Exception caught." << std::endl;
    std::cout << e.what() << std::endl;
    std::cout << e.StackTrace() << std::endl;
  } catch (std::exception& e) {
    std::cout << "Exception caught." << std::endl;
    std::cout << e.what() << std::endl;
  }
  std::cout << "Terminating EnDjinn" << std::endl;
}

std::vector<Djn::Gfx::Vertex> GetDefaultVertices() {
  std::vector<Djn::Gfx::Vertex> vertexList;
  vertexList.push_back(
      Djn::Gfx::Vertex(vec4(0.f, 0.f, 0.f, 0.f), vec4(1.f, 0.f, 0.f, 0.f)));
  vertexList.push_back(
      Djn::Gfx::Vertex(vec4(0.5f, 0.f, 0.f, 0.f), vec4(0.f, 1.f, 0.f, 0.f)));
  vertexList.push_back(
      Djn::Gfx::Vertex(vec4(0.f, 0.5f, 0.f, 0.f), vec4(0.f, 0.f, 1.f, 0.f)));
  return vertexList;
}
