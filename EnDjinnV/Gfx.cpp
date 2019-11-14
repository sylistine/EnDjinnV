#include "EnDjinnV.h"

#include "Gfx.h"

#include <vector>

using namespace Djn::Gfx;


Device::Device(uint32_t gfxQueueFamilyIdx, VkPhysicalDevice physicalDevice)
{
    // create logical device
    float queuePriorities[1] = { 0.0 };

    auto deviceQueueCI = VkUtil::DeviceQueueCI();
    deviceQueueCI.queueFamilyIndex = gfxQueueFamilyIdx;
    deviceQueueCI.queueCount = 1;
    deviceQueueCI.pQueuePriorities = queuePriorities;

    std::vector<const char*> deviceExtensions;
    deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    auto deviceCI = VkUtil::DeviceCI();
    deviceCI.queueCreateInfoCount = 1;
    deviceCI.pQueueCreateInfos = &deviceQueueCI;
    deviceCI.enabledLayerCount = 0;
    deviceCI.ppEnabledLayerNames = NULL;
    deviceCI.enabledExtensionCount = deviceExtensions.size();
    deviceCI.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCI.pEnabledFeatures = NULL;
    VkResult result = vkCreateDevice(physicalDevice, &deviceCI, NULL, &vkDevice);
    if (result != VK_SUCCESS) throw std::exception("Unable to create logical device.");

    inited = true;
}


Device::~Device()
{
    if (!inited) return;
    
    vkDestroyDevice(vkDevice, NULL);
}


CommandPool::CommandPool(VkDevice device, uint32_t gfxQueueFamilyIdx) : vkDevice(device)
{
    auto commandPoolCI = VkUtil::CommandPoolCI();
    commandPoolCI.queueFamilyIndex = gfxQueueFamilyIdx;
    VkResult result = vkCreateCommandPool(vkDevice, &commandPoolCI, NULL, &vkCommandPool);
    if (result != VK_SUCCESS) throw std::exception("Unable to create command pool.");

    inited = true;
}


CommandPool::~CommandPool()
{
    if (!inited) return;
    
    vkDestroyCommandPool(vkDevice, vkCommandPool, NULL);
}


CommandBuffer::CommandBuffer(VkDevice device, VkCommandPool cmdPool) :
    vkDevice(device), vkCmdPool(cmdPool)
{
    count = 1;
    auto commandBufferAllocInfo = VkUtil::CommandBufferAllocInfo();
    commandBufferAllocInfo.commandPool = vkCmdPool;
    commandBufferAllocInfo.commandBufferCount = count;
    VkResult result = vkAllocateCommandBuffers(vkDevice, &commandBufferAllocInfo, &vkCommandBuffer);
    if (result != VK_SUCCESS) throw std::exception("Unable to allocate command buffer.");

    inited = true;
}


CommandBuffer::~CommandBuffer()
{
    if (!inited) return;

    VkCommandBuffer cmdBufferList[] = { vkCommandBuffer };
    vkFreeCommandBuffers(vkDevice, vkCmdPool, count, cmdBufferList);
}


Swapchain::Swapchain(VkDevice device, VkSurfaceKHR surface, VkFormat format,
    VkSurfaceCapabilitiesKHR surfaceCapabilities,
    std::vector<uint32_t> queueFamilyIndices) :
    vkDevice(device)
{
    // Do swapchain setup.

    VkExtent2D swapchainExtent;
    // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapchainExtent.width = 512;
        swapchainExtent.height = 512;
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

    VkSurfaceTransformFlagBitsKHR surfaceTransformFlagBits;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        surfaceTransformFlagBits = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        surfaceTransformFlagBits = surfaceCapabilities.currentTransform;
    }

    VkCompositeAlphaFlagBitsKHR compositeAlpha;
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (uint32_t i = 0; i < sizeof(compositeAlphaFlags) / sizeof(compositeAlphaFlags[0]); i++) {
        if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
            compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }

    // create swapchain
    auto swapchainCI = VkUtil::SwapChainCI();
    swapchainCI.surface = surface;
    swapchainCI.minImageCount = surfaceCapabilities.minImageCount;
    swapchainCI.imageFormat = format;
    swapchainCI.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchainCI.imageExtent = swapchainExtent;
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCI.preTransform = surfaceTransformFlagBits;
    swapchainCI.compositeAlpha = compositeAlpha;
    swapchainCI.presentMode = VK_PRESENT_MODE_FIFO_KHR; // Default, always supported.
    swapchainCI.clipped = true;
    swapchainCI.oldSwapchain = VK_NULL_HANDLE;

    if (queueFamilyIndices.size() > 0) {
        swapchainCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCI.queueFamilyIndexCount = queueFamilyIndices.size();
        swapchainCI.pQueueFamilyIndices = queueFamilyIndices.data();
    } else {
        swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCI.queueFamilyIndexCount = 0;
        swapchainCI.pQueueFamilyIndices = NULL;
    }

    VkResult result = vkCreateSwapchainKHR(vkDevice, &swapchainCI, NULL, &vkSwapchain);
    if (result != VK_SUCCESS) throw std::exception("Unable to create swapchain");

    swapchainImages = VkUtil::GetSwapchainImages(vkDevice, vkSwapchain);
    swapchainImageViews.resize(swapchainImages.size());
    for (auto i = 0u; i < swapchainImages.size(); i++) {
        auto swapchainImageViewCI = VkUtil::ImageViewCI();
        swapchainImageViewCI.image = swapchainImages[i];
        swapchainImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
        swapchainImageViewCI.format = format;
        swapchainImageViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
        swapchainImageViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
        swapchainImageViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
        swapchainImageViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
        swapchainImageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        swapchainImageViewCI.subresourceRange.baseMipLevel = 0;
        swapchainImageViewCI.subresourceRange.levelCount = 1;
        swapchainImageViewCI.subresourceRange.baseArrayLayer = 0;
        swapchainImageViewCI.subresourceRange.layerCount = 1;
        result = vkCreateImageView(vkDevice, &swapchainImageViewCI, NULL, &swapchainImageViews[i]);
        if (result != VK_SUCCESS) {
            vkDestroySwapchainKHR(vkDevice, vkSwapchain, NULL);
            throw std::exception("Unable to create views to swapchain images.");
        }
    }

    inited = true;
}


Swapchain::~Swapchain()
{
    if (!inited) return;

    for (auto i = 0u; i < swapchainImages.size(); i++) {
        vkDestroyImageView(vkDevice, swapchainImageViews[i], NULL);
    }
    vkDestroySwapchainKHR(vkDevice, vkSwapchain, NULL);
}


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
