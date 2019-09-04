#include <vector>
#include <iostream>

#include "Gfx.h"

int main()
{
    try {
        std::cout << "Engine startup." << std::endl;
        Gfx gfx("EnDjinn");
        while (true) {}
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        std::cout << "Terminating app." << std::endl;
    }
}