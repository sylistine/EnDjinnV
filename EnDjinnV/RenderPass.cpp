#include "RenderPass.h"
#include "Buffer.h"
#include "EnDjinnV.h"

using namespace Djn::Gfx;

Djn::Gfx::RenderPass::RenderPass(VkDevice device, VkRenderPass renderPass)
    : device(device), renderPass(renderPass) {}

Djn::Gfx::RenderPass::~RenderPass() {
  vkDestroyRenderPass(device, renderPass, NULL);
}
