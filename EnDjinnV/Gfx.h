#pragma once

#include "GfxInstance.h"
#include "GfxDevice.h"

#ifdef _WIN32
#include <Windows.h>
#endif

class Gfx
{
public:
    Gfx(const char* appName);
    ~Gfx();
private:
    const char* appName;
    GfxInstance instance;
    GfxDevice device;
#ifdef _WIN32
    HINSTANCE hModule;
    HWND hWindow;
#endif
};
