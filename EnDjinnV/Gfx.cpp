#include "EnDjinnV.h"

#include "Gfx.h"

#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

using namespace Djn::Gfx;


// Begin Graphics Manager Statics
void Manager::Initialize(vk::Instance vkInstance, vk::SurfaceKHR surface)
{
    if (gfxInstance != NULL) throw Exception("Gfx is already initialized.");
    gfxInstance = new Manager(vkInstance, surface);
}


void Manager::SetViewProjectionMatrices(mat4 viewMatrix, mat4 projectionMatrix)
{
    if (gfxInstance == NULL) throw Exception("Graphics has not been initialized.");
    gfxInstance->SetPrimaryViewProjectionMatrices(viewMatrix, projectionMatrix);
}


void Manager::SetVertices(std::vector<Vertex> vertices)
{
    if (gfxInstance == NULL) throw Exception("Graphics has not been initialized.");
    gfxInstance->SetPrimaryVertexBuffer(vertices);
}


void Manager::TempBuildAndRunPipeline()
{
    if (gfxInstance == NULL) throw Exception("Graphics has not been initialized.");
    gfxInstance->TempPipelineStuff();
}


Manager* Manager::gfxInstance = NULL;
// End Graphics Manager Statics


Manager::Manager(vk::Instance vkInstance, vk::SurfaceKHR surface) :
    instance(vkInstance),
    primaryGPU(instance.enumeratePhysicalDevices()[0], surface),
    device(primaryGPU)
{
    vk::Result result;
    
    auto gfxQueueFamilyIdx = primaryGPU.GetGraphicsQueueFamilyIndex();
    auto presentQueueFamilyIdx = primaryGPU.GetPresentQueueFamilyIndex();
    gfxCommandPool = CommandPool(device.GetLogical(), gfxQueueFamilyIdx, 1);

    // Create default swapchain and depth texture.
    std::vector<uint32_t> queueFamilyIndices;
    if (gfxQueueFamilyIdx != presentQueueFamilyIdx) {
        queueFamilyIndices.push_back(gfxQueueFamilyIdx);
        queueFamilyIndices.push_back(presentQueueFamilyIdx);
    }
    swapchain = Swapchain(device.GetLogical(), surface, primaryGPU.GetOutputFormat(),
        primaryGPU.GetSurfaceCapabilities(), queueFamilyIndices);

    // create depth texture
    depthTexture = new DepthTexture(device, 512, 512);

    // Shader Module Setup phase
    // TODO: This should not be handled in graphics initialization.
    // It should be done before this binary gets built.
    auto vertexShader = Shader::LoadBasicVertexShader();
    auto fragmentShader = Shader::LoadBasicFragmentShader();

    vk::ShaderModuleCreateInfo vsModuleCI;
    vsModuleCI.codeSize = vertexShader.size() * sizeof(unsigned int);
    vsModuleCI.pCode = vertexShader.data();
    result = device.GetLogical().createShaderModule(&vsModuleCI, NULL, &vertexShaderModule);
    if (result != vk::Result::eSuccess) {
        throw Exception("Unable to create vertex shader module.");
    }

    vk::ShaderModuleCreateInfo fsModuleCI;
    fsModuleCI.codeSize = fragmentShader.size() * sizeof(unsigned int);
    fsModuleCI.pCode = fragmentShader.data();

    result = device.GetLogical().createShaderModule(&fsModuleCI, NULL, &fragmentShaderModule);
    if (result != vk::Result::eSuccess) {
        device.GetLogical().destroyShaderModule(vertexShaderModule);
        throw Exception("Unable to create fragment shader module.");
    }

    vk::PipelineShaderStageCreateInfo vsPipelineShaderStageCI;
    vsPipelineShaderStageCI.stage = vk::ShaderStageFlagBits::eVertex;
    vsPipelineShaderStageCI.module = vertexShaderModule;
    vsPipelineShaderStageCI.pName = "main";
    vsPipelineShaderStageCI.pSpecializationInfo = NULL;

    vk::PipelineShaderStageCreateInfo  fsPipelineShaderStageCI;
    fsPipelineShaderStageCI.stage = vk::ShaderStageFlagBits::eFragment;
    fsPipelineShaderStageCI.module = fragmentShaderModule;
    fsPipelineShaderStageCI.pName = "main";
    fsPipelineShaderStageCI.pSpecializationInfo = NULL;

    pipelineShaderStages.push_back(vsPipelineShaderStageCI);
    pipelineShaderStages.push_back(fsPipelineShaderStageCI);

    SetupPrimaryRenderPass();
}


Manager::~Manager()
{
    TeardownPrimaryRenderPass();

    auto d = device.GetLogical();
    d.destroyShaderModule(fragmentShaderModule);
    d.destroyShaderModule(vertexShaderModule);
    delete depthTexture;
}


//std::vector<unsigned int> Manager::CompileShader(
//    shaderc::Compiler& compiler,
//    Shader shader)
//{
//    auto program = compiler.CompileGlslToSpv(
//        shader.shader, strlen(shader.shader),
//        shader.kind, shader.name);
//    if (program.GetCompilationStatus() != shaderc_compilation_status_success) {
//        switch (program.GetCompilationStatus()) {
//        case shaderc_compilation_status_invalid_stage:
//            std::cout << "Stage deduction failure." << std::endl;
//            break;
//        case shaderc_compilation_status_compilation_error:
//            std::cout << "Compilation error." << std::endl;
//            break;
//        case shaderc_compilation_status_internal_error:
//            std::cout << "Unexpected internal failure." << std::endl;
//            break;
//        case shaderc_compilation_status_null_result_object:
//            std::cout << "null result object." << std::endl;
//            break;
//        case shaderc_compilation_status_invalid_assembly:
//            std::cout << "Invalid assembly." << std::endl;
//            break;
//        case shaderc_compilation_status_validation_error:
//            std::cout << "Validation error." << std::endl;
//            break;
//        case shaderc_compilation_status_transformation_error:
//            std::cout << "Transformation error." << std::endl;
//            break;
//        case shaderc_compilation_status_success:
//        default:
//            std::cout << "Compilation success!" << std::endl;
//            break;
//        }
//        if (program.GetNumErrors() > 0) {
//            std::cout << "Detected errors during vertex program compilation." << std::endl;
//            std::cout << program.GetErrorMessage() << std::endl;
//        }
//        throw Exception("error compiling shaders");
//    }
//    std::vector<unsigned int> data;
//    for (auto it = program.cbegin(); it != nullptr && it != program.cend(); it++) {
//        data.push_back(*it);
//    }
//
//    return data;
//}


void Manager::SetupPrimaryRenderPass()
{
    auto d = device.GetLogical();

    const uint32_t attachmentDescCount = 2;
    vk::AttachmentDescription attachmentDescs[attachmentDescCount];
    // Primary color attachment.
    attachmentDescs[0].format = primaryGPU.GetOutputFormat();
    attachmentDescs[0].samples = vk::SampleCountFlagBits::e1;
    attachmentDescs[0].loadOp = vk::AttachmentLoadOp::eClear;
    attachmentDescs[0].storeOp = vk::AttachmentStoreOp::eStore;
    attachmentDescs[0].stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachmentDescs[0].stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDescs[0].initialLayout = vk::ImageLayout::eUndefined;
    attachmentDescs[0].finalLayout = vk::ImageLayout::ePresentSrcKHR;
    // Depth attachment.
    attachmentDescs[1].format = vk::Format::eD16Unorm;
    attachmentDescs[1].samples = vk::SampleCountFlagBits::e1;
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

    vk::FramebufferCreateInfo framebufferCI;
    framebufferCI.renderPass = primaryRenderPass;
    framebufferCI.attachmentCount = 2;
    framebufferCI.pAttachments = attachments;
    framebufferCI.width = 512;
    framebufferCI.height = 512;
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
    vk::DescriptorSetLayoutBinding descSetBindings;
    descSetBindings.binding = 0;
    descSetBindings.descriptorType = vk::DescriptorType::eUniformBuffer;
    descSetBindings.descriptorCount = 1;
    descSetBindings.stageFlags = vk::ShaderStageFlagBits::eVertex;
    descSetBindings.pImmutableSamplers = NULL;

    vk::DescriptorSetLayoutCreateInfo descSetLayoutCI;
    descSetLayoutCI.bindingCount = 1;
    descSetLayoutCI.pBindings = &descSetBindings;
    d.createDescriptorSetLayout(&descSetLayoutCI, NULL, &primaryDescriptorSetLayout);

    vk::PipelineLayoutCreateInfo pipelineLayoutCI;
    pipelineLayoutCI.pushConstantRangeCount = 0;
    pipelineLayoutCI.pPushConstantRanges = NULL;
    pipelineLayoutCI.setLayoutCount = 1;
    pipelineLayoutCI.pSetLayouts = &primaryDescriptorSetLayout;
    result = d.createPipelineLayout(&pipelineLayoutCI, NULL, &primaryPipelineLayout);

    // Allocate the actual set.
    vk::DescriptorPoolSize descPoolSize;
    descPoolSize.descriptorCount = 1;

    vk::DescriptorPoolCreateInfo descPoolCI;
    descPoolCI.maxSets = 1;
    descPoolCI.poolSizeCount = 1;
    descPoolCI.pPoolSizes = &descPoolSize;

    result = d.createDescriptorPool(&descPoolCI, NULL, &primaryDescriptorPool);

    vk::DescriptorSetAllocateInfo descSetAllocInfo;
    descSetAllocInfo.descriptorPool = primaryDescriptorPool;
    descSetAllocInfo.descriptorSetCount = 1;
    descSetAllocInfo.pSetLayouts = &primaryDescriptorSetLayout;
    result = d.allocateDescriptorSets(&descSetAllocInfo, &primaryDescriptorSet);
}


void Manager::TeardownPrimaryRenderPass()
{
    auto d = device.GetLogical();
    for (auto i = 0u; i < swapchain.GetImageCount(); i++) {
        d.destroyFramebuffer(primaryFramebuffer[i]);
    }
    free(primaryFramebuffer);
    d.destroyRenderPass(primaryRenderPass);
}


void Manager::SetPrimaryVertexBuffer(std::vector<Vertex> vertices)
{
    vertexBuffer = Buffer(
        device, vk::BufferUsageFlagBits::eVertexBuffer,
        vertices.data(), vertices.size() * sizeof(vertices[0]));

    viBindingDesc.binding = 0;
    viBindingDesc.inputRate = vk::VertexInputRate::eVertex;
    viBindingDesc.stride = sizeof(vertices[0]);
}


void Manager::SetPrimaryViewProjectionMatrices(mat4 viewMatrix, mat4 projectionMatrix)
{
    auto viewAndProjectionMatrices = { viewMatrix, projectionMatrix };
    // TODO: this buffer should probably only be created once, with contents updated each frame.
    viewProjectionBuffer = Buffer(
        device, vk::BufferUsageFlagBits::eUniformBuffer,
        &viewAndProjectionMatrices, sizeof(mat4) * 2);

    // Write to the set.
    vk::DescriptorBufferInfo descBufferInfo;
    descBufferInfo.buffer = viewProjectionBuffer.Get();
    descBufferInfo.offset = 0;
    descBufferInfo.range = sizeof(mat4) * 2;

    vk::WriteDescriptorSet writeInfo;
    writeInfo.dstSet = primaryDescriptorSet;
    writeInfo.descriptorCount = 1;
    writeInfo.descriptorType = vk::DescriptorType::eUniformBuffer;
    writeInfo.pBufferInfo = &descBufferInfo;
    writeInfo.dstArrayElement = 0;
    writeInfo.dstBinding = 0;

    device.GetLogical().updateDescriptorSets(1, &writeInfo, 0, NULL);
}


/// This should be called after the primary render pass and buffers (frame, vert, uniform, etc) have been created.
void Manager::TempPipelineStuff()
{
    auto d = device.GetLogical();

    // Create pipeline.
    vk::DynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
    vk::PipelineDynamicStateCreateInfo dynamicState;
    memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);

    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 0;

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

    vk::PipelineColorBlendAttachmentState cBlendAttachmentState[1];
    cBlendAttachmentState[0].colorWriteMask = vk::ColorComponentFlags(0xf);
    cBlendAttachmentState[0].blendEnable = false;
    cBlendAttachmentState[0].alphaBlendOp = vk::BlendOp::eAdd;
    cBlendAttachmentState[0].colorBlendOp = vk::BlendOp::eAdd;
    cBlendAttachmentState[0].srcColorBlendFactor = vk::BlendFactor::eZero;
    cBlendAttachmentState[0].dstColorBlendFactor = vk::BlendFactor::eZero;;
    cBlendAttachmentState[0].srcAlphaBlendFactor = vk::BlendFactor::eZero;;
    cBlendAttachmentState[0].dstAlphaBlendFactor = vk::BlendFactor::eZero;;

    vk::PipelineColorBlendStateCreateInfo cb;
    cb.attachmentCount = 1;
    cb.pAttachments = cBlendAttachmentState;
    cb.logicOpEnable = false;
    cb.logicOp = vk::LogicOp::eNoOp;
    cb.blendConstants[0] = 1.0f;
    cb.blendConstants[1] = 1.0f;
    cb.blendConstants[2] = 1.0f;
    cb.blendConstants[3] = 1.0f;

    vk::PipelineViewportStateCreateInfo vp = {};
    vp.viewportCount = 1;
    dynamicStateEnables[dynamicState.dynamicStateCount++] = vk::DynamicState::eViewport;
    vp.scissorCount = 1;
    dynamicStateEnables[dynamicState.dynamicStateCount++] = vk::DynamicState::eScissor;
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
    ms.sampleShadingEnable = false;
    ms.alphaToCoverageEnable = false;
    ms.alphaToOneEnable = false;
    ms.minSampleShading = 0.0;

    vk::Pipeline basePipeline;
    vk::GraphicsPipelineCreateInfo pipelineCI;
    pipelineCI.layout = primaryPipelineLayout;
    pipelineCI.basePipelineHandle = basePipeline;
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
    pipelineCI.stageCount = (uint32_t)pipelineShaderStages.size();
    pipelineCI.pStages = pipelineShaderStages.data();
    pipelineCI.renderPass = primaryRenderPass;
    pipelineCI.subpass = 0;

    vk::PipelineCache pipelineCache;
    vk::Result result = d.createGraphicsPipelines(pipelineCache, 1, &pipelineCI, NULL, &primaryPipeline);
    if (result != vk::Result::eSuccess) throw Exception("Unable to create graphics pipeline.");

    // Command buffer etc
    vk::ClearColorValue clearValueColor;
    vk::ClearValue clearValue;
    clearValue.color = clearValueColor;
    clearValue.depthStencil.depth = 1.0f;
    clearValue.depthStencil.stencil = 0;

    vk::SemaphoreCreateInfo semaphoreCI;
    vk::Semaphore imageAcqSem = d.createSemaphore(semaphoreCI);

    uint32_t imgIdx;
    result = d.acquireNextImageKHR(swapchain.GetSwapchainKHR(), UINT64_MAX, imageAcqSem, vk::Fence(), &imgIdx);
    if (result != vk::Result::eSuccess) throw Exception(vk::to_string(result));

    vk::RenderPassBeginInfo rpBeginInfo;
    rpBeginInfo.renderPass = primaryRenderPass;
    rpBeginInfo.framebuffer = primaryFramebuffer[imgIdx];
    rpBeginInfo.renderArea.offset.x = 0;
    rpBeginInfo.renderArea.offset.y = 0;
    rpBeginInfo.renderArea.extent.width = 512;
    rpBeginInfo.renderArea.extent.height = 512;
    rpBeginInfo.clearValueCount = 1;
    rpBeginInfo.pClearValues = &clearValue;

    auto cmdBuffer = gfxCommandPool[0];
    vk::CommandBufferBeginInfo commandBufferBeginInfo;
    cmdBuffer.begin(commandBufferBeginInfo);
    cmdBuffer.beginRenderPass(&rpBeginInfo, vk::SubpassContents::eInline);
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, primaryPipeline);
    cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, primaryPipelineLayout,
        0, 1, &primaryDescriptorSet, 0, NULL);
    std::cout << &vertexBuffer.Get() << std::endl;
    vk::DeviceSize offsets[1];
    offsets[0] = 0;
    cmdBuffer.bindVertexBuffers(0, 1, &vertexBuffer.Get(), offsets);
    vk::Viewport viewport;
    viewport.width = 512;
    viewport.height = 512;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    viewport.x = 0;
    viewport.y = 0;
    cmdBuffer.setViewport(0, 1, &viewport);
    vk::Rect2D scissor;
    scissor.extent.width = 512;
    scissor.extent.height = 512;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    cmdBuffer.setScissor(0, 1, &scissor);
    cmdBuffer.draw(3, 1, 0, 0);
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

    vk::PresentInfoKHR presentInfoKHR;
    presentInfoKHR.swapchainCount = 1;
    presentInfoKHR.pSwapchains = &swapchain.GetSwapchainKHR();
    presentInfoKHR.pImageIndices = &imgIdx;

    do {
        result = d.waitForFences(1, &drawFence, true, 1000000000);
    } while (result == vk::Result::eTimeout);
    std::cout << "Waited for fences. Final result: " << vk::to_string(result) << std::endl;

    result = device.GetPresentQueue().presentKHR(&presentInfoKHR);
    if (result != vk::Result::eSuccess) throw Exception(vk::to_string(result));

    // TODO:: Teardown pass that cleans up all the junk we created here.
}



