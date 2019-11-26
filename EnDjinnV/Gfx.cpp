#include "EnDjinnV.h"

#include "Gfx.h"

#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "TempUtil.h"

using namespace Djn::Gfx;


void Manager::Initialize(VkInstance vkInstance, VkSurfaceKHR surface)
{
    if (gfxInstance != NULL) throw std::exception("Gfx is already initialized.");

    gfxInstance = new Manager(vkInstance, surface);
}


Manager* Manager::gfxInstance = NULL;


Manager::Manager(VkInstance vkInstance, VkSurfaceKHR surface) :
    instance(vkInstance),
    primaryGPU(VkUtil::GetDefaultPhysicalDevice(instance), surface),
    device(primaryGPU)
{
    VkResult result;
    
    auto gfxQueueFamilyIdx = primaryGPU.GetGraphicsQueueFamilyIndex();
    auto presentQueueFamilyIdx = primaryGPU.GetPresentQueueFamilyIndex();
    gfxCommandPool = CommandPool(device.GetLogical(), gfxQueueFamilyIdx, 1);

    std::vector<uint32_t> queueFamilyIndices;
    if (gfxQueueFamilyIdx != presentQueueFamilyIdx) {
        queueFamilyIndices.push_back(gfxQueueFamilyIdx);
        queueFamilyIndices.push_back(presentQueueFamilyIdx);
    }
    swapchain = Swapchain(device.GetLogical(), surface, primaryGPU.GetOutputFormat(),
        primaryGPU.GetSurfaceCapabilities(), queueFamilyIndices);

    // create depth texture
    depthTexture = new DepthTexture(device, 512, 512);

    /*
     * Create VkRenderPass
     */
    auto semaphoreCI = VkUtil::SemaphoreCI();
    result = vkCreateSemaphore(device.GetLogical(), &semaphoreCI, NULL, &imageAcquiredSemaphore);
    if (result != VK_SUCCESS) throw std::exception("Unable to create semaphore for image acquisition.");

    uint32_t bufferIdx;
    result = vkAcquireNextImageKHR(
        device.GetLogical(),
        swapchain.GetSwapchainKHR(),
        UINT64_MAX,
        imageAcquiredSemaphore,
        VK_NULL_HANDLE,
        &bufferIdx);

    const uint32_t attachmentDescCount = 2;
    VkAttachmentDescription attachmentDescs[attachmentDescCount];
    attachmentDescs[0] = VkUtil::AttachmentDescription();
    attachmentDescs[0].format = primaryGPU.GetOutputFormat();
    attachmentDescs[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescs[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescs[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescs[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescs[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescs[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescs[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    attachmentDescs[1] = VkUtil::AttachmentDescription();
    attachmentDescs[1].format = VK_FORMAT_D16_UNORM;
    attachmentDescs[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescs[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescs[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescs[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescs[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescs[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescs[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDesc = {};
    subpassDesc.flags = 0;
    subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc.inputAttachmentCount = 0;
    subpassDesc.pInputAttachments = NULL;
    subpassDesc.colorAttachmentCount = 1;
    subpassDesc.pColorAttachments = &colorReference;
    subpassDesc.pResolveAttachments = NULL;
    subpassDesc.pDepthStencilAttachment = &depthReference;
    subpassDesc.preserveAttachmentCount = 0;
    subpassDesc.pPreserveAttachments = NULL;

    auto renderpassCI = VkUtil::RenderPassCI();
    renderpassCI.attachmentCount = attachmentDescCount;
    renderpassCI.pAttachments = attachmentDescs;
    renderpassCI.subpassCount = 1;
    renderpassCI.pSubpasses = &subpassDesc;
    renderpassCI.dependencyCount = 0;
    renderpassCI.pDependencies = NULL;

    result = vkCreateRenderPass(device.GetLogical(), &renderpassCI, NULL, &renderPass);
    if (result != VK_SUCCESS) {
        vkDestroySemaphore(device.GetLogical(), imageAcquiredSemaphore, NULL);
    }

    // TODO: setup shader workflow.
    // in the mean time...
    shaderc::Compiler compiler;
    auto vertexShader = CompileShader(compiler, BasicVertexShader);
    auto fragmentShader = CompileShader(compiler, BasicFragmentShader);

    auto vsModuleCI = VkUtil::ShaderModuleCI();
    vsModuleCI.codeSize = vertexShader.size() * sizeof(unsigned int);
    vsModuleCI.pCode = vertexShader.data();
    result = vkCreateShaderModule(device.GetLogical(), &vsModuleCI, NULL, &vertexShaderModule);
    if (result != VK_SUCCESS) {
        vkDestroyRenderPass(device.GetLogical(), renderPass, NULL);
        vkDestroySemaphore(device.GetLogical(), imageAcquiredSemaphore, NULL);
        throw std::exception("Unable to create vertex shader module.");
    }

    auto fsModuleCI = VkUtil::ShaderModuleCI();
    fsModuleCI.codeSize = fragmentShader.size() * sizeof(unsigned int);
    fsModuleCI.pCode = fragmentShader.data();
    result = vkCreateShaderModule(device.GetLogical(), &fsModuleCI, NULL, &fragmentShaderModule);
    if (result != VK_SUCCESS) {
        vkDestroyShaderModule(device.GetLogical(), vertexShaderModule, NULL);
        vkDestroyRenderPass(device.GetLogical(), renderPass, NULL);
        vkDestroySemaphore(device.GetLogical(), imageAcquiredSemaphore, NULL);
        throw std::exception("Unable to create fragment shader module.");
    }

    auto vsPipelineShaderStageCI = VkUtil::PipelineShaderStageCI();
    vsPipelineShaderStageCI.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vsPipelineShaderStageCI.module = vertexShaderModule;
    vsPipelineShaderStageCI.pName = "main";
    vsPipelineShaderStageCI.pSpecializationInfo = NULL;

    auto fsPipelineShaderStageCI = VkUtil::PipelineShaderStageCI();
    fsPipelineShaderStageCI.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fsPipelineShaderStageCI.module = fragmentShaderModule;
    fsPipelineShaderStageCI.pName = "main";
    fsPipelineShaderStageCI.pSpecializationInfo = NULL;

    std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStages;
    pipelineShaderStages.push_back(vsPipelineShaderStageCI);
    pipelineShaderStages.push_back(fsPipelineShaderStageCI);

    // Build frame buffer...
    VkImageView attachments[2];
    attachments[1] = depthTexture->GetView();

    auto framebufferCI = VkUtil::FrameBufferCI();
    framebufferCI.renderPass = renderPass;
    framebufferCI.attachmentCount = 2;
    framebufferCI.pAttachments = attachments;
    framebufferCI.width = 512;
    framebufferCI.height = 512;
    framebufferCI.layers = 1;

    framebuffer = (VkFramebuffer*)malloc(swapchain.GetImageCount() * sizeof(VkFramebuffer));
    for (auto i = 0u; i < swapchain.GetImageCount(); i++) {
        attachments[0] = swapchain.GetImageView(i);
        result = vkCreateFramebuffer(device.GetLogical(), &framebufferCI, NULL, &framebuffer[i]);
        if (result != VK_SUCCESS) {
            vkDestroyShaderModule(device.GetLogical(), fragmentShaderModule, NULL);
            vkDestroyShaderModule(device.GetLogical(), vertexShaderModule, NULL);
            vkDestroyRenderPass(device.GetLogical(), renderPass, NULL);
            vkDestroySemaphore(device.GetLogical(), imageAcquiredSemaphore, NULL);
            throw std::exception("Unable to create framebuffer.");
        }
    }

    // TODO: mvp buffer and vertex buffers will be setup
    // AFTER graphics initialization in the future.
    // Create Uniform MVP buffer.
    mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    mat4 viewMatrix = glm::lookAt(
        glm::vec3(-5, 3, -10),  // Camera is at (-5,3,-10), in World Space
        glm::vec3(0, 0, 0),     // and looks at the origin
        glm::vec3(0, -1, 0)     // Head is up (set to 0,-1,0 to look upside-down)
    );
    mat4 modelMatrix = mat4(1.0f);
    mat4 clipMatrix = mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
    mat4 mvp = clipMatrix * projectionMatrix * viewMatrix * modelMatrix;
    Buffer mvpBuffer(
        device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        &mvp, sizeof(mvp));

    // Create vertex buffer.
    std::vector<Vertex> vertexList;
    vertexList.push_back(Vertex(vec4(0.f, 0.f, 0.f, 0.f), vec4(1.f, 0.f, 0.f, 0.f)));
    vertexList.push_back(Vertex(vec4(0.5f, 0.f, 0.f, 0.f), vec4(0.f, 1.f, 0.f, 0.f)));
    vertexList.push_back(Vertex(vec4(0.f, 0.5f, 0.f, 0.f), vec4(0.f, 0.f, 1.f, 0.f)));
    Buffer vertexBuffer(
        device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        vertexList.data(), vertexList.size() * sizeof(vertexList[0]));

    VkVertexInputBindingDescription vertexInputBindingDesc = {};
    vertexInputBindingDesc.binding = 0;
    vertexInputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexInputBindingDesc.stride = sizeof(vertexList[0]);

    // Create pipeline.
    /*VkPipelineLayoutCreateInfo newCI = vk::PipelineLayoutCreateInfo();
    newCI.pushConstantRangeCount = 0;
    newCI.pPushConstantRanges = NULL;
    newCI.setLayoutCount = 1;
    newCI.pSetLayouts = info.desc_layout.data();
    
    VkPipelineLayout pipelineLayout;


    VkPipeline pipeline;
    Djn::Gfx::PipelineCodeUtil(
        device,
        vertexInputBindingDesc,
        Vertex::AttributeDescs(),
        pipelineLayout,
        pipelineShaderStages,
        renderPass,
        &pipeline);*/

    /*
     * Build command buffer *INCOMPLETE*
     */
    auto cmdBufferInheritanceInfo = VkUtil::CommandBufferInheritanceInfo();
    cmdBufferInheritanceInfo.renderPass = renderPass;
    //fillout
    auto cmdBufferBeginInfo = VkUtil::CommandBufferBeginInfo();
    cmdBufferBeginInfo.pInheritanceInfo = &cmdBufferInheritanceInfo;
    vkBeginCommandBuffer(gfxCommandPool[0], &cmdBufferBeginInfo);
}


Manager::~Manager()
{
    for (auto i = 0u; i < swapchain.GetImageCount(); i++) {
        vkDestroyFramebuffer(device.GetLogical(), framebuffer[i], NULL);
    }
    vkDestroyShaderModule(device.GetLogical(), fragmentShaderModule, NULL);
    vkDestroyShaderModule(device.GetLogical(), vertexShaderModule, NULL);
    vkDestroyRenderPass(device.GetLogical(), renderPass, NULL);
    vkDestroySemaphore(device.GetLogical(), imageAcquiredSemaphore, NULL);

    delete depthTexture;
}


std::vector<unsigned int> Manager::CompileShader(
    shaderc::Compiler& compiler,
    Shader shader)
{
    auto program = compiler.CompileGlslToSpv(
        shader.shader, strlen(shader.shader),
        shader.kind, shader.name);
    if (program.GetCompilationStatus() != shaderc_compilation_status_success) {
        switch (program.GetCompilationStatus()) {
        case shaderc_compilation_status_invalid_stage:
            std::cout << "Stage deduction failure." << std::endl;
            break;
        case shaderc_compilation_status_compilation_error:
            std::cout << "Compilation error." << std::endl;
            break;
        case shaderc_compilation_status_internal_error:
            std::cout << "Unexpected internal failure." << std::endl;
            break;
        case shaderc_compilation_status_null_result_object:
            std::cout << "null result object." << std::endl;
            break;
        case shaderc_compilation_status_invalid_assembly:
            std::cout << "Invalid assembly." << std::endl;
            break;
        case shaderc_compilation_status_validation_error:
            std::cout << "Validation error." << std::endl;
            break;
        case shaderc_compilation_status_transformation_error:
            std::cout << "Transformation error." << std::endl;
            break;
        case shaderc_compilation_status_success:
        default:
            std::cout << "Compilation success!" << std::endl;
            break;
        }
        if (program.GetNumErrors() > 0) {
            std::cout << "Detected errors during vertex program compilation." << std::endl;
            std::cout << program.GetErrorMessage() << std::endl;
        }
        throw std::exception("error compiling shaders");
    }
    std::vector<unsigned int> data;
    for (auto it = program.cbegin(); it != nullptr && it != program.cend(); it++) {
        data.push_back(*it);
    }

    return data;
}
