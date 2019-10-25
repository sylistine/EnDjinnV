#pragma once

#include <Windows.h>

namespace Djn::XPlat
{
    void Initialize(const char* appName);
    bool Update();
    // TODO: implement input and timing?
    namespace Windows
    {
        HINSTANCE GetModuleInstanceHandle();
        HWND GetWindowHandle();
    }
}
