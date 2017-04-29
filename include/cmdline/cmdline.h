// cmdline.h

#pragma once

#include <map>
#include <string>
#include <vector>

namespace cmdline
{

// This holds a single value for an option. If the string is null, then the option
// is not present
class Value
{
public:
    Value() : str(nullptr) { valid = true; }
    Value(char* str_) : str(str_) {}
    ~Value() {}

    const char* string() const { return str; }
    bool exists() const { return valid; /*return str != nullptr;*/ }
private:
	const char* str;
    bool valid;
};

class Cmdline
{
public:
    // Create a Cmdline object from a c-string spec and parse the supplied argv array
    // against the command-line spec
	Cmdline(int argc, char** argv, char* spec);
	~Cmdline();

	// Use operator[] to get an option's value. If you ask for an option that wasn't actually
	// presented on the command line, you'll get the noValue object and Value.exists will be false
	const Value& operator[](const char* option);

    // usage returns a usage/help string
    const std::string& usage();

    void study();
    void eval(int argc, char** argv);

    char* spec;
    char* specEnd;
    std::string usageMsg;
    bool failed; // bad spec

    // This is our list of all command-line options, and their parsed values.
    // TBD replace heavyweight std::string with interned strings?
    std::map<std::string, Value> options;

    // This is the ordered list of position arguments
    std::vector<std::string> positionals;

    // This is the default empty value - all options point to it at spec parse time,
    // then are updated with actual values (if any).
	Value noValue;
};

}
