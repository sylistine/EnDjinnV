#include "EnDjinnV.h"

#include "Gfx.h"

#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Shader.h"

using namespace Djn::Gfx;


const vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1;


/// Static
Manager* Manager::Initialize(vk::Instance vkInstance, vk::SurfaceKHR surface)
{
    if (instance != NULL) throw Exception("Manager is already initialized.");
    instance = new Manager(vkInstance, surface);
    return instance;
}


/// Static
void Manager::Terminate()
{
    if (instance == NULL) {
        std::cout << "Graphics has either not been initialized or has already been deleted." << std::endl;
        return;
    }
    delete instance;
}


/// Static
Manager* Manager::GetInstance()
{
    if (instance == NULL) throw Exception("Manager has not been initialized.");
    return instance;
}


/// Static
Manager* Manager::instance = NULL;


Manager::Manager(vk::Instance vkInstance, vk::SurfaceKHR surface) :
    vulkan(vkInstance),
    primaryGPU(vulkan.enumeratePhysicalDevices()[0], surface),
    device(primaryGPU)
{
    vk::Result result;

    auto gfxQueueFamilyIdx = primaryGPU.GetGraphicsQueueFamilyIndex();
    auto presentQueueFamilyIdx = primaryGPU.GetPresentQueueFamilyIndex();
    gfxCommandPool = CommandPool(device.GetLogical(), gfxQueueFamilyIdx, 1);

    // Create default swapchain and depth texture.
    // Get swapchain size.
    auto surfaceCapabilities = primaryGPU.GetSurfaceCapabilities();
    vk::Extent2D swapchainExtent;
    // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapchainExtent = surfaceCapabilities.currentExtent;
        if (swapchainExtent.width < surfaceCapabilities.minImageExtent.width) {
            swapchainExtent.width = surfaceCapabilities.minImageExtent.width;
        } else if (swapchainExtent.width > surfaceCapabilities.maxImageExtent.width) {
            swapchainExtent.width = surfaceCapabilities.maxImageExtent.width;
        }

        if (swapchainExtent.height < surfaceCapabilities.minImageExtent.height) {
            swapchainExtent.height = surfaceCapabilities.minImageExtent.height;
        } else if (swapchainExtent.height > surfaceCapabilities.maxImageExtent.height) {
            swapchainExtent.height = surfaceCapabilities.maxImageExtent.height;
        }
    } else {
        // If the surface size is defined, the swap chain size must match
        swapchainExtent = surfaceCapabilities.currentExtent;
    }
    std::vector<uint32_t> queueFamilyIndices;
    if (gfxQueueFamilyIdx != presentQueueFamilyIdx) {
        queueFamilyIndices.push_back(gfxQueueFamilyIdx);
        queueFamilyIndices.push_back(presentQueueFamilyIdx);
    }
    swapchain = Swapchain(device.GetLogical(), surface, primaryGPU.GetOutputFormat(), swapchainExtent,
        surfaceCapabilities, queueFamilyIndices);
    swapchain.createSwapchain();

    // create depth texture
    depthTexture = new DepthTexture(device, primaryGPU.GetSurfaceCapabilities().currentExtent);

    // Setup basic vertex shader.
    auto vertexShader = Shader::LoadBasicVertexShader();
    vk::ShaderModuleCreateInfo vsModuleCI;
    vsModuleCI.codeSize = vertexShader.size() * sizeof(vertexShader[0]);
    vsModuleCI.pCode = (uint32_t*)vertexShader.data();
    result = device.GetLogical().createShaderModule(&vsModuleCI, NULL, &vertexShaderModule);
    if (result != vk::Result::eSuccess) {
        throw Exception("Unable to create vertex shader module.");
    }
    vk::PipelineShaderStageCreateInfo vsPipelineShaderStageCI;
    vsPipelineShaderStageCI.stage = vk::ShaderStageFlagBits::eVertex;
    vsPipelineShaderStageCI.module = vertexShaderModule;
    vsPipelineShaderStageCI.pName = "main";
    vsPipelineShaderStageCI.pSpecializationInfo = NULL;
    pipelineShaderStages.push_back(vsPipelineShaderStageCI);

    // Setup basic fragment shader.
    auto fragmentShader = Shader::LoadBasicFragmentShader();
    vk::ShaderModuleCreateInfo fsModuleCI;
    fsModuleCI.codeSize = fragmentShader.size() * sizeof(fragmentShader[0]);
    fsModuleCI.pCode = (uint32_t*)fragmentShader.data();
    result = device.GetLogical().createShaderModule(&fsModuleCI, NULL, &fragmentShaderModule);
    if (result != vk::Result::eSuccess) {
        device.GetLogical().destroyShaderModule(vertexShaderModule);
        throw Exception("Unable to create fragment shader module.");
    }
    vk::PipelineShaderStageCreateInfo  fsPipelineShaderStageCI;
    fsPipelineShaderStageCI.stage = vk::ShaderStageFlagBits::eFragment;
    fsPipelineShaderStageCI.module = fragmentShaderModule;
    fsPipelineShaderStageCI.pName = "main";
    fsPipelineShaderStageCI.pSpecializationInfo = NULL;
    pipelineShaderStages.push_back(fsPipelineShaderStageCI);

    timeBuffer = Buffer(
        device,
        vk::BufferUsageFlagBits::eUniformBuffer,
        &timeData,
        sizeof(timeData));
    viewProjectionBuffer = Buffer(
        device,
        vk::BufferUsageFlagBits::eUniformBuffer,
        &viewProjectionData,
        sizeof(viewProjectionData));

    setupPrimaryRenderPass();
}


Manager::~Manager()
{
    auto d = device.GetLogical();
    d.destroyPipeline(primaryPipeline);
    d.destroyPipelineLayout(primaryPipelineLayout);
    d.destroyPipelineCache(pipelineCache);
    d.destroyRenderPass(primaryRenderPass);
    d.destroyDescriptorSetLayout(primaryDescriptorSetLayout);
    d.destroyDescriptorPool(primaryDescriptorPool);
    for (auto i = 0u; i < swapchain.GetImageCount(); i++) {
        d.destroyFramebuffer(primaryFramebuffer[i]);
    }
    free(primaryFramebuffer);

    
    d.destroyShaderModule(fragmentShaderModule);
    d.destroyShaderModule(vertexShaderModule);
    delete depthTexture;
    swapchain.destroySwapchain();
}


void Manager::setupPrimaryRenderPass()
{
    auto d = device.GetLogical();

    const uint32_t attachmentDescCount = 2;
    vk::AttachmentDescription attachmentDescs[attachmentDescCount];
    // Primary color attachment.
    attachmentDescs[0].format = primaryGPU.GetOutputFormat();
    attachmentDescs[0].samples = sampleCount;
    attachmentDescs[0].loadOp = vk::AttachmentLoadOp::eClear;
    attachmentDescs[0].storeOp = vk::AttachmentStoreOp::eStore;
    attachmentDescs[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachmentDescs[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDescs[0].initialLayout = vk::ImageLayout::eUndefined;
    attachmentDescs[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;
    // Depth attachment.
    attachmentDescs[1].format = vk::Format::eD16Unorm;
    attachmentDescs[1].samples = sampleCount;
    attachmentDescs[1].loadOp = vk::AttachmentLoadOp::eClear;
    attachmentDescs[1].storeOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDescs[1].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachmentDescs[1].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDescs[1].initialLayout = vk::ImageLayout::eUndefined;
    attachmentDescs[1].finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::SubpassDescription subpassDesc = {};
    subpassDesc.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpassDesc.inputAttachmentCount = 0;
    subpassDesc.pInputAttachments = NULL;
    subpassDesc.colorAttachmentCount = 1;
    subpassDesc.pColorAttachments = &colorReference;
    subpassDesc.pResolveAttachments = NULL;
    subpassDesc.pDepthStencilAttachment = &depthReference;
    subpassDesc.preserveAttachmentCount = 0;
    subpassDesc.pPreserveAttachments = NULL;

    vk::RenderPassCreateInfo renderpassCI;
    renderpassCI.attachmentCount = attachmentDescCount;
    renderpassCI.pAttachments = attachmentDescs;
    renderpassCI.subpassCount = 1;
    renderpassCI.pSubpasses = &subpassDesc;
    renderpassCI.dependencyCount = 0;
    renderpassCI.pDependencies = NULL;

    vk::Result result = d.createRenderPass(&renderpassCI, NULL, &primaryRenderPass);

    if (result != vk::Result::eSuccess) {
        throw Exception("Problem creating render pass.");
    }

    // Build frame buffer...
    vk::ImageView attachments[2];
    attachments[1] = depthTexture->GetView();

    auto extents = primaryGPU.GetSurfaceCapabilities().currentExtent;

    vk::FramebufferCreateInfo framebufferCI;
    framebufferCI.renderPass = primaryRenderPass;
    framebufferCI.attachmentCount = 2;
    framebufferCI.pAttachments = attachments;
    framebufferCI.width = extents.width;
    framebufferCI.height = extents.height;
    framebufferCI.layers = 1;

    primaryFramebuffer = (vk::Framebuffer*)malloc(swapchain.GetImageCount() * sizeof(vk::Framebuffer));
    if (primaryFramebuffer == nullptr) {
        d.destroyRenderPass(primaryRenderPass);
        throw Exception("Unable to allocate memory for primary frame buffer.");
    }

    for (auto i = 0u; i < swapchain.GetImageCount(); i++) {
        attachments[0] = swapchain.GetImageView(i);
        result = d.createFramebuffer(&framebufferCI, NULL, &primaryFramebuffer[i]);
        if (result != vk::Result::eSuccess) {
            for (auto j = i; j > 0; j--) {
                d.destroyFramebuffer(primaryFramebuffer[j - 1]);
            }
            d.destroyRenderPass(primaryRenderPass);
            throw Exception("Unable to create framebuffer.");
        }
    }

    // Descriptor sets.
    // These prewarm various buffers that our shaders will reference later.
    // Presently, the only buffer we use is the view/projection buffer.
    // In the future, this will include textures sampled by the fragment shader, time keeping, etc.
    vk::DescriptorPoolSize descPoolSize[2];
    descPoolSize[0].type = vk::DescriptorType::eUniformBuffer;
    descPoolSize[0].descriptorCount = 1;
    descPoolSize[1].type = vk::DescriptorType::eUniformBuffer;
    descPoolSize[1].descriptorCount = 1;

    vk::DescriptorPoolCreateInfo descPoolCI;
    descPoolCI.maxSets = 2;
    descPoolCI.poolSizeCount = 2;
    descPoolCI.pPoolSizes = descPoolSize;
    result = d.createDescriptorPool(&descPoolCI, NULL, &primaryDescriptorPool);

    // Zero is time, One is MVP
    vk::DescriptorSetLayoutBinding descSetBindings[2];
    descSetBindings[0].binding = 0;
    descSetBindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
    descSetBindings[0].descriptorCount = 1;
    descSetBindings[0].stageFlags = vk::ShaderStageFlagBits::eVertex;
    descSetBindings[0].pImmutableSamplers = NULL;
    descSetBindings[1].binding = 1;
    descSetBindings[1].descriptorType = vk::DescriptorType::eUniformBuffer;
    descSetBindings[1].descriptorCount = 1;
    descSetBindings[1].stageFlags = vk::ShaderStageFlagBits::eVertex;
    descSetBindings[1].pImmutableSamplers = NULL;

    vk::DescriptorSetLayoutCreateInfo descSetLayoutCI;
    descSetLayoutCI.bindingCount = 2;
    descSetLayoutCI.pBindings = descSetBindings;
    d.createDescriptorSetLayout(&descSetLayoutCI, NULL, &primaryDescriptorSetLayout);

    vk::PipelineLayoutCreateInfo pipelineLayoutCI;
    pipelineLayoutCI.pushConstantRangeCount = 0;
    pipelineLayoutCI.pPushConstantRanges = NULL;
    pipelineLayoutCI.setLayoutCount = 1;
    pipelineLayoutCI.pSetLayouts = &primaryDescriptorSetLayout;
    result = d.createPipelineLayout(&pipelineLayoutCI, NULL, &primaryPipelineLayout);

    vk::DescriptorSetAllocateInfo descSetAllocInfo;
    descSetAllocInfo.descriptorPool = primaryDescriptorPool;
    descSetAllocInfo.descriptorSetCount = 1;
    descSetAllocInfo.pSetLayouts = &primaryDescriptorSetLayout;
    result = d.allocateDescriptorSets(&descSetAllocInfo, &primaryDescriptorSet);
}


void Manager::setPrimaryVertexBuffer(std::vector<Vertex> vertices)
{
    vertexBuffer = Buffer(
        device, vk::BufferUsageFlagBits::eVertexBuffer,
        vertices.data(), vertices.size() * sizeof(vertices[0]));
    vertexCount = vertices.size();

    viBindingDesc.binding = 0;
    viBindingDesc.inputRate = vk::VertexInputRate::eVertex;
    viBindingDesc.stride = sizeof(vertices[0]);
}


void Manager::tempSetCameraParameters(float fovy, float nearClip, float farClip, vec3 pos, quat rot)
{
    mat4 model = glm::mat4(1.0f);
    mat4 I = glm::mat4(1);
    mat4 view = glm::translate(I, -pos); // *glm::toMat4(rot);

    auto extents = primaryGPU.GetSurfaceCapabilities().currentExtent;
    mat4 proj = glm::perspective(fovy, (float)extents.width / (float)extents.height, nearClip, farClip);

    // Vulkan clip space has inverted Y and half Z.
    mat4 clip = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);

    viewProjectionData.clip = clip;
    viewProjectionData.model = model;
    viewProjectionData.view = view;
    viewProjectionData.proj = proj;
    viewProjectionData.mvp = clip * proj * view * model;
    //viewProjectionBuffer = Buffer(device, vk::BufferUsageFlagBits::eVertexBuffer, &viewProjectionData, sizeof(viewProjectionData));
    viewProjectionBuffer.updateData(&viewProjectionData, sizeof(viewProjectionData));

    vk::DescriptorBufferInfo descBufferInfo;
    descBufferInfo.buffer = viewProjectionBuffer.Get();
    descBufferInfo.offset = 0;
    descBufferInfo.range = sizeof(viewProjectionData);

    vk::WriteDescriptorSet writeInfo;
    writeInfo.dstSet = primaryDescriptorSet;
    writeInfo.descriptorCount = 1;
    writeInfo.descriptorType = vk::DescriptorType::eUniformBuffer;
    writeInfo.pBufferInfo = &descBufferInfo;
    writeInfo.dstArrayElement = 0;
    writeInfo.dstBinding = 1;

    device.GetLogical().updateDescriptorSets(1, &writeInfo, 0, NULL);
}


void Manager::resize()
{

}


/// This should be called after the primary render pass and buffers (frame, vert, uniform, etc) have been created.
void Manager::tempPipelineStuff()
{
    auto d = device.GetLogical();

    // Create pipeline.
    vk::DynamicState dynamicStates[VK_DYNAMIC_STATE_RANGE_SIZE];
    vk::PipelineDynamicStateCreateInfo dynamicStateCI;
    memset(dynamicStates, 0, sizeof dynamicStates);

    dynamicStateCI.pDynamicStates = dynamicStates;
    dynamicStateCI.dynamicStateCount = 0;

    // Pipeline state CIs
    auto vertexAtributes = Vertex::AttributeDescs();
    vk::PipelineVertexInputStateCreateInfo vi;
    vi.vertexBindingDescriptionCount = 1;
    vi.pVertexBindingDescriptions = &viBindingDesc;
    vi.vertexAttributeDescriptionCount = (uint32_t)vertexAtributes.size();
    vi.pVertexAttributeDescriptions = vertexAtributes.data();

    vk::PipelineInputAssemblyStateCreateInfo ia;
    ia.primitiveRestartEnable = false;
    ia.topology = vk::PrimitiveTopology::eTriangleList;

    vk::PipelineTessellationStateCreateInfo ts;

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

    const int NUM_COLOR_BLEND_ATTACHMENT_STATES = 1;
    vk::PipelineColorBlendAttachmentState cBlendAttachmentState[NUM_COLOR_BLEND_ATTACHMENT_STATES];
    cBlendAttachmentState[0].colorWriteMask =
        vk::ColorComponentFlagBits::eA |
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB;
    cBlendAttachmentState[0].blendEnable = false;
    cBlendAttachmentState[0].alphaBlendOp = vk::BlendOp::eAdd;
    cBlendAttachmentState[0].colorBlendOp = vk::BlendOp::eAdd;
    cBlendAttachmentState[0].srcColorBlendFactor = vk::BlendFactor::eZero;
    cBlendAttachmentState[0].dstColorBlendFactor = vk::BlendFactor::eZero;
    cBlendAttachmentState[0].srcAlphaBlendFactor = vk::BlendFactor::eZero;
    cBlendAttachmentState[0].dstAlphaBlendFactor = vk::BlendFactor::eZero;

    vk::PipelineColorBlendStateCreateInfo cb;
    cb.attachmentCount = NUM_COLOR_BLEND_ATTACHMENT_STATES;
    cb.pAttachments = cBlendAttachmentState;
    cb.logicOpEnable = false;
    cb.logicOp = vk::LogicOp::eNoOp;
    cb.blendConstants[0] = 1.0f;
    cb.blendConstants[1] = 1.0f;
    cb.blendConstants[2] = 1.0f;
    cb.blendConstants[3] = 1.0f;

    vk::PipelineViewportStateCreateInfo vp = {};
    vp.viewportCount = 1;
    dynamicStates[dynamicStateCI.dynamicStateCount++] = vk::DynamicState::eViewport;
    vp.scissorCount = 1;
    dynamicStates[dynamicStateCI.dynamicStateCount++] = vk::DynamicState::eScissor;
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
    ms.rasterizationSamples = sampleCount;
    ms.sampleShadingEnable = false;
    ms.alphaToCoverageEnable = false;
    ms.alphaToOneEnable = false;
    ms.minSampleShading = 0.0;

    vk::Pipeline basePipeline;
    vk::GraphicsPipelineCreateInfo pipelineCI;
    pipelineCI.stageCount = (uint32_t)pipelineShaderStages.size();
    pipelineCI.pStages = pipelineShaderStages.data();
    pipelineCI.pVertexInputState = &vi;
    pipelineCI.pInputAssemblyState = &ia;
    pipelineCI.pTessellationState = &ts;
    pipelineCI.pViewportState = &vp;
    pipelineCI.pRasterizationState = &rs;
    pipelineCI.pMultisampleState = &ms;
    pipelineCI.pDepthStencilState = &ds;
    pipelineCI.pColorBlendState = &cb;
    pipelineCI.pDynamicState = &dynamicStateCI;
    pipelineCI.renderPass = primaryRenderPass;
    pipelineCI.subpass = 0;
    pipelineCI.layout = primaryPipelineLayout;
    pipelineCI.basePipelineHandle = basePipeline;
    pipelineCI.basePipelineIndex = 0;

    vk::PipelineCacheCreateInfo pipelineCacheCI;
    if (d.createPipelineCache(&pipelineCacheCI, NULL, &pipelineCache) != vk::Result::eSuccess)
        throw Exception("Failed to create pipeline cache.");
    vk::Result result = d.createGraphicsPipelines(pipelineCache, 1, &pipelineCI, NULL, &primaryPipeline);
    if (result != vk::Result::eSuccess) throw Exception("Unable to create graphics pipeline.");
}


void Manager::updateTimeBuffer(float timeSinceStart)
{
    timeBuffer.updateData(&timeSinceStart, sizeof(timeSinceStart));

    vk::DescriptorBufferInfo descBufferInfo;
    descBufferInfo.buffer = timeBuffer.Get();
    descBufferInfo.offset = 0;
    descBufferInfo.range = sizeof(timeSinceStart);

    vk::WriteDescriptorSet writeInfo;
    writeInfo.dstSet = primaryDescriptorSet;
    writeInfo.descriptorCount = 1;
    writeInfo.descriptorType = vk::DescriptorType::eUniformBuffer;
    writeInfo.pBufferInfo = &descBufferInfo;
    writeInfo.dstArrayElement = 0;
    writeInfo.dstBinding = 0;

    device.GetLogical().updateDescriptorSets(1, &writeInfo, 0, NULL);
}


void Manager::draw(Djn::Time& time)
{
    updateTimeBuffer(time.totalTime());

    auto d = device.GetLogical();
    // Command buffer etc
    vk::ClearValue clearValues[2];
    clearValues[0].color.float32[0] = 0.1f;
    clearValues[0].color.float32[1] = 0.1f;
    clearValues[0].color.float32[2] = 0.1f;
    clearValues[0].color.float32[3] = 0.1f;
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0;

    vk::SemaphoreCreateInfo semaphoreCI;
    vk::Semaphore imageAcqSem = d.createSemaphore(semaphoreCI);

    uint32_t imgIdx;
    vk::Result result = d.acquireNextImageKHR(swapchain.GetSwapchainKHR(), UINT64_MAX, imageAcqSem, vk::Fence(), &imgIdx);
    if (result != vk::Result::eSuccess) throw Exception(vk::to_string(result));

    auto extent = primaryGPU.GetSurfaceCapabilities().currentExtent;

    vk::RenderPassBeginInfo rpBeginInfo;
    rpBeginInfo.renderPass = primaryRenderPass;
    rpBeginInfo.framebuffer = primaryFramebuffer[imgIdx];
    rpBeginInfo.renderArea.offset.x = 0;
    rpBeginInfo.renderArea.offset.y = 0;
    rpBeginInfo.renderArea.extent = extent;
    rpBeginInfo.clearValueCount = 2;
    rpBeginInfo.pClearValues = clearValues;

    auto cmdBuffer = gfxCommandPool[0];
    vk::CommandBufferBeginInfo commandBufferBeginInfo;
    cmdBuffer.begin(commandBufferBeginInfo);
    cmdBuffer.beginRenderPass(&rpBeginInfo, vk::SubpassContents::eInline);
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, primaryPipeline);
    cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, primaryPipelineLayout,
        0, 1, &primaryDescriptorSet, 0, NULL);

    vk::DeviceSize offsets[1] = { 0 };
    cmdBuffer.bindVertexBuffers(0, 1, &vertexBuffer.Get(), offsets);
    vk::Viewport viewport;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    viewport.x = 0;
    viewport.y = 0;
    cmdBuffer.setViewport(0, 1, &viewport);
    vk::Rect2D scissor;
    scissor.extent.width = extent.width;
    scissor.extent.height = extent.height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    cmdBuffer.setScissor(0, 1, &scissor);
    cmdBuffer.draw(vertexCount, 1, 0, 0);
    cmdBuffer.endRenderPass();
    cmdBuffer.end();
    vk::CommandBuffer commandBuffers[1];
    commandBuffers[0] = cmdBuffer;

    auto fenceCI = vk::FenceCreateInfo();
    auto drawFence = d.createFence(fenceCI);
    vk::PipelineStageFlags pipelineStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submitInfo;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAcqSem;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffers;

    result = device.GetGraphicsQueue().submit(1, &submitInfo, drawFence);
    if (result != vk::Result::eSuccess) throw Exception("Unable to submit graphics queue.");

    auto swapchainKHR = swapchain.GetSwapchainKHR();
    vk::PresentInfoKHR presentInfoKHR;
    presentInfoKHR.swapchainCount = 1;
    presentInfoKHR.pSwapchains = &swapchainKHR;
    presentInfoKHR.pImageIndices = &imgIdx;

    do {
        result = d.waitForFences(1, &drawFence, true, 1000000000);
    } while (result == vk::Result::eTimeout);
    if (result != vk::Result::eSuccess) throw Exception(vk::to_string(result));

    result = device.GetPresentQueue().presentKHR(&presentInfoKHR);
    if (result != vk::Result::eSuccess) throw Exception(vk::to_string(result));

    d.destroyFence(drawFence);
    d.destroySemaphore(imageAcqSem);
}
