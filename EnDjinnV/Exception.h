#pragma once

#include "EnDjinnV.h"

#include <string>


namespace Djn
{
    class Exception : public std::exception
    {
    public:
        Exception();
        Exception(const char* str);
        Exception(std::string str);
        const char* StackTrace() const { return trace.c_str(); }
    private:
        void Init();
        std::string trace;
    };
}
