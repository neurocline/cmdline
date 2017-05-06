//=================================================================================================
// cmdline.cpp
//  - command-line parser
//=================================================================================================

#include "cmdline/cmdline.h"

#include <string.h>
#include <sstream>
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
    for (auto v : values)
        delete v;
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

	return *(pos->second);
}

//=================================================================================================

// Construct state string
const std::string cmdline::Cmdline::state()
{
    std::stringstream buf;

    // argv parts
    buf << "Argv substrings: (" << argv_parts.size() << ")\n";
    for (auto& parts : argv_parts)
        buf << "    " << parts << "\n";

    // positionals
    buf << "Positional arguments: (" << positionals.size() << ")\n";
    for (auto& pos : positionals)
        buf << "    " << pos << "\n";

    // options
    int used = 0;
    for (auto& opt : options)
        if (opt.second->exists())
            used += 1;

    buf << "Options used: (" << used << ")\n";
    for (auto& opt : options)
        if (opt.second->exists())
            buf << "    " << opt.first << ": " << opt.second->print() << "\n";

    buf << "Options not used: (" << (options.size() - used) << ")\n";
    for (auto& opt : options)
        if (!opt.second->exists())
            buf << "    " << opt.first << ": " << opt.second->print() << "\n";

    std::string rval = buf.str();
    return rval;
}

//=================================================================================================

const std::string cmdline::Value::print()
{
    std::stringstream buf;

    buf << "{ exist: " << (valid ? "true" : "false");
    buf << ", nargs: " << num_args;
    buf << ", str: " << (str == nullptr ? "<null>" : str) << "}";

    return buf.str();
}

//=================================================================================================

void Cmdline::eval(int argc, char** argv)
{
    // Populate values into the command-line. Positional args are assigned by relative
    // offset in the command line

    int positional = 0;
    int i = 1; // first arg is always program name
    for (; i < argc; i++)
    {
        // If this is a positional argument, find and assign it
        if (argv[i][0] != '-')
        {
            auto pos = options.find(positionals[positional]);
            if (pos == options.end())
                break; // this is a bad argument

            pos->second->set(argv[i]);
            positional++;
        }

        // Otherwise, it must be a named argument. This could be a --option=value
        // or --option value; only arguments that can take values allow them.
        // TBD we don't handle --option=value yet
        else
        {
            const char* opt = argv[i];
            if (*opt == '-') opt++;
            if (*opt == '-') opt++;

            // If we find a '=' character in the argument, then split it into pieces
            // (and qq now we need to allocate some strings)
            const char* eq = strchr(opt, '=');
            const char* opt_val = nullptr;
            if (eq != nullptr)
            {
                int csize = strlen(opt);
                int argsize = eq - opt;
                int valsize = csize - argsize - 1;
                argv_parts.push_back(std::string(opt, argsize));
                argv_parts.push_back(std::string(eq + 1, valsize));
                opt = (argv_parts.end() - 2)->c_str();
                opt_val = argv_parts.back().c_str();
            }

            auto pos = options.find(opt);
            if (pos == options.end())
                break; // this is a bad argument

            // If this argument consumes values, then get them
            // TBD we just handle one value at the moment
            if (pos->second->nargs() > 0)
            {
                int n = pos->second->nargs();
                for (; n > 0; n--)
                {
                    if (opt_val != nullptr)
                    {
                        pos->second->set(opt_val);
                        continue;
                    }

                    i += 1;
                    if (i >= argc)
                        break; // syntax error
                    auto val = argv[i];
                    pos->second->set(val);
                }
            }

            // If it takes no args, it's a boolean
            else
                pos->second->set("True");
        }
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
    bool NAMED(int& pos, Fragment& f, Value*& v);
    bool NAMEDLIST(int& pos, Fragment& f);

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
//  CMDLINE ::= (TEXT | POSITIONAL | NAMEDLIST)
//  TEXT ::= string+
//  POSITIONAL ::= ^ '<' ARGUMENT '>' TEXT
//  NAMEDLIST ::= NAMED (',' NAMED)*
//  NAMED ::= '-' '-'? ARGUMENT ('='? VALUE)?
//  ARGUMENT ::= string+

bool internal::Parser::parse()
{
    int pos = 0;

    for (; pos < textEnd - text; )
    {
	    Fragment f;
        if (TEXT(pos, f))
            continue;

        if (POSITIONAL(pos, f))
            continue;

        if (NAMEDLIST(pos, f))
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
        if (linestart && (*e == '<' || *e == '[' || *e == '-'))
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
    auto v = new Value;
    cmd->values.push_back(v);
    cmd->options[arg] = v;
    cmd->positionals.push_back(arg);

    pos = p;
    return true;
}

// ------------------------------------------------------------------------------------------------

bool internal::Parser::NAMEDLIST(int& pos, Fragment& f)
{
    int p{ pos };

    // We share the same Value among all the synonyms, and we
    // use the first one created
    Value* v = nullptr;

    // there must be at least one NAMED to start with
    if (!NAMED(p, f, v))
        return false;

    // We can have zero or more NAMED following this. Since we are still
    // expecting a NAMED, reset the SOL marker too.
    int pstart{ p };
    for (;;)
    {
        ConsumeWhitespace(p);
        if (!MatchChar(p, ','))
            break;
        ConsumeWhitespace(p);
        if (!NAMED(p, f, v))
            break;
        pstart = p; // we successfully found another token
    }
    p = pstart;

    pos = p;
    return true;
}

// Consume a complete NAMED nonterminal or consume nothing
bool internal::Parser::NAMED(int& pos, Fragment& f, Value*& v)
{
    int p{ pos };

    // Consume the beginning
    if (!MatchChar(p, '-'))
        return false;
    MatchChar(p, '-');

    // Consume an ARGUMENT
    if (!ARGUMENT(p, f))
        return false;

    // Consume optional VALUEs
    int narg = 0;
    while (p < textEnd - text)
    {
        int argpos{ p };
        Fragment farg;

        ConsumeWhitespace(argpos);
        MatchChar(argpos, '='); // optional

        if (!MatchChar(argpos, '<'))
            break;
        if (!ARGUMENT(argpos, farg))
            break;
        narg += 1;
        if (!MatchChar(argpos, '>'))
            break;

        p = argpos;
    }

    // We now have a named argument. The simple version is bool-if-exists, or
    // an argument count if it takes further arguments
    if (v == nullptr)
    {
        v = new Value("False", false);
        cmd->values.push_back(v);
    }
    if (narg > 0)
        v->nargs(narg);

    std::string arg(f.b, f.e - f.b);
    cmd->options[arg] = v;

    pos = p;
    return true;
}

// ------------------------------------------------------------------------------------------------

// Consume an ARGUMENT non-terminal. For now, this is just a name, e.g. anything up
// a non-argument character
bool internal::Parser::ARGUMENT(int& pos, Fragment& f)
{
    auto begin = pos;
    const char* b = &text[pos];
    const char* e = b;
    for (; e < textEnd; e++)
    {
        if (*e == ']' || *e == '>' || *e == ' ' || *e == '\n' || *e == ',')
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
