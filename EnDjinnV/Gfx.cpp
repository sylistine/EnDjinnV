#include "EnDjinnV.h"

#include "Gfx.h"

#include <vector>

using namespace Djn::Gfx;


void Manager::Initialize(VkInstance vkInstance, VkSurfaceKHR surface)
{
    if (gfxInstance != NULL) throw std::exception("Gfx is already initialized.");

    gfxInstance = new Manager(vkInstance, surface);
}


Manager* Manager::gfxInstance = NULL;


Manager::Manager(VkInstance vkInstance, VkSurfaceKHR surface) : instance(vkInstance), surface(surface)
{
    VkResult result;

    // todo: better physical device selection.
    primaryGPU = PhysicalDevice(VkUtil::GetDefaultPhysicalDevice(instance));

    // get surface capabilities against chosen device
    uint32_t gfxQueueFamilyIdx;
    uint32_t presentQueueFamilyIdx;
    VkUtil::GetGfxAndPresentQueueFamilyIndex(
        primaryGPU.device,
        primaryGPU.queueFamilyProperties,
        surface,
        gfxQueueFamilyIdx,
        presentQueueFamilyIdx);
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(primaryGPU.device, surface, &surfaceCapabilities);
    if (result != VK_SUCCESS) throw std::exception("Unable to get surface capabilities.");
    
    auto physicalDeviceSurfacePresentModes = VkUtil::GetPhysicalDeviceSurfacePresentModes(primaryGPU.device, surface);
    auto physicalDeviceSurfaceFormats = VkUtil::GetPhysicalDeviceSurfaceFormats(primaryGPU.device, surface);
    if (physicalDeviceSurfaceFormats.size() < 1) throw std::exception("Unable to determine surface formats.");
    VkFormat swapchainFormat = physicalDeviceSurfaceFormats[0].format;
    if (swapchainFormat == VK_FORMAT_UNDEFINED) {
        swapchainFormat = VK_FORMAT_B8G8R8A8_UNORM;
    }

    device = Device(gfxQueueFamilyIdx, primaryGPU.device);
    cmdPool = CommandPool(device.Get(), gfxQueueFamilyIdx);
    cmdBuffer = CommandBuffer(device.Get(), cmdPool.Get());
    std::vector<uint32_t> queueFamilyIndices;
    if (gfxQueueFamilyIdx != presentQueueFamilyIdx) {
        queueFamilyIndices.push_back(gfxQueueFamilyIdx);
        queueFamilyIndices.push_back(presentQueueFamilyIdx);
    }
    swapchain = Swapchain(device.Get(), surface, swapchainFormat,
        surfaceCapabilities, queueFamilyIndices);

    // create depth texture
    depthTexture = new DepthTexture(device.Get(), 512, 512, primaryGPU.memoryProperties);

    // TODO: setup uniform buffer for MVP, time, audio data...?

    /*
     * Create VkRenderPass
     */
    const uint32_t attachmentDescCount = 2;
    VkAttachmentDescription attachmentDescs[attachmentDescCount];
    attachmentDescs[0] = VkUtil::AttachmentDescription();
    attachmentDescs[0].format = swapchainFormat;
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

    vkCreateRenderPass(device.Get(), &renderpassCI, NULL, &renderPass);

    // TODO: setup shader workflow.
    // in the mean time...
    shaderc::Compiler compiler;
    auto vertexShader = CompileShader(compiler, BasicVertexShader);
    auto fragmentShader = CompileShader(compiler, BasicFragmentShader);

    auto vsModuleCI = VkUtil::ShaderModuleCI();
    vsModuleCI.codeSize = vertexShader.size() * sizeof(unsigned int);
    vsModuleCI.pCode = vertexShader.data();
    result = vkCreateShaderModule(device.Get(), &vsModuleCI, NULL, &vertexShaderModule);
    if (result != VK_SUCCESS) throw std::exception("Unable to create vertex shader module.");
    auto vsPipelineShaderStageCI = VkUtil::PipelineShaderStageCI();
    vsPipelineShaderStageCI.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vsPipelineShaderStageCI.module = vertexShaderModule;
    vsPipelineShaderStageCI.pName = "main";
    vsPipelineShaderStageCI.pSpecializationInfo = NULL;

    auto fsModuleCI = VkUtil::ShaderModuleCI();
    fsModuleCI.codeSize = fragmentShader.size() * sizeof(unsigned int);
    fsModuleCI.pCode = fragmentShader.data();
    result = vkCreateShaderModule(device.Get(), &fsModuleCI, NULL, &fragmentShaderModule);
    if (result != VK_SUCCESS) throw std::exception("Unable to create fragment shader module.");
    auto fsPipelineShaderStageCI = VkUtil::PipelineShaderStageCI();
    fsPipelineShaderStageCI.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fsPipelineShaderStageCI.module = fragmentShaderModule;
    fsPipelineShaderStageCI.pName = "main";
    fsPipelineShaderStageCI.pSpecializationInfo = NULL;
    
    

    /*
     * Build command buffer *INCOMPLETE*
     */
    auto cmdBufferInheritanceInfo = VkUtil::CommandBufferInheritanceInfo();
    cmdBufferInheritanceInfo.renderPass = renderPass;
    //fillout
    auto cmdBufferBeginInfo = VkUtil::CommandBufferBeginInfo();
    cmdBufferBeginInfo.pInheritanceInfo = &cmdBufferInheritanceInfo;
    vkBeginCommandBuffer(cmdBuffer.Get(), &cmdBufferBeginInfo);
}


Manager::~Manager()
{
    vkDestroyShaderModule(device.Get(), fragmentShaderModule, NULL);
    vkDestroyShaderModule(device.Get(), vertexShaderModule, NULL);
    vkDestroyRenderPass(device.Get(), renderPass, NULL);

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
    for (auto it = program.cbegin(); it != nullptr && it != program.cend(); it++)         {
        data.push_back(*it);
    }

    return data;
}
