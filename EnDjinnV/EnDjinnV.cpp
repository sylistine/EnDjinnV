#include "EnDjinnV.h"

#include <iostream>

#include "Platform.h"
#include "Gfx.h"

int main()
{
#if defined(_WIN32) && defined(NDEBUG)
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    std::cout << "Starting EnDjinn." << std::endl;
    using namespace Djn;
    auto appName = "Vulkan Exploration";
    try {
        std::cout << "Constructing platform." << std::endl;
        Platform platform(appName);
        std::cout << "Initializing graphics." << std::endl;
        Gfx::Manager::Initialize(platform.GetVkInstance(), platform.GetSurface());
        std::cout << "Looping forever." << std::endl;
        while (true) {}
    } catch (const std::exception& e) {
        std::cout << "Exception caught." << std::endl;
        // TODO: setup that logger again.
        std::cout << e.what() << std::endl;
    }
    std::cout << "Terminating EnDjinn" << std::endl;
}