//=============================================================================================
// cmdline.h
//=============================================================================================

#pragma once

#include <map>

namespace cmdline
{

//---------------------------------------------------------------------------------------------

struct Value
{
	bool exists;
	const char* string;
};

class Cmdline
{
public:
	// Initialize a Cmdline object with the options description
	// When using this constructor, you are guaranteeing that desc will not move or change
	// for the lifetime of the Cmdline object. Since these strings are typically from
	// code, this is normal behavior.
	Cmdline(const char* desc);

	// This constructor will copy the passed-in string, so, really, why are you using it?
	Cmdline(const std::string& desc);

	// Do the parse of the command-line
	bool parse(int argc, char* argv[], bool noMixed = false);

	// Use operator[] to get an option's value. If you ask for an option that wasn't actually
	// presented on the command line, you'll get the noValue object and Value.exists will be false
	const Value& operator[](const char* option);

	void study();

	const char* opt;
	std::string descString;
	std::map<std::string, Value> options;

	Value noValue;
};

//---------------------------------------------------------------------------------------------

} // namespace cmdline
