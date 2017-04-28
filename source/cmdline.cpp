// cmdline.cpp

#include "cmdline/cmdline.h"

#include <string>

namespace cmdline
{

// Parse a command line instance according to the spec
Cmdline::Cmdline(int /*argc*/, char** /*argv*/, char* spec_) : spec(spec_)
{
    int len = strlen(spec);
    specEnd = spec + len;

    // Skip leading newlines as being artifacts of how embedded
    // specs are supplied (typically with R"raw(...)raw" strings)
    while ((spec < specEnd && spec[0] == '\n') || (spec+1 < specEnd && spec[0] == '\r' && spec[1] == '\n'))
        spec += 1;

    // For now, the entire spec is also the help message
    usageMsg = std::string(spec);
}

Cmdline::~Cmdline()
{
}

const std::string& Cmdline::usage()
{
    return usageMsg;
}

} // namespace cmdline
