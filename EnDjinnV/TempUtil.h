#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "Device.h"

namespace Djn::Gfx {
inline void PipelineCodeUtil(
    const Device& device, vk::VertexInputBindingDescription viBindingDesc,
    std::vector<vk::VertexInputAttributeDescription> viAttributes,
    vk::PipelineLayout layout,
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages,
    vk::RenderPass renderPass, vk::Pipeline* pipeline) {
  vk::DynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
  vk::PipelineDynamicStateCreateInfo dynamicState = {};
  memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);

  dynamicState.pDynamicStates = dynamicStateEnables;
  dynamicState.dynamicStateCount = 0;

  vk::PipelineVertexInputStateCreateInfo vi;
  vi.vertexBindingDescriptionCount = 1;
  vi.pVertexBindingDescriptions = &viBindingDesc;
  vi.vertexAttributeDescriptionCount = viAttributes.size();
  vi.pVertexAttributeDescriptions = viAttributes.data();

  vk::PipelineInputAssemblyStateCreateInfo ia;
  ia.primitiveRestartEnable = false;
  ia.topology = vk::PrimitiveTopology::eTriangleList;

  vk::PipelineRasterizationStateCreateInfo rs;
  rs.polygonMode = vk::PolygonMode::eFill;
  rs.cullMode = vk::CullModeFlagBits::eBack;
  rs.frontFace = vk::FrontFace::eClockwise;
  rs.depthClampEnable = false;
  rs.rasterizerDiscardEnable = false;
  rs.depthBiasEnable = false;
  rs.depthBiasConstantFactor = 0;
  rs.depthBiasClamp = 0;
  rs.depthBiasSlopeFactor = 0;
  rs.lineWidth = 1.0f;

  vk::PipelineColorBlendAttachmentState att_state[1];
  att_state[0].colorWriteMask = vk::ColorComponentFlags(0xf);
  att_state[0].blendEnable = false;
  att_state[0].alphaBlendOp = vk::BlendOp::eAdd;
  att_state[0].colorBlendOp = vk::BlendOp::eAdd;
  att_state[0].srcColorBlendFactor = vk::BlendFactor::eZero;
  att_state[0].dstColorBlendFactor = vk::BlendFactor::eZero;
  att_state[0].srcAlphaBlendFactor = vk::BlendFactor::eZero;
  att_state[0].dstAlphaBlendFactor = vk::BlendFactor::eZero;

  vk::PipelineColorBlendStateCreateInfo cb;
  cb.attachmentCount = 1;
  cb.pAttachments = att_state;
  cb.logicOpEnable = false;
  cb.logicOp = vk::LogicOp::eNoOp;
  cb.blendConstants[0] = 1.0f;
  cb.blendConstants[1] = 1.0f;
  cb.blendConstants[2] = 1.0f;
  cb.blendConstants[3] = 1.0f;

  vk::PipelineViewportStateCreateInfo vp = {};
  vp.viewportCount = 1;
  dynamicStateEnables[dynamicState.dynamicStateCount++] =
      vk::DynamicState::eViewport;
  vp.scissorCount = 1;
  dynamicStateEnables[dynamicState.dynamicStateCount++] =
      vk::DynamicState::eScissor;
  vp.pScissors = NULL;
  vp.pViewports = NULL;

  vk::PipelineDepthStencilStateCreateInfo ds;
  ds.depthTestEnable = true;
  ds.depthWriteEnable = true;
  ds.depthCompareOp = vk::CompareOp::eLessOrEqual;
  ds.depthBoundsTestEnable = false;
  ds.minDepthBounds = 0;
  ds.maxDepthBounds = 0;
  ds.stencilTestEnable = false;
  ds.back.failOp = vk::StencilOp::eKeep;
  ds.back.passOp = vk::StencilOp::eKeep;
  ds.back.compareOp = vk::CompareOp::eAlways;
  ds.back.compareMask = 0;
  ds.back.reference = 0;
  ds.back.depthFailOp = vk::StencilOp::eKeep;
  ds.back.writeMask = 0;
  ds.front = ds.back;

  vk::PipelineMultisampleStateCreateInfo ms;
  ms.pSampleMask = NULL;
  ms.rasterizationSamples = vk::SampleCountFlagBits::e1;
  ms.sampleShadingEnable = VK_FALSE;
  ms.alphaToCoverageEnable = VK_FALSE;
  ms.alphaToOneEnable = VK_FALSE;
  ms.minSampleShading = 0.0;

  vk::GraphicsPipelineCreateInfo pipelineCI;
  pipelineCI.layout = layout;
  pipelineCI.basePipelineHandle = vk::Pipeline();
  pipelineCI.basePipelineIndex = 0;
  pipelineCI.pVertexInputState = &vi;
  pipelineCI.pInputAssemblyState = &ia;
  pipelineCI.pRasterizationState = &rs;
  pipelineCI.pColorBlendState = &cb;
  pipelineCI.pTessellationState = NULL;
  pipelineCI.pMultisampleState = &ms;
  pipelineCI.pDynamicState = &dynamicState;
  pipelineCI.pViewportState = &vp;
  pipelineCI.pDepthStencilState = &ds;
  pipelineCI.pStages = shaderStages.data();
  pipelineCI.stageCount = shaderStages.size();
  pipelineCI.renderPass = renderPass;
  pipelineCI.subpass = 0;

  vk::Result result = device.GetLogical().createGraphicsPipelines(
      vk::PipelineCache(), 1, &pipelineCI, NULL, pipeline);
  if (result != vk::Result::eSuccess) {
    throw Exception("Unable to create graphics pipeline.");
  }
}
}  // namespace Djn::Gfx
