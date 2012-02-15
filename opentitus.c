/*   
 * Copyright (C) 2008 - 2011 The OpenTitus team
 *
 * Authors:
 * Eirik Stople
 *
 * "Titus the Fox: To Marrakech and Back" (1992) and
 * "Lagaf': Les Aventures de Moktar - Vol 1: La Zoubida" (1991)
 * was developed by, and is probably copyrighted by Titus Software,
 * which, according to Wikipedia, stopped buisness in 2005.
 *
 * OpenTitus is not affiliated with Titus Software.
 *
 * OpenTitus is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 3  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

/* opentitus.c
 * Main source file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
//#include "opentitus.h"

//Probably not the best way, but it works...
#define HAVE_CONFIG_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef AUDIO_ENABLED
#include "audio.h"
#endif

/*
#ifdef AUDIO_MIKMOD_SINGLETHREAD
#include <mikmod.h>
#endif

#ifdef AUDIO_SDL_MIXER
#include "SDL/SDL_mixer.h"
#endif
*/

#include "tituserror.h"
#include "sqz.h"
#include "settings.h"
#include "malloc.h"
#include "sprites.h"
#include "backbuffer.h"
#include "viewimage.h"
#include "fonts.h"
#include "menu.h"
#include "engine.h"
#include "original.h"
#include "objects.h"

int main(int argc, char *argv[]) {

    int retval;
    int state = 1; //View the menu when the main loop starts
    retval = init();
    if (retval < 0)
        state = 0;

    if (state) {
        retval = viewintrotext();
        if (retval < 0)
            state = 0;
    }

    if (state) {
        retval = viewimage(tituslogofile, tituslogoformat, 0, 4000);
        if (retval < 0)
            state = 0;
    }

#ifdef AUDIO_ENABLED
    SELECT_MUSIC(15);
#endif

    if (state) {
        retval = viewimage(titusintrofile, titusintroformat, 0, 6500);
        if (retval < 0)
            state = 0;
    }

    while (state) {
        retval = viewmenu(titusmenufile, titusmenuformat);

        if (retval <= 0)
            state = 0;

        if (state && (retval <= levelcount)) {
            retval = playtitus(retval - 1);
            if (retval < 0)
                state = 0;
        }
    }
    
    freefonts();

#ifdef AUDIO_ENABLED
    freeaudio();
#endif

    SDL_Quit();

    checkerror();

    if (retval == -1)
        retval = 0;

    return retval;
}

int init() {

    int retval;

    retval = readconfig(OPENTITUS_CONFIG_FILE);
    if (retval < 0)
        return retval;

#ifdef AUDIO_ENABLED
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return TITUS_ERROR_SDL_ERROR;
    }
#else
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return TITUS_ERROR_SDL_ERROR;
    }
#endif

#ifdef _DINGUX
    //fullscreen
    SDL_ShowCursor(SDL_DISABLE);
    screen = SDL_SetVideoMode(reswidth, resheight, bitdepth, SDL_SWSURFACE);
#else
    switch (videomode) {
    case 0: //window mode
        screen = SDL_SetVideoMode(reswidth, resheight, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
        SDL_WM_SetCaption(OPENTITUS_WINDOW_TEXT, 0);
        break;
    case 1: //fullscreen
        SDL_ShowCursor(SDL_DISABLE);
        screen = SDL_SetVideoMode(reswidth, resheight, bitdepth, SDL_DOUBLEBUF | SDL_FULLSCREEN);
        break;
    }
#endif

    if (screen == NULL) {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        return TITUS_ERROR_SDL_ERROR;
    }

    SDL_EnableUNICODE (1);

/*
#ifdef AUDIO_MIKMOD_SINGLETHREAD
    MikMod_RegisterAllDrivers();
    MikMod_RegisterAllLoaders();
    md_mode = AUDIO_MIKMOD_MODE;
    if (MikMod_Init("")) {
        printf("Could not initialize sound, reason: %s\n", MikMod_strerror(MikMod_errno));
        return TITUS_ERROR_AUDIO_ERROR;
    }
#endif

#ifdef AUDIO_MIKMOD_MULTITHREAD
    retval = startmodulethread();
    if (retval < 0)
        return retval;
#endif

#ifdef AUDIO_SDL_MIXER
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        return TITUS_ERROR_AUDIO_ERROR;
    }
#endif
*/

	initaudio();

    initoriginal();

    initcodes();

    initleveltitles();

    loadfonts();

    return 0;

}

void checkerror(void) {
    printf("%s\n", lasterror);
}

