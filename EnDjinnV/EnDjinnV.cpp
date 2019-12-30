#include "EnDjinnV.h"

#include <iostream>

#include "Platform.h"
#include "Gfx.h"

// Temp. These should be moved out later.
#include "Camera.h"
#include "Vertex.h"
#include "VulkanUtil.h"
#include "MathUtil.h"

std::vector<Djn::Gfx::Vertex> GetDefaultVertices();
std::vector<Djn::Gfx::Vertex> CubeMesh();

void CreateDebugCallbacks(VkInstance instance);

int main()
{
#if defined(_WIN32) && defined(NDEBUG)
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    std::cout << "Starting EnDjinn." << std::endl;
    using namespace Djn;
    auto appName = "Vulkan Exploration";
    Platform::PlatformHandler* platformHandler = NULL;
    VkInstance vkInstance;

    try {
        std::cout << "Constructing platform." << std::endl;
        Platform::PlatformHandler::Initialize(appName);
        platformHandler = Platform::PlatformHandler::GetHandler();
    } catch (std::exception & e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    try {
        std::cout << "Initializing graphics." << std::endl;
        vkInstance = Gfx::CreateVulkanInstance(appName);
        CreateDebugCallbacks(vkInstance);
        platformHandler->createSurface(vkInstance); 
    } catch (std::exception & e) {
        std::cout << e.what() << std::endl;
        if (platformHandler) {
            delete platformHandler;
        }
        return -1;
    }

    try {
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
            Gfx::Manager::Draw();
        }
    } catch (std::exception & e) {
        std::cout << e.what() << std::endl;
    }

    std::cout << "Terminating EnDjinn" << std::endl;
    vkDestroyInstance(vkInstance, NULL);
    if (platformHandler) {
        delete platformHandler;
    }
    return 0;
}


bool debugUtilsMessengerInited = false;
VkDebugUtilsMessengerEXT messenger;
bool debugReportCallbackInited = false;
VkDebugReportCallbackEXT callback;


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
    std::cout << "VkDbg [" << pLayerPrefix << "]\t" << pMessage << std::endl;
    return VK_FALSE;
}


static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cout << "VkDbg " << Djn::VkUtil::to_string(messageSeverity) << " " << pCallbackData->pMessage << std::endl;
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::cout << boost::stacktrace::to_string(boost::stacktrace::stacktrace()) << std::endl;
    }
    return VK_FALSE;
}

void CreateDebugCallbacks(VkInstance instance)
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
    if (cduc != nullptr) {
        VkResult result = cduc((VkInstance)instance, &debugUtilsMessengerCI, NULL, &messenger);
        debugUtilsMessengerInited = result == VK_SUCCESS;
        std::cout << "Messenger created with result " << Djn::VkUtil::to_string(result) << std::endl;
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
    if (cdrc != nullptr) {
        VkResult result = cdrc((VkInstance)instance, &debugReportCallbackCI, NULL, &callback);
        debugReportCallbackInited = result == VK_SUCCESS;
        std::cout << "Callback created with result " << Djn::VkUtil::to_string(result) << std::endl;
    } else {
        std::cout << "Unable to locate vkCreateDebugReportCallbackEXT function." << std::endl;
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
