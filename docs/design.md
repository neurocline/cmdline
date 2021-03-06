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

Named parameters with values
----------------------------

It's common for named parameters to take values, and not just be booleans. Returning to `git clone`,
we want to say this:

```
usage: git clone [<options>] [--] <repo> [<dir>]

    -j, --jobs <n>        number of submodules cloned in parallel
    --reference <repo>    reference repository
```

When we have synonyms, we expect each synonym to take the same parameter, so it's reasonable
to not need to name it twice.

At first, we're just going to ignore the name we assign to the value of the named parameter.
What we will do is mark this value as requiring a parameter - this is so that we know to
consume a token from the command-line when we parse the actual command-line arguments.

When parsing command-line arguments, we want to support both styles of named parameter arguments:

```
git-clone -j 2
git-glone -j=2
```

The equal sign is optional and implied.

Note that on Windows, arguments are split by whitespace only, so `git-clone j=2` has `argc` of 2,
whereas `git-clone j 2` has `argc` of 3. Handling of `=` to split parameters has to be handled
inside the parsing of the `argv` array.

Handling the '=' operator
-------------------------

Naive command-line argument splitting sees text like `jobs=2` as a single argument token.
For cases where we know that this is really `jobs`, `=`, `2`, we need to do the splitting
when we process the `argv` array.

We also only want to do this splitting in specific cases.

At the point where we see that we have a named argument, then it's reasonable to turn `--arg=x`
into three tokens, and check to see if `arg` is a named argument.

There is a more complicated case where `=` is used to have a key-value pair assigned in a
named parameter. Again from `git clone`:

```
    -c, --config <key=value>
```

The expectation here is that this is used as follows:

```
git clone --config user.email=bfitz@blizzard.com
```

This is starting to get into esoteric behavior that might not be part of the built-in behavior
of a command-line parser. But it is a pattern that is used from time to time, so we should
probably support it.

Types
-----

We can allow annotation of parameter names with types as follows:

```
    -j, --jobs <n:int>      number of submodules cloned in parallel
    -v, --verbose <:bool>   be more verbose
    --reference <repo:str>  reference repository
```

We default to 0-argument options being `bool` and 1-argument options being `str`. The second and
third options above have redundant type info. We introduce a nameless argument name syntax to
note that this is a 0-argument option but that it has a type.

We can do lists by adding a `@` suffix to the type (which if mising, is a `str`):

```
    -c, --config <key=value:@>
                          set config inside the new repository
```

This turns the value of the `--config` option into an array. Without `@`, multiple uses of `--config`
would be an error. Note that if we can work out an acceptable human-readable syntax, this becomes
documentation in the usage line as well as for generating the parser. This is our goal, that the
help for the user is also the instructions to the compiler.

argparse
========

For parity with the Python `argparse` library, there are more features to add.

Help text before and after argument help. Shared arguments between parsers. The ability to change the
opptional argument prefix.  Ability to read arguments from a file. Auto-help argument.
Allow options to be abbreviated. Sub-parsers.

Argument actions: store, store_const, store_true, store_false, append, append_const, count, plus
oddballs help and version.

We already implicitly handle store_true vs store. We need to add append for allowing the creation
of a list. The count action is goofy but we could support it. The store_const and append_const are
not absolutely necessary. Instead, is there a way to support custom actions? This is the down side
of a purely textual approach; C++ doesn't have reflection or introspection, so we can't match a
string in text against a type or function in code; we would need a data structure to support that,
and now we're back to the original problem that a syntax has to be memorized.

We need to support nargs. Right now we have 0 and 1, but we need "0 or 1" and "0 or more", and
"1 or more". Technically speaking, we only need "0 or more", because the caller could do any
further error checking, but we want to balance ease of use.

One thing we probably do need is "remaining", e.g. an argument that gathers up all remaining
arguments; this is needed if we want to do some kind of child parser that can't be described in
our syntax.

We need types, again mostly for easy error handling. We can support type conversion easily
enough, as helpers. Here is where custom type handling comes in handy.

Actually, there is a way to do the introspection in a way that's not too onerous or hard to
remember. We can have the user supply a map of strings to functors.

We need to support `--` which ends named arguments; everything after must be a positional argument.

We should support parsing from a single string which is the command-line as well as parsing
an argv array. But the parsing should follow the operating system rules so that code remains
consistent.
