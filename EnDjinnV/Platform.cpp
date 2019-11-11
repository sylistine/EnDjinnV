#include "EnDjinnV.h"

#include "Platform.h"

#include <vector>

#include "VulkanUtil.h"

#ifdef _WIN32
#include "WindowsPlatformUtil.h"
#endif


using namespace Djn;


Platform::Platform(const char* appName)
{
    XPlat::Initialize(appName);

    /*
     * Create vulkan instance and surface for the platform.
     */
     // Create Vulkan Instance.
    VkApplicationInfo vkAppInfo = {};
    vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vkAppInfo.pApplicationName = appName;
    vkAppInfo.pEngineName = appName;
    vkAppInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> vkExtensions;
    vkExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
    vkExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    auto vkInstanceCI = VkUtil::InstanceCI();
    vkInstanceCI.pApplicationInfo = &vkAppInfo;
    vkInstanceCI.enabledExtensionCount = vkExtensions.size();
    vkInstanceCI.ppEnabledExtensionNames = vkExtensions.data();

    VkResult vkResult = vkCreateInstance(&vkInstanceCI, NULL, &vkInstance);
    if (vkResult != VK_SUCCESS) throw std::exception("Unable to create vulkan instance.");

    // Create vulkan surface from platform window.
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.hinstance = XPlat::Windows::GetModuleInstanceHandle();
    surfaceCreateInfo.hwnd = XPlat::Windows::GetWindowHandle();
    vkResult = vkCreateWin32SurfaceKHR(vkInstance, &surfaceCreateInfo, NULL, &surface);
    if (vkResult != VK_SUCCESS) throw std::exception("Unable to create Win32 surface.");
#endif
}


Platform::~Platform()
{
    vkDestroySurfaceKHR(vkInstance, surface, NULL);
    vkDestroyInstance(vkInstance, NULL);
}
