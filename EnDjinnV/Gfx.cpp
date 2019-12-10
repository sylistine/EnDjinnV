#include "EnDjinnV.h"

#include "Gfx.h"

#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "TempUtil.h"

using namespace Djn::Gfx;


// Begin Graphics Manager Statics
void Manager::Initialize(vk::Instance vkInstance, vk::SurfaceKHR surface)
{
    if (gfxInstance != NULL) throw Exception("Gfx is already initialized.");
    gfxInstance = new Manager(vkInstance, surface);
}


void Manager::SetViewProjectionMatrices(mat4 viewMatrix, mat4 projectionMatrix)
{

}


void Manager::SetVertices(std::vector<Vertex> vertices)
{
    if (gfxInstance == NULL) throw Exception("Graphics has not been initialized.");
    gfxInstance->SetPrimaryVertexBuffer(vertices);
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
    shaderc::Compiler compiler;
    auto vertexShader = CompileShader(compiler, BasicVertexShader);
    auto fragmentShader = CompileShader(compiler, BasicFragmentShader);

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

    std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStages;
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
        throw Exception("error compiling shaders");
    }
    std::vector<unsigned int> data;
    for (auto it = program.cbegin(); it != nullptr && it != program.cend(); it++) {
        data.push_back(*it);
    }

    return data;
}


Buffer Manager::CreateUniformBuffer(void* data, size_t size)
{
    return Buffer(
        device, vk::BufferUsageFlagBits::eUniformBuffer,
        data, size);
}


void Manager::SetupPrimaryRenderPass()
{
    auto d = device.GetLogical();

    vk::SemaphoreCreateInfo semaphoreCI;
    auto semaphore = d.createSemaphore(semaphoreCI);
    
    auto bufferIdx = d.acquireNextImageKHR(
        swapchain.GetSwapchainKHR(),
        UINT64_MAX,
        semaphore,
        vk::Fence());

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

    d.destroySemaphore(semaphore, NULL);

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
    vk::CommandBufferInheritanceInfo cmdBufferInheritanceInfo;
    cmdBufferInheritanceInfo.renderPass = primaryRenderPass;
    //fillout
    vk::CommandBufferBeginInfo cmdBufferBeginInfo;
    cmdBufferBeginInfo.pInheritanceInfo = &cmdBufferInheritanceInfo;
    
    gfxCommandPool[0].begin(&cmdBufferBeginInfo);
}


void Manager::TeardownPrimaryRenderPass()
{
    auto d = device.GetLogical();
    for (auto i = 0u; i < swapchain.GetImageCount(); i++) {
        d.destroyFramebuffer(primaryFramebuffer[i]);
    }
    d.destroyRenderPass(primaryRenderPass);
}


void Manager::SetPrimaryVertexBuffer(std::vector<Vertex> vertices)
{
    Buffer vertexBuffer(
        device, vk::BufferUsageFlagBits::eVertexBuffer,
        vertices.data(), vertices.size() * sizeof(vertices[0]));

    vk::VertexInputBindingDescription vertexInputBindingDesc = {};
    vertexInputBindingDesc.binding = 0;
    vertexInputBindingDesc.inputRate = vk::VertexInputRate::eVertex;
    vertexInputBindingDesc.stride = sizeof(vertices[0]);
}
