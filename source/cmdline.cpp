//=================================================================================================
// cmdline.cpp
//  - command-line parser
//=================================================================================================

#include "cmdline/cmdline.h"

#include <string>

namespace cmdline
{

// Parse a command line instance according to the spec
Cmdline::Cmdline(int argc, char** argv, char* spec_) : spec(spec_)
{
    int len = strlen(spec);
    specEnd = spec + len;

    // Skip leading newlines as being artifacts of how embedded
    // specs are supplied (typically with R"raw(...)raw" strings)
    while ((spec < specEnd && spec[0] == '\n')
        || (spec+1 < specEnd && spec[0] == '\r' && spec[1] == '\n')
    )
        spec += 1;

    // For now, the entire spec is also the help message
    usageMsg = std::string(spec);

    // Parse the spec and assign values to parameters
    study();
    eval(argc, argv);
}

Cmdline::~Cmdline()
{
}

const std::string& Cmdline::usage()
{
    return usageMsg;
}

// Find a parameter and return its value. All declared parameters are
// in the parsed object, but parameters not supplied on the command-line
// have null values
const cmdline::Value& cmdline::Cmdline::operator[](const char* option)
{
	auto pos = options.find(option);

    // If we don't find it, it's actually a syntax error by the caller, so
    // we should do something in debug like throw or assert.
	if (pos == options.end())
		return noValue;

	return pos->second;
}

//=================================================================================================

void Cmdline::eval(int argc, char** argv)
{
    // Populate values into the command-line. Positional args are assigned by relative
    // offset in the command line

    int positional = 0;
    for (int i = 1; i < argc; i++)
    {
        // If this is a positional argument, find and assign it
        if (argv[i][0] != '-')
        {
            auto pos = options.find(positionals[positional]);
            if (pos == options.end())
                break; // this is a bad argument

            Value v(argv[i]);
            pos->second = v;
            positional++;
        }

        // Otherwise, it must be a named argument
    }
}

//=================================================================================================

namespace internal
{
class Parser
{
public:
    Parser(const char* text, const char* textEnd, Cmdline* cmd);
    bool parse();

private:
    const char* text;
    const char* textEnd;
    bool linestart; // true when at beginning of line including whitespace

    Cmdline* cmd; // pointer to upstream commandline

    struct Fragment
    {
		Fragment() { b = nullptr; e = nullptr; }
        const char* b;
        const char* e;
    };
    bool TEXT(int& pos, Fragment& f);
    bool POSITIONAL(int& pos, Fragment& f);
    bool ARGUMENT(int& pos, Fragment& f);

    bool MatchChar(int& pos, char c);

    void ConsumeWhitespace(int& pos);
};
}

void Cmdline::study()
{
    internal::Parser parser(spec, specEnd, this);
    if (!parser.parse())
        failed = true; // save parsing error
}

internal::Parser::Parser(const char* text, const char* textEnd, Cmdline* cmd_)
    : text(text), textEnd(textEnd), cmd(cmd_)
{
    linestart = true;
}

// ------------------------------------------------------------------------------------------------

// Grammar is something like this (where ^ means line start)
//  CMDLINE ::= (TEXT | POSITIONAL)
//  TEXT ::= string+
//  POSITIONAL ::= ^ '<' ARGUMENT '>' TEXT
//  ARGUMENT ::= string+

bool internal::Parser::parse()
{
    int pos = 0;

    for (;;)
    {
	    Fragment f;
        if (TEXT(pos, f))
            continue;

        if (POSITIONAL(pos, f))
            continue;

        // syntax error
        break;
    }

    return pos == (textEnd - text);
}

// ------------------------------------------------------------------------------------------------

// Consume TEXT up until a POSITIONAL starts
bool internal::Parser::TEXT(int& pos, Fragment& f)
{
    auto begin = pos;
    const char* b = &text[pos];
    const char*e = b;
    for (; e < textEnd; e++)
    {
        if (e[0] == '\n')
        {
            linestart = true;
            continue;
        }
        if (e+1 < textEnd && e[0] == '\r' && e[1] == '\n')
        {
            e += 1;
            linestart = true;
            continue;
        }

        // Is this a symbol that terminates text mode?
        if (linestart && (*e == '<' || *e == '['))
            break;

        // See if we are no longer at the "start" of a line
        if (*e != ' ' && *e != '\t')
            linestart = false;
    }

    pos = e - text;
    if (pos == begin)
        return false;

    f.b = b;
    f.e = e;
    return true;
}

// ------------------------------------------------------------------------------------------------

// Consume a complete POSITIONAL nonterminal or consume nothing
bool internal::Parser::POSITIONAL(int& pos, Fragment& f)
{
    int p{ pos };

    // Consume the start symbol
    if (!MatchChar(p, '<'))
        return false;

    // Consume an ARGUMENT
    if (!ARGUMENT(p, f))
        return false;

    // Consume the end symbol
    ConsumeWhitespace(p);
    if (!MatchChar(p, '>'))
        return false;

    // At this point, we have all the pieces for a new positional argument
    std::string arg(f.b, f.e - f.b); // TBD force to lower case?
    cmd->options[arg] = cmd->noValue; // do this some other way
    cmd->positionals.push_back(arg);

    pos = p;
    return true;
}

// Consume an ARGUMENT non-terminal. For now, this is just a name, e.g. anything up
// a non-argument character
bool internal::Parser::ARGUMENT(int& pos, Fragment& f)
{
    auto begin = pos;
    const char* b = &text[pos];
    const char* e = b;
    for (; e < textEnd; e++)
    {
        if (*e == ']' || *e == '>')
            break;
    }
    pos = e - text;
    f.b = b;
    f.e = e;
    // TBD we should actually trim trailing whitespace or complain about it
    return pos > begin;
}

bool internal::Parser::MatchChar(int& pos, char c)
{
    if (text[pos] != c)
        return false;
    pos += 1;
    return true;
}

void internal::Parser::ConsumeWhitespace(int& pos)
{
	const char* p = &text[pos];
	while (*p)
	{
		if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
			p++;
		else
			break;
	}
	pos = p - text;
}

// ------------------------------------------------------------------------------------------------

} // namespace cmdline
