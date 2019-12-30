#include "EnDjinnV.h"

#include <iostream>

#include "Platform.h"
#include "Gfx.h"

// Temp. These should be moved out later.
#include "VulkanUtil.h"
#include "MathUtil.h"

#include "Camera.h"
#include "Vertex.h"

std::vector<Djn::Gfx::Vertex> GetDefaultVertices();
std::vector<Djn::Gfx::Vertex> CubeMesh();

void CreateDebugCallbacks(VkInstance instance, VkDebugUtilsMessengerEXT& messenger, VkDebugReportCallbackEXT& callback);
void DestroyDebugCallbacks(VkInstance instance, VkDebugUtilsMessengerEXT& messenger, VkDebugReportCallbackEXT& callback);

auto appName = "Vulkan Exploration";


int main()
{
#if defined(_WIN32) && defined(NDEBUG)
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    std::cout << "Starting EnDjinn." << std::endl;
    using namespace Djn;

    // Platform initialization.
    Djn::Platform::PlatformHandler* platformHandler = NULL;
    try {
        std::cout << "Constructing platform." << std::endl;
        Platform::PlatformHandler::Initialize(appName);
        platformHandler = Platform::PlatformHandler::GetHandler();
    } catch (std::exception & e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    // Graphics initialization.
    VkInstance vkInstance;
    try {
        std::cout << "Initializing graphics." << std::endl;
        vkInstance = Gfx::CreateVulkanInstance(appName);
    } catch (std::exception & e) {
        std::cout << e.what() << std::endl;
        if (platformHandler) {
            platformHandler->destroySurface();
            delete platformHandler;
        }
        return -1;
    }

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT messenger;
    VkDebugReportCallbackEXT callback;
    CreateDebugCallbacks(vkInstance, messenger, callback);
#endif
    try {
        platformHandler->createSurface(vkInstance);
        Gfx::Manager::Initialize(vkInstance, platformHandler->getRenderSurface());
        Djn::Camera mainCamera(vec3(0.f, 0.f, -5.f));
        auto mvp = mainCamera.ClipMatrix() * mainCamera.ViewMatrix() * mainCamera.ProjectionMatrix();
        Gfx::Manager::SetViewProjectionMatrices(mvp);
        Gfx::Manager::SetVertices(CubeMesh());
        Gfx::Manager::SetupPipeline();

        while (!platformHandler->getIsQuitting()) {
            if (platformHandler->update()) {
                if (platformHandler->getDidResize()) {
                    // Trigger resize of swapchain and frame buffers.
                    // VkSurfaceKHR updates window surface size automatically,
                    // so we do not need to pass them from the platform.
                }
                continue;
            }
            std::cout << "Draw start" << std::endl;
            Gfx::Manager::Draw();
            std::cout << "Draw end" << std::endl;
        }
    } catch (std::exception & e) {
        std::cout << e.what() << std::endl;
    }

    std::cout << "Terminating EnDjinn" << std::endl;
    Gfx::Manager::Terminate();
    if (platformHandler) {
        platformHandler->destroySurface();
        delete platformHandler;
    }
#ifdef _DEBUG
    DestroyDebugCallbacks(vkInstance, messenger, callback);
#endif
    vkDestroyInstance(vkInstance, NULL);
    return 0;
}


static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
    VkDebugReportFlagsEXT       flags,
    VkDebugReportObjectTypeEXT  objectType,
    uint64_t                    object,
    size_t                      location,
    int32_t                     messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData)
{
    std::cout << "Vulkan [" << pLayerPrefix << "]\t" << pMessage << std::endl << std::endl;
    return VK_FALSE;
}


static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cout << "Vulkan " << Djn::VkUtil::to_string(messageSeverity) << " " << pCallbackData->pMessage << std::endl;
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::cout << boost::stacktrace::to_string(boost::stacktrace::stacktrace(8, 20)) << std::endl;
    }
    std::cout << std::endl;
    return VK_FALSE;
}


void CreateDebugCallbacks(VkInstance instance, VkDebugUtilsMessengerEXT& messenger, VkDebugReportCallbackEXT& callback)
{
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI = {};
    debugUtilsMessengerCI.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCI.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCI.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCI.pfnUserCallback = DebugUtilsCallback;
    auto cduc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (cduc != NULL) {
        VkResult result = cduc(instance, &debugUtilsMessengerCI, NULL, &messenger);
        if (result != VK_SUCCESS) throw std::exception("Unable to create debug utils messenger.");
    } else {
        std::cout << "Unable to locate vkCreateDebugUtilsCallbackEXT function." << std::endl;
    }

    VkDebugReportCallbackCreateInfoEXT debugReportCallbackCI = {};
    debugReportCallbackCI.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debugReportCallbackCI.flags =
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_DEBUG_BIT_EXT;
    debugReportCallbackCI.pfnCallback = &DebugReportCallback;
    auto cdrc = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (cdrc != NULL) {
        VkResult result = cdrc(instance, &debugReportCallbackCI, NULL, &callback);
        if (result != VK_SUCCESS) throw std::exception("Unable to create debug report callback.");
    } else {
        std::cout << "Unable to locate vkCreateDebugReportCallbackEXT function." << std::endl;
    }
}

void DestroyDebugCallbacks(VkInstance instance, VkDebugUtilsMessengerEXT& messenger, VkDebugReportCallbackEXT& callback)
{
    auto ddum = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (ddum != NULL) {
        ddum(instance, messenger, NULL);
    }
    auto ddrc = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (ddrc != NULL) {
        ddrc(instance, callback, NULL);
    }
}

std::vector<Djn::Gfx::Vertex> GetDefaultVertices()
{
    std::vector<Djn::Gfx::Vertex> vertexList;
    vertexList.push_back(Djn::Gfx::Vertex(vec4(-1.f, 0.f, 0.f, 1.f), vec4(1.f, 0.f, 0.f, 1.f)));
    vertexList.push_back(Djn::Gfx::Vertex(vec4(0.0f, 1.f, 0.f, 1.f), vec4(0.f, 1.f, 0.f, 1.f)));
    vertexList.push_back(Djn::Gfx::Vertex(vec4(1.f, 0.f, 0.f, 1.f), vec4(0.f, 0.f, 1.f, 1.f)));
    vertexList.push_back(Djn::Gfx::Vertex(vec4(1.f, 0.f, 0.f, 1.f), vec4(0.f, 0.f, 1.f, 1.f)));
    vertexList.push_back(Djn::Gfx::Vertex(vec4(0.0f, 1.f, 0.f, 1.f), vec4(0.f, 1.f, 0.f, 1.f)));
    vertexList.push_back(Djn::Gfx::Vertex(vec4(-1.f, 0.f, 0.f, 1.f), vec4(1.f, 0.f, 0.f, 1.f)));
    return vertexList;
}


static std::vector<Djn::Gfx::Vertex> CubeMesh()
{
    std::vector<Djn::Gfx::Vertex> verts{
        // red face
        {vec4(-1, -1, 1, 1), vec4(1.f, 0.f, 0.f, 1)},
        {vec4(-1, 1, 1, 1), vec4(1.f, 0.f, 0.f, 1.f)},
        {vec4(1, -1, 1, 1), vec4(1.f, 0.f, 0.f, 1.f)},
        {vec4(1, -1, 1, 1), vec4(1.f, 0.f, 0.f, 1.f)},
        {vec4(-1, 1, 1, 1), vec4(1.f, 0.f, 0.f, 1.f)},
        {vec4(1, 1, 1, 1), vec4(1.f, 0.f, 0.f, 1.f)},
        // green face
        {vec4(-1, -1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        {vec4(1, -1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        {vec4(-1, 1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        {vec4(-1, 1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        {vec4(1, -1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        {vec4(1, 1, -1, 1), vec4(0.f, 1.f, 0.f, 1.f)},
        // blue face
        {vec4(-1, 1, 1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        {vec4(-1, -1, 1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        {vec4(-1, 1, -1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        {vec4(-1, 1, -1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        {vec4(-1, -1, 1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        {vec4(-1, -1, -1, 1), vec4(0.f, 0.f, 1.f, 1.f)},
        // yellow face
        {vec4(1, 1, 1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        {vec4(1, 1, -1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        {vec4(1, -1, 1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        {vec4(1, -1, 1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        {vec4(1, 1, -1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        {vec4(1, -1, -1, 1), vec4(1.f, 1.f, 0.f, 1.f)},
        // magenta face
        {vec4(1, 1, 1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        {vec4(-1, 1, 1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        {vec4(1, 1, -1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        {vec4(1, 1, -1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        {vec4(-1, 1, 1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        {vec4(-1, 1, -1, 1), vec4(1.f, 0.f, 1.f, 1.f)},
        // cyan face
        {vec4(1, -1, 1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
        {vec4(1, -1, -1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
        {vec4(-1, -1, 1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
        {vec4(-1, -1, 1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
        {vec4(1, -1, -1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
        {vec4(-1, -1, -1, 1), vec4(0.f, 1.f, 1.f, 1.f)},
    };
    return verts;
}
