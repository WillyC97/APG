APG - Automated Playlist Generation Player
=========================
yes the name is bad

Build requirements
----
* python3
* The essentia dependencies: [Linux](https://link-url-here.org) and [macOS](https://essentia.upf.edu/installing.html#installing-dependencies-on-macos).
 * <i> Other install methods can also be found on [this page](https://essentia.upf.edu/installing.html#installing-dependencies-on-macos) </i>.
* Xcode command line tools
* CMake

This project features submodules: essentia and JUCE, so after cloning, a `git submodule update --init` is required.

3rd Party Library Installation
---
TODO: Automate

Currently, to build the project it is a requirement to build the 3rd party dependencies manually.

This can be done by entering the `external/essentia` folder and running the following:

`python3 waf configure --build-static`

followed by

`python3 waf`

This <i>should</i> build the essentia static library. The likelihood of encountering issues here is higher on macOS than Linux.

Next, essentia's dependencies should be compiled. this is completed by entering the `/packages` folder within essentia and running `./build_3rdparty_static_debian.sh`.

If anything fails or a higher level of customisation is required, the individual scripts for installing the libs can be found in `/packaging/debian_3rdparty`

Usage
----

To generate the project files, use the `build.sh` script in the root of the directory.

Arguments can be seen by running `build.sh --help`

The project files will be placed in a folder named `projects`.

On macOS this will generate an Xcode project and on Linux will generate a make file.
