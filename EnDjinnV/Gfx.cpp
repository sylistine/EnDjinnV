#include "Gfx.h"

#ifdef _WIN32
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        // run(info);
        return 0;
    default:
        break;
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}
#endif

Gfx::Gfx(const char* appName) :
    appName(appName),
    instance(appName),
    device(instance.GetPreferredGpu(), instance.GetPreferredGpuPropertyFamilyIdx())
{
#ifdef _WIN32
    hModule = GetModuleHandle(NULL);

    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hModule;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = appName;
    wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    if (!RegisterClassEx(&wndClass)) {
        throw std::exception("Unable to register window class.");
    }

    hWindow = CreateWindowEx(
        0,
        appName,
        appName,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        8, 8,
        256, 256,
        NULL,
        NULL,
        hModule,
        NULL);
    if (!hWindow) {
        throw std::exception("Unable to create window.");
    }
#endif
}


Gfx::~Gfx()
{
}
