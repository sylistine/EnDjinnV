#pragma once

#include <vulkan/vulkan.hpp>

namespace Djn {
class Platform {
 public:
  Platform(const char* appName);
  ~Platform();
  vk::Instance GetVkInstance() const { return vkInstance; }
  vk::SurfaceKHR GetSurface() const { return surface; }

 private:
  vk::Instance vkInstance;
  vk::SurfaceKHR surface;
};
}  // namespace Djn