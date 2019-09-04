#include "Gfx.h"

Gfx::Gfx(const char* appName) :
    appName(appName),
    instance(appName),
    platform(appName, instance.Get()),
    device(instance.Get(), platform.GetSurface()),
    swapChain(device, platform)
{
}


Gfx::~Gfx()
{
}
