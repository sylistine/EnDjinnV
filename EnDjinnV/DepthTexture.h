#pragma once

#include <vulkan/vulkan.h>

#include "Device.h"

namespace Djn::Gfx {
class DepthTexture {
 public:
  DepthTexture(const Device& device, uint32_t initialWidth,
               uint32_t initialHeight);
  ~DepthTexture();
  vk::ImageView GetView() const { return view; }

 private:
  vk::Device vkDevice;
  vk::Image image;
  vk::DeviceMemory memory;
  vk::ImageView view;
};
}  // namespace Djn::Gfx
