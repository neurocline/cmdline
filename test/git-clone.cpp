#include "cmdline/cmdline.h"
#include "bf/AutoRegister.h"

#include <stdio.h>
extern void PrintArgs(int argc, char* argv[]);

AUTO_REGISTER(GitClonePartial)
{
    printf("-------------------------------------------\n");
    printf("GitClone\n");
	int argc = 4;
	char* argv[] = { "git-clone", "-v", "-j=2", "git@github.com:neurocline/cmdline.git" };
    PrintArgs(argc, argv);

	cmdline::Cmdline cmd(argc, argv, R"raw(
usage: git clone [<options>] [--] <repository> [<directory>]

    <repository>          location of upstream repo
    <directory>           local directory to clone into (default to ./<repo-name>)

    -v, --verbose         be more verbose
    -q, --quiet           be more quiet
    --progress            force progress reporting
    -n, --no-checkout     don't create a checkout
    --bare                create a bare repository
    --mirror              create a mirror repository (implies bare)
    -l, --local           to clone from a local repository
    --no-hardlinks        don't use local hardlinks, always copy
    -s, --shared          setup as shared repository
    --recursive           initialize submodules in the clone
    --recurse-submodules  initialize submodules in the clone
    -j, --jobs <n>        number of submodules cloned in parallel
    --template <template-directory>
                          directory from which templates will be used
    --reference <repo>    reference repository
    --reference-if-able <repo>
                          reference repository
    --dissociate          use --reference only while cloning
    -o, --origin <name>   use <name> instead of 'origin' to track upstream
    -b, --branch <branch>
                          checkout <branch> instead of the remote's HEAD
    -u, --upload-pack <path>
                          path to git-upload-pack on the remote
    --depth <depth>       create a shallow clone of that depth
    --shallow-since <time>
                          create a shallow clone since a specific time
    --shallow-exclude <revision>
                          deepen history of shallow clone, excluding rev
    --single-branch       clone only one branch, HEAD or --branch
    --shallow-submodules  any cloned submodules will be shallow
    --separate-git-dir <gitdir>
                          separate git dir from working tree
    -4, --ipv4            use IPv4 addresses only
    -6, --ipv6            use IPv6 addresses only
)raw");

    printf("\n");
    printf("Usage message\n");
    puts(cmd.usage().c_str());

    if (!cmd["v"].exists())
        printf("Optional parameter 'v' missing\n");
    else
        printf("v=%s\n", cmd["v"].string());

    if (!cmd["verbose"].exists())
        printf("Optional parameter 'verbose' missing\n");
    else
        printf("verbose=%s\n", cmd["verbose"].string());

    if (!cmd["jobs"].exists())
        printf("Optional parameter 'jobs' missing\n");
    else
        printf("jobs=%s\n", cmd["jobs"].string());

    printf("\n");
}
