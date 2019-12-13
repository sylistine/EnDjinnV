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
    vkExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
    vkExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
    

    std::vector<const char*> vkLayers;
#ifdef _DEBUG
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
    VkDebugReportCallbackCreateInfoEXT debugReportCallbackCI;
    debugReportCallbackCI.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |// vk::DebugReportFlagBitsEXT::eError |
        VK_DEBUG_REPORT_ERROR_BIT_EXT | //vk::DebugReportFlagBitsEXT::eWarning |
        VK_DEBUG_REPORT_ERROR_BIT_EXT | //vk::DebugReportFlagBitsEXT::eDebug |
        VK_DEBUG_REPORT_ERROR_BIT_EXT | //vk::DebugReportFlagBitsEXT::eInformation |
        VK_DEBUG_REPORT_ERROR_BIT_EXT; //vk::DebugReportFlagBitsEXT::ePerformanceWarning;
    debugReportCallbackCI.pfnCallback = &MyDebugReportCallback;
    VkResult result = vkCreateDebugReportCallbackEXT((VkInstance)vkInstance, &debugReportCallbackCI, NULL, &callback);
    debugCallbackInited = result == VK_SUCCESS;
    //vk::Result result = vkInstance.createDebugReportCallbackEXT(
    //    &debugReportCallbackCI,
    //    NULL,
    //    &callback);
    // debugCallbackInited  = result == vk::Result::eSuccess;
    std::cout << "Callback created with result " << Djn::VkUtil::to_string(result) << std::endl;
#endif
}


Platform::~Platform()
{
#ifdef _DEBUG
    vkDestroyDebugReportCallbackEXT((VkInstance)vkInstance, callback, NULL);
#endif
    vkDestroySurfaceKHR(vkInstance, surface, NULL);
    vkDestroyInstance(vkInstance, NULL);
}
