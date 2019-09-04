#include "Platform.h"

#include <exception>

#ifdef _WIN32
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_DESTROY:
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        // run(info);
        return 0;
    default:
        break;
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}
#endif

Platform::Platform(const char* appName, VkInstance instance)
{
    windowRect.x = 0;
    windowRect.y = 0;
    windowRect.w = 256;
    windowRect.h = 256;

#ifdef _WIN32
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = appName;
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    if (!RegisterClassEx(&wndClass)) {
        throw std::exception("Unable to register window class.");
    }

    HWND hWnd = CreateWindowEx(
        0,
        appName,
        appName,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        windowRect.x, windowRect.y,
        windowRect.w, windowRect.h,
        NULL,
        NULL,
        hInstance,
        NULL);
    if (!hWnd) {
        throw std::exception("Unable to create window.");
    }
#endif

    VkResult result;

#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = NULL;
    surfaceCreateInfo.hinstance = hInstance;
    surfaceCreateInfo.hwnd = hWnd;
    result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
#endif

    if (result != VK_SUCCESS) throw std::exception("Unable to create surface.");
}

Platform::~Platform() {}
