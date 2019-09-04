#pragma once

#include "GfxInstance.h"
#include "Platform.h"
#include "GfxDevice.h"
#include "SwapChain.h"


class Gfx
{
public:
    Gfx(const char* appName);
    ~Gfx();
private:
    const char* appName;
    GfxInstance instance;
    Platform platform;
    GfxDevice device;
    SwapChain swapChain;
};
