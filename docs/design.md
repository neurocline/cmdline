Design
======

Basic usage: a command-line that takes no parameters but the body is used as 
a help message.

```c++
	int argc = 1;
	char* argv[] = { "cmd0" };

	cmdline::Cmdline cmd(argc, argv, R"raw(
cmd0: Command with no parameters
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
