//=============================================================================================
// main.cpp
//=============================================================================================

#include <stdio.h>

#include "cmdline/cmdline.h"

#define PROGRAM_VERSION "1.0"

extern const char* git_options;

int main(int argc, char* argv[])
{
	printf("My wonderful command-line tester\n");


	cmdline::Cmdline c(git_options);
	bool ok = c.parse(argc, argv);
	if (!ok)
	{
		printf("Command-line parsing failed\n");
		return 1;
	}

	printf("Command-line parsing succeeded\n");
	if (c["version"].exists)
		printf("version " PROGRAM_VERSION "\n");
	return 0;
}

const char* git_options = R"raw(
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
)raw";
