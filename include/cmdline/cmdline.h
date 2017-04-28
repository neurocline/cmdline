// cmdline.h

#pragma once

#include <string>

namespace cmdline
{

class Cmdline
{
public:
	Cmdline(int argc, char** argv, char* spec_);
	~Cmdline();

    const std::string& usage();
private:

    char* spec;
    char* specEnd;
    std::string usageMsg;
};

}
