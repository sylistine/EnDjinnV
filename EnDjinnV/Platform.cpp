#include "EnDjinnV.h"

#include "Platform.h"

#include <vector>

#ifdef _WIN32
#include "WindowsPlatformUtil.h"
#endif

#include "VulkanUtil.h"


using namespace Djn;


Platform::Platform(const char* appName)
{
    XPlat::Initialize(appName);

    /*
     * Create vulkan instance and surface for the platform.
     */
     // Create Vulkan Instance.
    vk::ApplicationInfo vkAppInfo;
    vkAppInfo.pApplicationName = appName;
    vkAppInfo.pEngineName = appName;
    vkAppInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> enabledExtensions;
    std::vector<const char*> enabledLayers;

    enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
    enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

#ifdef _DEBUG
    //std::cout << "Available extensions: " << std::endl;
    //VkUtil::PrintInstanceExtensions();
    //std::cout << "Available layers: " << std::endl;
    //VkUtil::PrintInstanceLayers();

    enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    enabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    enabledLayers.push_back(VkUtil::VK_LAYER_FULL_VALIDATION);
    //enabledLayers.push_back(VkUtil::VK_LAYER_RENDERDOC_CAPTURE);
#endif

    // Validate requested layers.
    std::vector<vk::LayerProperties> instLayers = vk::enumerateInstanceLayerProperties();
    for (auto& enabledLayer : enabledLayers) {
        bool foundLayer = false;
        for (auto& instLayer : instLayers) {
            if (strcmp(instLayer.layerName, enabledLayer) == 0) {
                foundLayer = true;
                break;
            }
        }
        if (!foundLayer) {
            std::cout << "Unable to find required layer " << enabledLayer << std::endl;
            throw Exception("Unable to find required vulkan layer.");
        }
    }

    // Create VK Instance.
    vk::InstanceCreateInfo vkInstanceCI;
    vkInstanceCI.pApplicationInfo = &vkAppInfo;
    vkInstanceCI.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    vkInstanceCI.ppEnabledExtensionNames = enabledExtensions.data();
    vkInstanceCI.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
    vkInstanceCI.ppEnabledLayerNames = enabledLayers.data();

    vk::Result vkResult = vk::createInstance(&vkInstanceCI, NULL, &vkInstance);
    if (vkResult != vk::Result::eSuccess) throw Exception("Unable to create vulkan instance.");

    // Create vulkan surface from platform window.
#ifdef _WIN32
    vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.hinstance = XPlat::Windows::GetModuleInstanceHandle();
    surfaceCreateInfo.hwnd = XPlat::Windows::GetWindowHandle();
    vkResult = vkInstance.createWin32SurfaceKHR(&surfaceCreateInfo, NULL, &surface);
    if (vkResult != vk::Result::eSuccess) throw Exception("Unable to create Win32 surface.");
#endif

#ifdef _DEBUG
    // C-Style
    VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCI = {};
    debugUtilsMessengerCI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugUtilsMessengerCI.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugUtilsMessengerCI.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugUtilsMessengerCI.pfnUserCallback = VkUtil::DebugUtilsCallback;
    auto cduc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
    if (cduc != nullptr) {
        VkResult result = cduc((VkInstance)vkInstance, &debugUtilsMessengerCI, NULL, &messenger);
        debugUtilsMessengerInited = result == VK_SUCCESS;
        std::cout << "Messenger created with result " << VkUtil::to_string(result) << std::endl;
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
    debugReportCallbackCI.pfnCallback = &VkUtil::DebugReportCallback;
    auto cdrc = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugReportCallbackEXT");
    if (cdrc != nullptr) {
        VkResult result = cdrc((VkInstance)vkInstance, &debugReportCallbackCI, NULL, &callback);
        debugReportCallbackInited = result == VK_SUCCESS;
        std::cout << "Callback created with result " << VkUtil::to_string(result) << std::endl;
    } else {
        std::cout << "Unable to locate vkCreateDebugReportCallbackEXT function." << std::endl;
    }

    // CPP-Style
    //vk::DebugReportCallbackCreateInfoEXT debugReportCallbackCI;
    //debugReportCallbackCI.flags = vk::DebugReportFlagBitsEXT::eError |
    //    vk::DebugReportFlagBitsEXT::eWarning |
    //    vk::DebugReportFlagBitsEXT::eDebug |
    //    vk::DebugReportFlagBitsEXT::eInformation |
    //    vk::DebugReportFlagBitsEXT::ePerformanceWarning;
    //debugReportCallbackCI.pfnCallback = &MyDebugReportCallback;
    //vk::Result result = vkInstance.createDebugReportCallbackEXT(
    //    &debugReportCallbackCI,
    //    NULL,
    //    &callback);
    //debugCallbackInited = result == vk::Result::eSuccess;
    //std::cout << "Callback created with result " << vk::to_string(result) << std::endl;
#endif
}


Platform::~Platform()
{
#ifdef _DEBUG
    if (debugReportCallbackInited) {
        // C-Style
        auto ddrc = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugReportCallbackEXT");
        if (ddrc != nullptr) {
            ddrc((VkInstance)vkInstance, callback, NULL);
        } else {
            std::cout << "Debug callback was initted but destroy function could not be located." << std::endl;
        }

        // CPP-Style
        //vkInstance.destroyDebugReportCallbackEXT(callback);
    }
#endif
    vkDestroySurfaceKHR(vkInstance, surface, NULL);
    vkDestroyInstance(vkInstance, NULL);
}
