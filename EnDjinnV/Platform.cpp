#include "EnDjinnV.h"

#include "Platform.h"

#include <vector>

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
    vkAppInfo.apiVersion = VK_API_VERSION_1_1;

    std::vector<const char*> vkExtensions;
    vkExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
    vkExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    VkInstanceCreateInfo vkInstanceCreateInfo = {};
    vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vkInstanceCreateInfo.pApplicationInfo = &vkAppInfo;
    vkInstanceCreateInfo.enabledExtensionCount = vkExtensions.size();
    vkInstanceCreateInfo.ppEnabledExtensionNames = vkExtensions.data();

    VkResult vkResult = vkCreateInstance(&vkInstanceCreateInfo, NULL, &vkInstance);

    // Create vulkan surface from platform window.
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.hinstance = XPlat::Windows::GetModuleInstanceHandle();
    surfaceCreateInfo.hwnd = XPlat::Windows::GetWindowHandle();
    vkResult = vkCreateWin32SurfaceKHR(vkInstance, &surfaceCreateInfo, NULL, &surface);
#endif
}


Platform::~Platform()
{

}
