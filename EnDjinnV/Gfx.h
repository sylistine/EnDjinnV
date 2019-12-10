#pragma once

#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.hpp>

#include "Shader.h"
#include "VulkanUtil.h"

#include "Buffer.h"
#include "CommandPool.h"
#include "DepthTexture.h"
#include "Device.h"
#include "PhysicalDevice.h"
#include "RenderPass.h"
#include "SwapChain.h"

namespace Djn::Gfx {
class Manager {
 public:
  static void Initialize(vk::Instance vkInstance, vk::SurfaceKHR surface);
  static void SetViewProjectionMatrices(mat4 viewMatrix, mat4 projectionMatrix);
  static void SetVertices(std::vector<Vertex> vertices);

 private:
  static Manager* gfxInstance;
  Manager(vk::Instance vkInstance, vk::SurfaceKHR surface);
  ~Manager();

  vk::Instance instance;
  PhysicalDevice primaryGPU;
  Device device;
  CommandPool gfxCommandPool;
  Swapchain swapchain;
  DepthTexture* depthTexture;
  vk::ShaderModule vertexShaderModule;
  vk::ShaderModule fragmentShaderModule;

  vk::RenderPass primaryRenderPass;
  vk::Framebuffer* primaryFramebuffer;
  vk::Buffer vertexBuffer;
  vk::Buffer mvpBuffer;

  std::vector<unsigned int> CompileShader(shaderc::Compiler& compiler,
                                          Shader shader);
  void SetupPrimaryRenderPass();
  void TeardownPrimaryRenderPass();
  void SetPrimaryVertexBuffer(std::vector<Vertex> vertices);
  Buffer CreateUniformBuffer(void* data, size_t size);
};
}  // namespace Djn::Gfx