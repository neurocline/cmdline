//=============================================================================================
// main.cpp
//  - test driver for cmdline library
//=============================================================================================

#include "cmdline/cmdline.h"

#include <stdio.h>

void CmdZeroParms();
void PrintArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	cmdline::Cmdline cmd(argc, argv, R"raw(
test-cmdline: Test cmdline library
)raw");

	CmdZeroParms();
}

// Build and exercise a command-line that takes no parameters
void CmdZeroParms()
{
    printf("-------------------------------------------\n");
    printf("CmdZeroParms\n");
	int argc = 1;
	char* argv[] = { "cmd0" };
    PrintArgs(argc, argv);

	cmdline::Cmdline cmd(argc, argv, R"raw(
cmd0: Command with no parameters
)raw");

    printf("\n");
    printf("Help message\n");
    puts(cmd.usage().c_str());

    printf("\n");
}

void PrintArgs(int argc, char* argv[])
{
    printf("argc=%d\n", argc);
    for (int i = 0; i < argc; i++)
        printf("argv[%d]=%s\n", i, argv[i]);
}
