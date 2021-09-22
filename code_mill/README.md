# Code Mill

Code Mill is a plug-in for Code Crusader.  Code Mill is invoked by selecting one or more classes in the C++ class tree and then choosing the "Create derived class" item on the Tree menu.  The [on-line help](http://nps-codemill.sourceforge.net/help.html) explains all the features of the program.

### Building from source

1. Install the [JX Application Framework](https://github.com/jafl/jx_application_framework),  and set the `JX_ROOT` environment variable to point to its `include` directory.
1. `makemake`
1. `make`


## Installing a binary

http://sourceforge.net/p/nps-codemill/

The easiest way to install from the binary package is to run `sudo ./install`

Otherwise, the program (code_mill) should be placed in a directory that is on your execution path.  `/usr/bin` or `~/bin` are usually good choices.  Copy the libraries in `lib/` to a directory on your `LD_LIBRARY_PATH`.

## Requirements

On MacOS, this program requires XQuartz, the X11 server.  Before running this program, you must first start XQuartz.

You also need to `brew install ace icu4c pcre gd`.


## FAQ

For general questions, refer to the [Programs FAQ](https://github.com/jafl/jx_application_framework/blob/master/APPS.md).
