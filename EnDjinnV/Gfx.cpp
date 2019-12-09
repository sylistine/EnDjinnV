#include "EnDjinnV.h"

#include "Gfx.h"

#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "TempUtil.h"

using namespace Djn::Gfx;


void Manager::Initialize(VkInstance vkInstance, VkSurfaceKHR surface)
{
    if (gfxInstance != NULL) throw Exception("Gfx is already initialized.");

    gfxInstance = new Manager(vkInstance, surface);
}


Manager* Manager::gfxInstance = NULL;


Manager::Manager(VkInstance vkInstance, VkSurfaceKHR surface) :
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


    // Create vertex buffer.
    std::vector<Vertex> vertexList;
    vertexList.push_back(Vertex(vec4(0.f, 0.f, 0.f, 0.f), vec4(1.f, 0.f, 0.f, 0.f)));
    vertexList.push_back(Vertex(vec4(0.5f, 0.f, 0.f, 0.f), vec4(0.f, 1.f, 0.f, 0.f)));
    vertexList.push_back(Vertex(vec4(0.f, 0.5f, 0.f, 0.f), vec4(0.f, 0.f, 1.f, 0.f)));
    Buffer vertexBuffer(
        device, vk::BufferUsageFlagBits::eVertexBuffer,
        vertexList.data(), vertexList.size() * sizeof(vertexList[0]));

    VkVertexInputBindingDescription vertexInputBindingDesc = {};
    vertexInputBindingDesc.binding = 0;
    vertexInputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexInputBindingDesc.stride = sizeof(vertexList[0]);
}


Manager::~Manager()
{
    vkDestroyShaderModule(device.GetLogical(), fragmentShaderModule, NULL);
    vkDestroyShaderModule(device.GetLogical(), vertexShaderModule, NULL);

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
    auto cmdBufferInheritanceInfo = VkUtil::CommandBufferInheritanceInfo();
    cmdBufferInheritanceInfo.renderPass = primaryRenderPass;
    //fillout
    auto cmdBufferBeginInfo = VkUtil::CommandBufferBeginInfo();
    cmdBufferBeginInfo.pInheritanceInfo = &cmdBufferInheritanceInfo;
    vkBeginCommandBuffer(gfxCommandPool[0], &cmdBufferBeginInfo);
}
