#pragma once

#include <vulkan/vulkan.hpp>

#include <vector>

#include <boost/stacktrace.hpp>


namespace Djn::VkUtil
{
    static const char* VK_LAYER_FULL_VALIDATION = "VK_LAYER_KHRONOS_validation";
    static const char* VK_LAYER_RENDERDOC_CAPTURE = "VK_LAYER_RENDERDOC_Capture";


    static std::string to_string(VkDebugUtilsMessageSeverityFlagBitsEXT bits)
    {
        switch (bits) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: return "V";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: return "I";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return "W";
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: return "E";
        default: return "?";
        }
    }


    static std::string to_string(VkResult result)
    {
        switch (result) {
        case VK_SUCCESS: return "Success";
        case VK_NOT_READY: return "NotReady";
        case VK_TIMEOUT: return "Timeout";
        case VK_EVENT_SET: return "EventSet";
        case VK_EVENT_RESET: return "EventReset";
        case VK_INCOMPLETE: return "Incomplete";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "ErrorOutOfHostMemory";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "ErrorOutOfDeviceMemory";
        case VK_ERROR_INITIALIZATION_FAILED: return "ErrorInitializationFailed";
        case VK_ERROR_DEVICE_LOST: return "ErrorDeviceLost";
        case VK_ERROR_MEMORY_MAP_FAILED: return "ErrorMemoryMapFailed";
        case VK_ERROR_LAYER_NOT_PRESENT: return "ErrorLayerNotPresent";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "ErrorExtensionNotPresent";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "ErrorFeatureNotPresent";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "ErrorIncompatibleDriver";
        case VK_ERROR_TOO_MANY_OBJECTS: return "ErrorTooManyObjects";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "ErrorFormatNotSupported";
        case VK_ERROR_FRAGMENTED_POOL: return "ErrorFragmentedPool";
        case VK_ERROR_OUT_OF_POOL_MEMORY: return "ErrorOutOfPoolMemory";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "ErrorInvalidExternalHandle";
        case VK_ERROR_SURFACE_LOST_KHR: return "ErrorSurfaceLostKHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "ErrorNativeWindowInUseKHR";
        case VK_SUBOPTIMAL_KHR: return "SuboptimalKHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "ErrorOutOfDateKHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "ErrorIncompatibleDisplayKHR";
        case VK_ERROR_VALIDATION_FAILED_EXT: return "ErrorValidationFailedEXT";
        case VK_ERROR_INVALID_SHADER_NV: return "ErrorInvalidShaderNV";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "ErrorInvalidDrmFormatModifierPlaneLayoutEXT";
        case VK_ERROR_FRAGMENTATION_EXT: return "ErrorFragmentationEXT";
        case VK_ERROR_NOT_PERMITTED_EXT: return "ErrorNotPermittedEXT";
        case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT: return "ErrorInvalidDeviceAddressEXT";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "ErrorFullScreenExclusiveModeLostEXT";
        default: return "Invalid";
        }
    }


    static void PrintInstanceExtensions()
    {
        auto exts = vk::enumerateInstanceExtensionProperties();
        for (auto ext : exts) {
            std::cout << ext.extensionName << std::endl;
        }
    }


    static void PrintInstanceLayers()
    {
        auto layers = vk::enumerateInstanceLayerProperties();
        for (auto layer : layers) {
            std::cout << layer.layerName << std::endl;
        }
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
        std::cout << "VkDbg [" << pLayerPrefix << "]\t" << pMessage << std::endl;
        return VK_FALSE;
    }


    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        std::cout << "VkDbg " << to_string(messageSeverity) << " " << pCallbackData->pMessage << std::endl;
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            std::cout << boost::stacktrace::to_string(boost::stacktrace::stacktrace()) << std::endl;
        }
        return VK_FALSE;
    }
}
