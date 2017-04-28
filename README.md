Command Line
============

This is a command-line parser with the following goals

1. Easy to use
2. Minimize boilerplate and duplication when specifying command-line options
3. Allow free-form help
4. Support arbitrary character sets
5. Easy to integrate

For operating systems with argc+argv:

```
	cmdline::Cmdline c(argc, argv, R"raw(
Bloviate the argosphere

usage: blov [<options>] <file>

[--version]       show version number
[--ingest=<url>]  augment default argosphere with contents of <url>
<file>            path to write output of bloviation
)raw");
```

This is inspired by docopt.

Command-line grammar
--------------------

A command-line has a collection of named and positional arguments.

Named arguments begin with `-` or `--`, and can define zero or more parameters following.

We follow the GNU standard for named arguments

- `-o` for single-letter optional arguments
- `--option` for multi-letter optional arguments

It is possible to have mandatory optional arguments.
