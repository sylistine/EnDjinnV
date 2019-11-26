#include "EnDjinnV.h"
#include "Exception.h"

#include <boost/stacktrace.hpp>

using namespace Djn;

Exception::Exception() : exception("")
{
    Init();
}


Exception::Exception(const char* str) : exception(str)
{
    Init();
}


Exception::Exception(std::string str) : exception(str.c_str())
{
    Init();
}


void Exception::Init()
{
    trace = boost::stacktrace::to_string(boost::stacktrace::stacktrace());
}
