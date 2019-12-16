#include "EnDjinnV.h"

#include "Platform.h"

#include <vector>

#ifdef _WIN32
#include "WindowsPlatformUtil.h"
#endif

#include "VulkanUtil.h"


using namespace Djn;


VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(
    VkDebugReportFlagsEXT       flags,
    VkDebugReportObjectTypeEXT  objectType,
    uint64_t                    object,
    size_t                      location,
    int32_t                     messageCode,
    const char* pLayerPrefix,
    const char* pMessage,
    void* pUserData)
{
    std::cout << pMessage << std::endl;
    return VK_FALSE;
}


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

    std::vector<const char*> vkExtensions;
    std::vector<const char*> vkLayers;

    vkExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
    vkExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
    

#ifdef _DEBUG
    vkExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    auto allLayers = "VK_LAYER_LUNARG_standard_validation";
    std::vector<vk::LayerProperties> layerProps = vk::enumerateInstanceLayerProperties();
    for (auto& prop : layerProps) {
        if (strcmp(prop.layerName, allLayers) == 0) {
            vkLayers.push_back(allLayers);
            std::cout << "Enabling instance layer " << allLayers << std::endl;
        }
    }
#endif
    
    vk::InstanceCreateInfo vkInstanceCI;
    vkInstanceCI.pApplicationInfo = &vkAppInfo;
    vkInstanceCI.enabledExtensionCount = (uint32_t)vkExtensions.size();
    vkInstanceCI.ppEnabledExtensionNames = vkExtensions.data();
    vkInstanceCI.enabledLayerCount = (uint32_t)vkLayers.size();
    vkInstanceCI.ppEnabledLayerNames = vkLayers.data();

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
    VkDebugReportCallbackCreateInfoEXT debugReportCallbackCI;
    debugReportCallbackCI.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_ERROR_BIT_EXT;
    debugReportCallbackCI.pfnCallback = &MyDebugReportCallback;
    auto cdrc = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugReportCallbackEXT");
    if (cdrc != nullptr) {
        VkResult result = cdrc((VkInstance)vkInstance, &debugReportCallbackCI, NULL, &callback);
        debugCallbackInited = result == VK_SUCCESS;
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
    // debugCallbackInited  = result == vk::Result::eSuccess;
    //std::cout << "Callback created with result " << vk::to_string(result) << std::endl;
#endif
}


Platform::~Platform()
{
#ifdef _DEBUG
    if (debugCallbackInited) {
        auto ddrc = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugReportCallbackEXT");
        if (ddrc != nullptr) {
            ddrc((VkInstance)vkInstance, callback, NULL);
        } else {
            std::cout << "Debug callback was initted but destroy function could not be located." << std::endl;
        }
    }
#endif
    vkDestroySurfaceKHR(vkInstance, surface, NULL);
    vkDestroyInstance(vkInstance, NULL);
}
