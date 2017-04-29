//=============================================================================================
// main.cpp
//  - test driver for cmdline library
//=============================================================================================

#include "cmdline/cmdline.h"

#include <stdio.h>

void CmdZeroParms();
void CmdOnePositionalParameter();
void PrintArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	cmdline::Cmdline cmd(argc, argv, R"raw(
test-cmdline: Test cmdline library
)raw");

	CmdZeroParms();
    CmdOnePositionalParameter();
}

// Build and exercise a command-line that takes no parameters
void CmdZeroParms()
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
void CmdOnePositionalParameter()
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
        printf("dir=%s", cmd["dir"].string());

    printf("\n");
}

void PrintArgs(int argc, char* argv[])
{
    printf("argc=%d\n", argc);
    for (int i = 0; i < argc; i++)
        printf("argv[%d]=%s\n", i, argv[i]);
}
