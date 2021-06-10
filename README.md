# Readme for OpenTitus
Git version
http://opentitus.sourceforge.net/

OpenTitus is released under the Gnu GPL version 3 or (at your option) any later version of the license, and it is released "as is"; without any warranty.

## The OpenTitus team:
Eirik Stople - Main developer - eirik@pcfood.net
Eitan Tal - Valuable assistant, technical advisor

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


## Git instructions:
Compile according to COMPILING.
To test out your build, copy it into the game folder and follow the instructions inside bin/README.
Precompiled binaries are available in builds/

You need the original game files to make use of OpenTitus. OpenTitus parses the original files. It works with both Titus the Fox and Moktar.
Place the original game files in the "titus" or "moktar" folder. The files must be placed according to titus.conf.

## Runtime dependencies:
SDL runtime library
SDL_mixer runtime library

To run the game, run opentitus (in Linux) or opentitus.exe (windows).

To quickly start up Titus, copy a Titus game directory to the same folder as the opentitus executable, and name the folder "titus".
To quickly start up Moktar, copy a Moktar game directory to the same folder as the opentitus executable, and name the folder "moktar". You can modify titus.conf if you like, or use the preconfigured "titus_moktar.conf" which you need to rename "titus.conf".

The game engine are ported from the original game, and modified to fit OpenTitus' level structure. Movement are equal to the original game.

If you can find some bugs or differences between OpenTitus and the original games, feel free to contact us!

Enjoy!
