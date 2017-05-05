Design
======

No parameters
-------------

Basic usage: a command-line that takes no parameters but the body is used as 
a help message.

```c++
	int argc = 1;
	char* argv[] = { "pwd" };

	cmdline::Cmdline cmd(argc, argv, R"raw(
usage: pwd
    print working directory
)raw");

    puts(cmd.usage().c_str());
```

We are allowed to have as much comment text as possible. Any text not recognized as
a parameter (either positional or named) is ignored.

The first question is this: the way raw strings are typically used inserts a newline
as the first character. This makes the source more readable, but it inserts a newline that
we're tempted to want to skip, except that this discredits the user that does this to
actually get a newline as the first line.

Since I can't see a reason that the help message should start with a newline, we'll eat
leading newlines.

One positional parameter
------------------------

Positional parameters are, as indicated, at specific positions on the command-line (we'll
defer named parameters for now).

We're going to denote parameters in brackets, e.g. `<path>` is a positional parameter named
`path`.

```
usage: cd <dir>
    Change the shell working directory
```

This is a little less than ideal, because there are some challenges to parsing just
usage lines that we'll see when we get to options. So, without explaining, here is
a more verbose form that's easier to parse

```
usage: cd <dir>
    Change the shell working directory

positional arguments:
  <dir>  new working directory
```

The reason we don't want to key off just the usage line is that for complex commands,
the usage line is often a subset of what's possible, or multiple usage lines are
provided as examples. Sometimes you see both - the git command-lines are rich enough
that there is an initial synopsis, followed by a description, and then a number of
examples of usage.

Multiple positional parameters
------------------------------

In one respect, this is trivial. In another respect, it's challenging, because we can have
overloading of command lines. Take cp, for example. If there are two parameters, then
we have one meaning for the positional parameters, and if we have three or more, then
we actually have a different command:

With two parameter:

```
cp <source_file> <target_file>
```

With three or more parameters:

```
cp <source_file_1> <source_file_2> ... <target_directory>
```

In the first form, copy the file at path `source_file` to the path at `target_file`. In the second
form, copy all the files `source_file_1` ... to be in the directory `target_directory`.

We could order it by usage lines, which is also problematic; we might want multiple
usage lines, and then we need to disambiguate properly. There is no good answer here.

We also want to group multiple arguments up into a single name. This is for two
reasons

- we might have a list of arguments
- we have sub-parsers and need to pass some of the args to another parser

We'll ignore all of that for now. Multiple positional parameters are fine, each one has a
unique name, no lists.

Boolean named parameter
-------------------

The convention is that positional parameters are usually not optional, and named parameters
are usually optional. What makes it a named parameter? It has a name on the command line:

```
usage: ls [-a][-l]

optional arguments:
    -a     include directory entries that are normally hidden
    -l     list in long format
```

In this case, there are two optional named parameters. One is named `a`, and the other is
named `l`. Neither parameter takes a value; if the paramter is present, this means `true`,
if the parameter is not present, it means `false`.

Synonyms for named parameters
-----------------------------

The move to long option names was a good one - it's hard to keep 20 different single-character
option names straight, especially when it's a command you don't use all the time. However,
long names are descriptive but take time to type and read, so a convention developed
where the most common options had both long names and short names.

We could declare these as separate options, but that's redundant when reading help.
Instead, we want to specify all the pseudonyms on a single line.

```
usage: git clone [<options>] [--] <repository> [<directory>]

    <repository>       location of upstream repo
    <directory>        local directory to clone into (default to ./<repo-name>)

    -v, --verbose      be more verbose
    -q, --quiet        be more quiet
    --progress         force progress reporting
    -n, --no-checkout  don't create a checkout
    ...
```

This is not as simple as it seems. The set of synonyms all need to point to the same value;
if we have `-v` on the command line, then `cmd["verbose"]` should be true as well as `cmd["v"]`.
So this means we have a layer of indirection - there are a set of Values, and then specific
synonyms point to their Value.

This creates unneeded duplication for postional parameters, which can't have synonyms as far
as I can tell, but since command-lines typically have 10x the named parameters compared to
positional parameters, this isn't worth worrying about.
