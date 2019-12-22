# AUTO-HEADER

This is an utility I created for myself and published for anyone that
may be interested in using it.

This program helps you to create headers for your source code files, to
add a copyright note, filename, function of the file, project, date...

This is done from a template and can be configured in detail, you just
run the program in a directory.

## How to use

First a template file has to be created in the project base folder with name
`.ahtemplate`.

Then, every file that we want to apply the template have to have a comment with
the key `$AH$` in its first line.

Finally we can run the program in the project folder to fill all the comments.

### Template creation

A template file example can be created running `auto-header --create-example`.

This file is the same as `.ahtemplate.example` in this repository.
You can edit it and rename it to `.ahtemplate`.

To see all possible macros that can be used in a template just run `auto-header --list-macros`.

### Program arguments

With `auto-header --help` all arguments and flags can be seen.

## Compiling

Tested in macOS, should work in all operative systems.

Just run `make` and a binary called `auto-header` should be created.

## Disclaimer

There may be bugs and files may be corrupted. I cannot guarantee anything.
