# OpenTitus

![Game title screen](/docs/title.jpg)

A port of the game engine behind the DOS versions of Titus the Fox and Moktar.

OpenTitus is released under the Gnu GPL version 3 or (at your option) any later version of the license, and it is released "as is"; without any warranty.

This is a fork that aims to modernize the codebase, clean everything up, fix bugs, and make the game playable on modern systems with large 4K screens.

## The original OpenTitus team:
* Eirik Stople - Main developer - eirik@pcfood.net
* Eitan Tal - Valuable assistant, technical advisor

## Thanks to:
* Eric Zmiro and his team in Titus Interactive who made the great games Titus the Fox and Moktar!
* Jesse, who made a fansite for the original game and an editor and provided me information how the original game files was built up, and assisted me with the makefile and MinGW!
* The SDL project (LibSDL, LibSDL_mixer)!
* The GNU project (GCC, GDB and GPL)!
* MinGW/msys!
* Ubuntu (programming environment)!
* Linux!
* Dosbox!
* Dropbox!
* GEdit!
* Notepad++!
* Google search!
* Chocolate-Doom's great OPL (adlib) emulator!
* Various tutorials/information sources on the net!


## Building:
You need:
* Some sort of compiler
* Recent version of CMake
* SDL and SDL_mixer libraries available and usable for development

This should get things built and ready for testing/use:
```
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX="../bin/"
make install
```

## Running:
You need the original game files to make use of OpenTitus. OpenTitus parses the original files. It works with both Titus the Fox and Moktar.

* Build and install into the `bin` folder according to the build instructions.
* Place original game files in `bin/moktar` and/or `bin/titus` folders.
* Run `opentitus` inside `bin/titus` folder or `openmoktar` in `bin/moktar` folder.

Please do not upload the original game files to the git server, as they are proprietary!

If you can find some bugs or differences between OpenTitus and the original games, feel free to contact us!

Enjoy!
