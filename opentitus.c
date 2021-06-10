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
#include "SDL2/SDL.h"

#include "config.h"

#include "audio.h"

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

SDL_Window *window;
SDL_Renderer *renderer;

int init() {

    int retval;

    retval = readconfig(OPENTITUS_CONFIG_FILE);
    if (retval < 0)
        return retval;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return TITUS_ERROR_SDL_ERROR;
    }


    Uint32 windowflags = 0;
    int w;
    int h;
    switch (videomode) {
        default:
        case 0: //window mode
            w = 320;
            h = 200;
            windowflags = SDL_WINDOW_RESIZABLE;
            break;
        case 1: // fullscreen
            w = 0;
            h = 0;
            windowflags = SDL_WINDOW_FULLSCREEN_DESKTOP;
            break;
    }

    window = SDL_CreateWindow(
        OPENTITUS_WINDOW_TEXT,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        w,
        h,
        windowflags
    );
    /*
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        return TITUS_ERROR_SDL_ERROR;
    }
    */

    screen = SDL_GetWindowSurface(window);
    // screen = SDL_CreateRGBSurface(0, 320, 200, 8, 0, 0, 0, 0);

    /*
    SDL_RenderSetLogicalSize(renderer, 320, 200);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    */

    initaudio();

    initoriginal();

    initcodes();

    initleveltitles();

    loadfonts();

    return 0;

}

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

    SELECT_MUSIC(15);

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

    freeaudio();

    SDL_Quit();

    checkerror();

    if (retval == -1)
        retval = 0;

    return retval;
}

void checkerror(void) {
    printf("%s\n", lasterror);
}

