#pragma once

#include "GfxInstance.h"
#include "GfxDevice.h"
#include "Platform.h"


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
};
