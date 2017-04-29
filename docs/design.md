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

```c++
usage: cd <dir>
    Change the shell working directory
```

This is a little less than ideal, because there are some challenges to parsing just
usage lines that we'll see when we get to options. So, without explaining, here is
a more verbose form that's easier to parse

```c++
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
