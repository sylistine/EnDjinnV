#include "EnDjinnV.h"
#include "WindowsPlatformUtil.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_KEYUP:
        switch (wParam) {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        break;
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


HINSTANCE moduleHandle = NULL;
HWND hWnd = NULL;


void Djn::XPlat::Initialize(const char* appName)
{
    moduleHandle = GetModuleHandle(NULL);

    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = moduleHandle;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = appName;
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    if (!RegisterClassEx(&wndClass)) {
        throw Exception("Unable to register window class.");
    }

    hWnd = CreateWindowEx(
        0,
        appName,
        appName,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        6, 6,
        512, 512,
        NULL,
        NULL,
        moduleHandle,
        NULL);
    if (!hWnd) {
        throw Exception("Unable to create window.");
    }
}


bool Djn::XPlat::Update()
{
    return false;
}


HINSTANCE Djn::XPlat::Windows::GetModuleInstanceHandle()
{
    return moduleHandle;
}

HWND Djn::XPlat::Windows::GetWindowHandle()
{
    return hWnd;
}
