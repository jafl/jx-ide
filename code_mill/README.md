# Code Mill

Code Mill is a plug-in for Code Crusader.  Code Mill is invoked by selecting one or more classes in the C++ class tree and then choosing the "Create derived class" item on the Tree menu.  The [on-line help](http://nps-codemill.sourceforge.net/help.html) explains all the features of the program.


## Building from source

1. Install the [JX Application Framework](https://github.com/jafl/jx_application_framework),  and set the `JX_ROOT` environment variable to point to its `include` directory.
1. `makemake`
1. `make`


## Installing a binary

For macOS, the easiest way to install is via [Homebrew](https://brew.sh):

* `brew install --cask xquartz`
* `brew tap jafl/jx`
* `brew install jafl/jx/code_mill`

For all other systems, download a package from:

* https://github.com/jafl/jx-ide/releases
* http://sourceforge.net/p/nps-codemill/

If you download the tar, unpack it, cd into the directory, and run `sudo ./install`.

Otherwise, put the program (`code_mill`) in a directory that is on your execution path.  `~/bin` is typically good choices.

## Requirements

On MacOS, this program requires XQuartz, the X11 server.  Before running this program, you must first start XQuartz.


## FAQ

For general questions, refer to the [Programs FAQ](https://github.com/jafl/jx_application_framework/blob/master/APPS.md).
