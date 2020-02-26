#pragma once

#include "EnDjinnV.h"

#include <string>


namespace Djn
{
    class Exception : public std::exception
    {
    public:
        Exception() : Exception("") {}
        Exception(std::string str) :
            exception((str + "\n\n" + boost::stacktrace::to_string(boost::stacktrace::stacktrace(2, 20))).c_str())
        {}
    };
}
