#include "EnDjinnV.h"
#include "Exception.h"

#include <boost/stacktrace.hpp>

using namespace Djn;

Exception::Exception() : Exception("") {}


Exception::Exception(std::string str) :
    exception((str + "\n\n" + boost::stacktrace::to_string(boost::stacktrace::stacktrace(2, 20))).c_str())
{}
