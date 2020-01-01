#include "EndjinnV.h"
#include "Platform.h"

using namespace Djn;
using namespace Djn::Platform;


LRESULT CALLBACK CWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return PlatformHandler::GetHandler()->wndProc(hWnd, uMsg, wParam, lParam);
}

/* Begin Static Members */
PlatformHandler* PlatformHandler::instance = nullptr;

PlatformHandler* PlatformHandler::GetHandler()
{
    if (!instance) throw Exception("Platform handler has not been initialized.");
    return instance;
}

void PlatformHandler::Initialize(const char* appName)
{
    if (instance) {
        throw Exception("Window is already initialized.");
    }

    instance = new PlatformHandler(appName);
}
/* End Static Members */

PlatformHandler::PlatformHandler(const char* appName) :
    windowClassName(appName),
    moduleHandle(GetModuleHandle(NULL)),
    windowHandle(0),
    isQuitting(0)
{
    surface.state = Surface::State::Windowed;
    surface.posX = 0;
    surface.posY = 0;
    surface.width = 1024;
    surface.height = 1024;

    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = CWndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = moduleHandle;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = windowClassName;
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    if (!RegisterClassEx(&wndClass)) {
        throw Exception("Unable to register window class.");
    }
}

void PlatformHandler::createSurface(VkInstance instance)
{
    vkInstance = instance;

    RECT windowRect = { surface.posX, surface.posY, surface.width, surface.height };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);
    windowHandle = CreateWindowEx(
        0,
        windowClassName,
        windowClassName,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        surface.posX, surface.posY,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        moduleHandle,
        NULL);
    if (!windowHandle) {
        throw Exception("Unable to create window.");
    }

    VkWin32SurfaceCreateInfoKHR surfaceCI;
    surfaceCI.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCI.flags = 0;
    surfaceCI.hinstance = moduleHandle;
    surfaceCI.hwnd = windowHandle;
    surfaceCI.pNext = 0;
    VkResult result = vkCreateWin32SurfaceKHR(vkInstance, &surfaceCI, NULL, &renderSurface);
    if (result != VK_SUCCESS) throw Exception("Unable to create Win32 surface.");
}

void PlatformHandler::destroySurface()
{
    vkDestroySurfaceKHR(vkInstance, renderSurface, NULL);
}


bool PlatformHandler::update()
{
    didResize = false;

    MSG systemMessage = {};
    if (PeekMessage(&systemMessage, 0, 0, 0, PM_REMOVE)) {
        if (systemMessage.message == WM_QUIT) {
            isQuitting = true;
        } else {
            TranslateMessage(&systemMessage);
            DispatchMessage(&systemMessage);
        }
        return true;
    }
    return false;
}

LRESULT PlatformHandler::wndProc(HWND hWnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    /* Handling the following messages:
     * ACTIVATE
     * SIZE
     * ENTERSIZEMOVE
     * EXITSIZEMOVE
     * DESTROY
     * MENUCHAR
     * GETMINMAXINFO
     * L- M- RBUTTONDOWN
     * L- M- RBUTTONUP
     * MOUSEMOVE
     * KEYUP
     */
    switch (msg) {
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) {
            // pause
            // stop timer
        } else {
            // unpause
            // start timer
        }
        return 0;
    case WM_SIZE:
        surface.width = LOWORD(lParam);
        surface.height = HIWORD(lParam);
        if (wParam == SIZE_MINIMIZED) {
            surface.state = Surface::State::Minimized;
            // app paused = true
        } else if (wParam == SIZE_MAXIMIZED) {
            surface.state = Surface::State::Maximized;
            // app paused = false
            // Trigger buffer resizes.
        } else if (wParam == SIZE_RESTORED) {
            switch (surface.state) {
            case Surface::State::Minimized:
            case Surface::State::Maximized:
                surface.state = Surface::State::Windowed;
                didResize = true;
                // app paused = false
                break;
            case Surface::State::Resizing:
                break;
            default:
                didResize = true;
                break;
            }
        }
        return 0;
    case WM_ENTERSIZEMOVE:
        std::cout << "Enter resizing" << std::endl;
        surface.state = Surface::State::Resizing;
        // app paused = true
        // stop timer.
        return 0;
    case WM_EXITSIZEMOVE:
        std::cout << "Exit resizing" << std::endl;
        surface.state = Surface::State::Windowed;
        didResize = true;
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_QUIT:
        isQuitting = true;
        return 0;
    case WM_MENUCHAR:
        return MAKELRESULT(0, MNC_CLOSE);
    case WM_GETMINMAXINFO:
        reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 200;
        reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 200;
        return 0;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
        //OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        //OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_MOUSEMOVE:
        //OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;
    case WM_KEYUP:
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);
        }
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}