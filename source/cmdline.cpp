//=============================================================================================
// cmdline.cpp
//=============================================================================================

#define _CRT_SECURE_NO_WARNINGS

#include "cmdline.h"

cmdline::Cmdline::Cmdline(const char* desc)
{
	noValue.exists = false;
	opt = desc;
}

cmdline::Cmdline::Cmdline(const std::string& desc)
{
	noValue.exists = false;
	descString = desc; // make a copy of the string (expensive)
	opt = descString.c_str();
}

bool cmdline::Cmdline::parse(int argc, char* argv[], bool noMixed)
{
	study();
	for (int i = 0; i < argc; i++)
	{
		argv[i];
	}
	noMixed;
	return true;
}

const cmdline::Value& cmdline::Cmdline::operator[](const char* option)
{
	auto pos = options.find(option);
	if (pos == options.end())
		return noValue;
	
	return pos->second;
}

/*
Grammar (kinda)

CMDLINE  ::= OPTIONS TEXT
OPTIONS  ::= OPTARG*
OPTARG   ::= OPTION | ARGUMENT
OPTION   ::= '[' OPTNAME ']' TEXT
ARGUMENT ::= '<' ARGNAME '>' TEXT
TEXT     ::= | <string>

CMDLINE  ::= (TEXT | OPTION)*
TEXT     ::= <string>
OPTION   ::= '[' OPTBODY ']' SP SP+ TEXT*
OPTBODY  ::= OPTNAME ('=' OPTARG)?
*/

namespace cmdline
{

class Parser
{
public:
	Parser(const char* text) : text(text) {}

	struct Fragment
	{
		Fragment() { b = nullptr; e = nullptr; }
		const char* b;
		const char* e;
	};

	struct OptionValue
	{
		const char* name;
		const char* nameEnd;
		const char* text;
		const char* textEnd;
	};

	struct OptionName
	{
		const char* name;
		const char* nameEnd;
	};

	struct ArgumentValue
	{
		const char* name;
		const char* nameEnd;
		const char* text;
		const char* textEnd;
	};

	struct ArgumentName
	{
		const char* name;
		const char* nameEnd;
	};

	bool CMDLINE();
	bool OPTIONS(int& pos);
	bool TEXT(int& pos, Fragment& f);
	bool OPTARG(int& pos);
	bool OPTION(int& pos, OptionValue& ovalue);
	bool ARGUMENT(int& pos, ArgumentValue& f);
	bool OPTNAME(int& pos, OptionName& f);
	bool ARGNAME(int& pos, ArgumentName& f);

	bool LBRACKET(int& pos);
	bool RBRACKET(int& pos);
	bool LESSTHAN(int& pos);
	bool GREATERTHAN(int& pos);

	void SkipWhitespace(int& pos);

	const char* text;
};

bool Parser::CMDLINE()
{
	int pos = 0;
	if (!OPTIONS(pos))
		return false;

	Fragment f;
	TEXT(pos, f);
	// should be at the end of the string now
	if (pos != (int) strlen(text))
		return false;

	return true;
}

bool Parser::OPTIONS(int& pos)
{
	int p{ pos };
	for (;;)
	{
		if (!OPTARG(p))
			break;
	}
	Fragment f;
	TEXT(p, f);
	pos = p;
	return true;
}

bool Parser::OPTARG(int& pos)
{
	int p{ pos };
	OptionValue ovalue;
	if (OPTION(p, ovalue))
	{
		pos = p;
		return true;
	}

	ArgumentValue avalue;
	if (ARGUMENT(p, avalue))
	{
		pos = p;
		return true;
	}
	return false;
}

bool Parser::OPTION(int& pos, OptionValue& ovalue)
{
	int p{ pos };
	if (!LBRACKET(p)) return false;
	OptionName oname;
	if (!OPTNAME(p, oname)) return false;
	if (!RBRACKET(p)) return false;
	Fragment f;
	TEXT(p, f); // error check?
	ovalue.name = oname.name;
	ovalue.nameEnd = oname.nameEnd;
	ovalue.text = f.b;
	ovalue.textEnd = f.e;
	pos = p;

	char optname[256];
	int len = ovalue.nameEnd - ovalue.name;
	if (len >= 256) len = 255;
	strncpy(optname, ovalue.name, len);
	optname[len] = 0;
	char help[512];
	len = ovalue.textEnd - ovalue.text;
	if (len >= 256) len = 255;
	strncpy(help, ovalue.text, len);
	help[len] = 0;
	printf("option:\n   name=%s\n    help=%s\n", optname, help);

	return true;
}

bool Parser::ARGUMENT(int& pos, ArgumentValue& avalue)
{
	int p{ pos };
	if (!LESSTHAN(p)) return false;
	ArgumentName aname;
	if (!ARGNAME(p, aname)) return false;
	if (!GREATERTHAN(p)) return false;
	Fragment f;
	TEXT(p, f); // error check?
	avalue.name = aname.name;
	avalue.nameEnd = aname.nameEnd;
	avalue.text = f.b;
	avalue.textEnd = f.e;
	pos = p;

	char argname[256];
	int len = avalue.nameEnd - avalue.name;
	if (len >= 256) len = 255;
	strncpy(argname, avalue.name, len);
	argname[len] = 0;
	char help[512];
	len = avalue.textEnd - avalue.text;
	if (len >= 256) len = 255;
	strncpy(help, avalue.text, len);
	help[len] = 0;
	printf("argument:\n   name='%s'\n    help='%s'\n", argname, help);

	return true;
}

bool Parser::OPTNAME(int& pos, OptionName& oname)
{
	int p{ pos };
	SkipWhitespace(p);
	const char* b = &text[p];
	const char* e = b;
	while (*e && *e != ']')
		e += 1;
	p = e - text;
	pos = p;
	oname.name = b;
	oname.nameEnd = e;
	return e > b;
}

bool Parser::ARGNAME(int& pos, ArgumentName& f)
{
	int p{ pos };
	SkipWhitespace(p);
	const char* b = &text[p];
	const char* e = b;
	while (*e && *e != '>')
		e += 1;
	p = e - text;
	pos = p;
	f.name = b;
	f.nameEnd = e;
	return e > b;
}

bool Parser::TEXT(int& pos, Fragment& f)
{
	int p{ pos };
	SkipWhitespace(p);
	
	// Text runs to the end of input, or until we see something
	// that looks like it can be an option or argument.
	const char* b = &text[p];
	const char* e = b;
	bool linestart = false;
	for (; *e; e += 1)
	{
		// If this is a newline, then note it
		if (*e == '\n' || *e == '\r')
		{
			linestart = true;
			continue;
		}
		// We only expect options or arguments at start of line
		// Whitespace does not reset start of line
		if (linestart)
		{
			if (*e == ' ' || *e == '\t')
				continue;
			if (*e == '[' || *e == '<')
				break;
		}
	}

	pos = e - text;
	f.b = b;
	f.e = e;
	return b < e;
}

// Skip past whitespace. There are no comments allowed at the moment.
void Parser::SkipWhitespace(int& pos)
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

#define MATCH_CHAR(c) \
	int p{ pos }; \
	SkipWhitespace(p); \
	if (text[p] != c) \
		return false; \
	pos = p + 1; \
	return true \

bool Parser::LBRACKET(int& pos) { MATCH_CHAR('['); }
bool Parser::RBRACKET(int& pos) { MATCH_CHAR(']'); }
bool Parser::LESSTHAN(int& pos) { MATCH_CHAR('<'); }
bool Parser::GREATERTHAN(int& pos) { MATCH_CHAR('>'); }

}


void cmdline::Cmdline::study()
{
	cmdline::Parser p(opt);
	p.CMDLINE();
}
