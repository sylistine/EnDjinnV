#pragma once

#include "EnDjinnV.h"

#include <string>


namespace Djn
{
    class Exception : public std::exception
    {
    public:
        Exception();
        Exception(std::string str);
    };
}
