//=================================================================================================
// cmdline.h
//  - command-line parser
//=================================================================================================

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
    Value() : str(nullptr), valid(false), num_args(0) {}
    Value(char* str_) : str(str_), valid(true), num_args(0) {}
    Value(char* str_, bool f_) : str(str_), valid(f_), num_args(0) {}
    ~Value() {}

    const char* string() const { return str; }
    bool exists() const { return valid; }
    int nargs(int n = -1) { if (n >= 0) num_args = n; return num_args; }

    void set(const char* s) { str = s; valid = true; }
    const std::string print();
private:
	const char* str;
    bool valid;
    int num_args; // number of arguments consumed
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

    // state returns internal state
    const std::string state();

    void study();
    void eval(int argc, char** argv);

    char* spec;
    char* specEnd;
    std::string usageMsg;
    bool failed; // bad spec

    // This is our list of all command-line options, and their parsed values.
    // TBD replace heavyweight std::string with interned strings?
    std::map<std::string, Value*> options;

    // This is the list of values, held separately because two options can
    // point to the same value
    std::vector<Value*> values;

    // This is the ordered list of position arguments
    std::vector<std::string> positionals;

    // This is a list of parameters split out of argv
    std::vector<std::string> argv_parts;

    // This is the default empty value, currently only used when operator[] can't find
    // an entry
	Value noValue;
};

}
