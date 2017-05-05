//=============================================================================================
// main.cpp
//  - test driver for cmdline library
//=============================================================================================

#include "cmdline/cmdline.h"

#include "bf/AutoRegister.h"

#include <stdio.h>

void PrintArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	cmdline::Cmdline cmd(argc, argv, R"raw(
test-cmdline: Test cmdline library
)raw");

    AutoRegister::RunAll();
    return 0;
}

void PrintArgs(int argc, char* argv[])
{
    printf("argc=%d\n", argc);
    for (int i = 0; i < argc; i++)
        printf("argv[%d]=%s\n", i, argv[i]);
}

// ------------------------------------------------------------------------------------------------

// Build and exercise a command-line that takes no parameters
//void CmdZeroParms()
AUTO_REGISTER(CmdZeroParms)
{
    printf("-------------------------------------------\n");
    printf("CmdZeroParms\n");
	int argc = 1;
	char* argv[] = { "pwd" };
    PrintArgs(argc, argv);

	cmdline::Cmdline cmd(argc, argv, R"raw(
usage: pwd
    print working directory
)raw");

    printf("\n");
    printf("Usage message\n");
    puts(cmd.usage().c_str());

    printf("\n");
}

// Build and exercise a command-line that takes one positional
// parameter
AUTO_REGISTER(CmdOnePositionalParameter)
{
    printf("-------------------------------------------\n");
    printf("CmdOnePositionalParameter\n");
	int argc = 2;
	char* argv[] = { "cd", ".." };
    PrintArgs(argc, argv);

	cmdline::Cmdline cmd(argc, argv, R"raw(
usage: cd <dir>
    Change the shell working directory

positional arguments:
  <dir>    new working directory
)raw");

    printf("\n");
    printf("Usage message\n");
    puts(cmd.usage().c_str());

    if (!cmd["dir"].exists())
        printf("Mandatory parameter 'dir' missing\n");
    else
        printf("dir=%s\n", cmd["dir"].string());

    printf("\n");
}

// Build and exercise a command-line that takes multiple positional
// parameter
AUTO_REGISTER(CmdMultiplePositionalParameters)
{
    printf("-------------------------------------------\n");
    printf("CmdMultiplePositionalParameters\n");
	int argc = 3;
	char* argv[] = { "cp", "test.bin", "test.bin.backup" };
    PrintArgs(argc, argv);

	cmdline::Cmdline cmd(argc, argv, R"raw(
usage: cp <source-file> <target-file>
    Copy file from source to target

positional arguments:
  <source-file>  path to source file
  <target-file>  path to target file
)raw");

    printf("\n");
    printf("Usage message\n");
    puts(cmd.usage().c_str());

    if (!cmd["source-file"].exists())
        printf("Mandatory parameter 'source-file' missing\n");
    else
        printf("source-file=%s\n", cmd["source-file"].string());

    if (!cmd["target-file"].exists())
        printf("Mandatory parameter 'target-file' missing\n");
    else
        printf("target-file=%s\n", cmd["target-file"].string());

    printf("\n");
}

// Build and exercise a command-line that takes a single
// named parameter
AUTO_REGISTER(CmdSingleNamedParameter)
{
    printf("-------------------------------------------\n");
    printf("CmdSingleNamedParameter\n");
	int argc = 2;
	char* argv[] = { "ls", "-a"};
    PrintArgs(argc, argv);

	cmdline::Cmdline cmd(argc, argv, R"raw(
usage: ls [-a]
    Show directory listing

optional arguments:
  -a    show all files, including hidden
)raw");

    printf("\n");
    printf("Usage message\n");
    puts(cmd.usage().c_str());

    if (!cmd["a"].exists())
        printf("Optional parameter '-a' missing\n");
    else
        printf("-a=%s\n", cmd["a"].string());
}

// Build and exercise a command-line that takes a single
// named parameter
AUTO_REGISTER(CmdSingleNamedParameterMissing)
{
    printf("-------------------------------------------\n");
    printf("CmdSingleNamedParameterMissing\n");
	int argc = 1;
	char* argv[] = { "ls"};
    PrintArgs(argc, argv);

	cmdline::Cmdline cmd(argc, argv, R"raw(
usage: ls [-a]
    Show directory listing

optional arguments:
  -a    show all files, including hidden
)raw");

    printf("\n");
    printf("Usage message\n");
    puts(cmd.usage().c_str());

    if (!cmd["a"].exists())
        printf("Optional parameter '-a' missing\n");
    else
        printf("-a=%s\n", cmd["a"].string());
}
