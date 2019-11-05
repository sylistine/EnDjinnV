#include "EnDjinnV.h"

#include <iostream>

#include "Platform.h"
#include "Gfx.h"

int main()
{
#if defined(_WIN32) && defined(NDEBUG)
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif

    auto appName = "Vulkan Exploration";
    try {
        Djn::Platform platform(appName);
        Djn::Manager::Initialize(platform.GetVkInstance(), platform.GetSurface());
        while (true) {}
    } catch (const std::exception& e) {
        // TODO: setup that logger again.
        std::cout << e.what() << std::endl;
    }
}