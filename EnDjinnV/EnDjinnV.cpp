#include <vector>
#include <iostream>

#include "Gfx.h"

int main()
{
    try {
        Gfx gfx("EnDjinn");
        while (true) {}
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        std::cout << "Terminating app." << std::endl;
    }
}