
#if 0

/*
Our test case is going to be reproducing the Git command-line, as an example of something
both complex and useful.

Here is the top-level git command

git [--version] [--help] [-C <path>] [-c <name>=<value>]
[--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]
[-p|--paginate|--no-pager] [--no-replace-objects] [--bare]
[--git-dir=<path>] [--work-tree=<path>] [--namespace=<name>]
<command> [<args>]

Here it is broken down by structure
- characters except [] and <> represent themselves
- [] encloses an optional element
- <> indicates a syntactic/semantic object

[--version]
[--help]
[-C <path>]
[-c <name>=<value>]
[--exec-path[=<path>]]
[--html-path]
[--man-path]
[--info-path]
[-p|--paginate|--no-pager]
[--no-replace-objects]
[--bare]
[--git-dir=<path>]
[--work-tree=<path>]
[--namespace=<name>]
<command>
[<args>]

As a simple test, this might look as follows:
*/

struct cmdentry
{
	enum class cmdtype
	{
		none,
		string,
		string2,
		argument,
		arglist,
		optional = 128,
	};
	const char* optname;
	cmdtype type;
	const char* value;
	const char* value2;
};
const cmdentry::cmdtype& operator+(const cmdentry::cmdtype& a, const cmdentry::cmdtype& b)
{
	return (cmdentry::cmdtype)((int)a + (int)b);
}

cmdentry c[] = {
	{ "version" },
	{ "help" },
	{ "C", cmdentry::cmdtype::string },
	{ "c", cmdentry::cmdtype::string2, "key", "value" },
	{ "exec-path", cmdentry::cmdtype::optional + cmdentry::cmdtype::string , "path" },
	{ "html-path", cmdentry::cmdtype::none },
	{ "man-path", cmdentry::cmdtype::none },
	{ "info-path", cmdentry::cmdtype::none },
	{ "p|paginate|no-pager", cmdentry::cmdtype::none },
	{ "no-replace-objects", cmdentry::cmdtype::none },
	{ "bare", cmdentry::cmdtype::none },
	{ "git-dir", cmdentry::cmdtype::string, "path" },
	{ "work-tree", cmdentry::cmdtype::string, "path" },
	{ "namespace", cmdentry::cmdtype::string, "name" },
	{ "", cmdentry::cmdtype::argument, "command" },
	{ "", cmdentry::cmdtype::arglist, "args" }
};

/*
but this approach is not very elegant. it's not horrible. The Docopt approach is looking
pretty good after all. Here's how you specify the previous in Docopt. I may rewrite the
C++ version of Docopt to avoid using regex, to keep it really small.
*/

char* gitmain =
R"(usage: git [--version] [--exec-path=<path>] [--html-path]
           [-p|--paginate|--no-pager] [--no-replace-objects]
           [--bare] [--git-dir=<path>] [--work-tree=<path>]
           [-c <name>=<value>] [--help]
           <command> [<args>...]
options:
   -c <name=value>
   -h, --help
   -p, --paginate
The most commonly used git commands are:
   add        Add file contents to the index
   branch     List, create, or delete branches
   checkout   Checkout a branch or paths to the working tree
   clone      Clone a repository into a new directory
   commit     Record changes to the repository
   push       Update remote refs along with associated objects
   remote     Manage set of tracked repositories
See 'git help <command>' for more information on a specific command.
)";

/*
It's not precisely the Git command syntax, but close. Here's what git help looks
like at the moment (2.7.2)

C:\projects\github\neurocline\packages>git help
usage: git [--version] [--help] [-C <path>] [-c name=value]
[--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]
[-p | --paginate | --no-pager] [--no-replace-objects] [--bare]
[--git-dir=<path>] [--work-tree=<path>] [--namespace=<name>]
<command> [<args>]

These are common Git commands used in various situations:

start a working area (see also: git help tutorial)
clone      Clone a repository into a new directory
init       Create an empty Git repository or reinitialize an existing one

work on the current change (see also: git help everyday)
add        Add file contents to the index
mv         Move or rename a file, a directory, or a symlink
reset      Reset current HEAD to the specified state
rm         Remove files from the working tree and from the index

examine the history and state (see also: git help revisions)
bisect     Use binary search to find the commit that introduced a bug
grep       Print lines matching a pattern
log        Show commit logs
show       Show various types of objects
status     Show the working tree status

grow, mark and tweak your common history
branch     List, create, or delete branches
checkout   Switch branches or restore working tree files
commit     Record changes to the repository
diff       Show changes between commits, commit and working tree, etc
merge      Join two or more development histories together
rebase     Forward-port local commits to the updated upstream head
tag        Create, list, delete or verify a tag object signed with GPG

collaborate (see also: git help workflows)
fetch      Download objects and refs from another repository
pull       Fetch from and integrate with another repository or a local branch

push       Update remote refs along with associated objects

'git help -a' and 'git help -g' list available subcommands and some
concept guides. See 'git help <command>' or 'git help <concept>'
to read about a specific subcommand or concept.

*/

/*
Let's try again:

git [--version] [--help] [-C <path>] [-c <name>=<value>]
[--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]
[-p|--paginate|--no-pager] [--no-replace-objects] [--bare]
[--git-dir=<path>] [--work-tree=<path>] [--namespace=<name>]
<command> [<args>]
*/

char* opt = R"(
[--version]            Prints the Git suite version that the git program came from
[--help]               Prints the synopsis and a list of the most commonly used commands.
[-C <path>]            Run as if git was started in <path> instead of the current working directory. 
[-c <name>=<value>]    Pass a configuration parameter to the command.
[--exec-path[=<path>]] Path to wherever your core Git programs are installed.
[--html-path]          Print the path where Git’s HTML documentation is installed and exit.
[--man-path]           Print the manpath for the man pages for this version of Git and exit.
[--info-path]          Print the path where the Info files for this version of Git are installed and exit.
[-p|--paginate|--no-pager]
                       Pipe all output into pager if standard output is a terminal.||
                       Do not pipe Git output into a pager.
[--no-replace-objects] Do not use replacement refs to replace Git objects.
[--bare]               Treat the repository as a bare repository. 
[--git-dir=<path>]     Set the path to the repository. 
[--work-tree=<path>]   Set the path to the working tree.
[--namespace=<name>]   Set the Git namespace.
<command>
[<args>]
)";

/*
This should be easily parsable. The option names are the keys (in a map, presumably). If there
are values, then these are keys in the sub-map for the option and the value defaults to a string.
I think we should have sigils for types, like :int and :bool, and :string is the default.

Interesting. The synopsis for "git help" doesn't have all the options in it. I wonder if
this is on purpose.

Also, if every command has a short help string, it gets a little unreadable.
*/

#include <map>
struct Value { char* string; };
struct Option { bool stuff; operator bool(); Value& operator[](const char*); };
typedef std::map<char*, Option> Parsed;
Parsed& cmdline(char* desc, int argc, char* argv[]);
void print_version();

void test(int argc, char* argv[])
{
	auto options = cmdline(opt, argc, argv);
	if (options["version"]) { print_version(); }
	if (options["C"])
	{
		auto path = options["C"]["path"].string;
		if (path != nullptr) printf("C path = %s\n", path);
	}
}

/*
Or Option could even be a type with operator[] and we look stuff up dynamically,
parsing the options string to figure out what it means. That would be much slower
in execution time but really keep the space bounded, because there would be
no extra space for the parsed data. There's a tradeoff there, of course.
*/

#endif
