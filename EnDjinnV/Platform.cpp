#include "EnDjinnV.h"

#include "Platform.h"

#include <vector>

#ifdef _WIN32
#include "WindowsPlatformUtil.h"
#endif

using namespace Djn;

Platform::Platform(const char* appName) {
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

  vk::InstanceCreateInfo vkInstanceCI;
  vkInstanceCI.pApplicationInfo = &vkAppInfo;
  vkInstanceCI.enabledExtensionCount = vkExtensions.size();
  vkInstanceCI.ppEnabledExtensionNames = vkExtensions.data();

  vk::Result vkResult = vk::createInstance(&vkInstanceCI, NULL, &vkInstance);
  if (vkResult != vk::Result::eSuccess)
    throw Exception("Unable to create vulkan instance.");

    // Create vulkan surface from platform window.
#ifdef _WIN32
  vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
  surfaceCreateInfo.hinstance = XPlat::Windows::GetModuleInstanceHandle();
  surfaceCreateInfo.hwnd = XPlat::Windows::GetWindowHandle();
  vkResult =
      vkInstance.createWin32SurfaceKHR(&surfaceCreateInfo, NULL, &surface);
  if (vkResult != vk::Result::eSuccess)
    throw Exception("Unable to create Win32 surface.");
#endif
}

Platform::~Platform() {
  vkDestroySurfaceKHR(vkInstance, surface, NULL);
  vkDestroyInstance(vkInstance, NULL);
}
