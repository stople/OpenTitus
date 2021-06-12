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

/* keyboard.c
 * Keyboard functions
 */

#include "SDL2/SDL.h"
#include "keyboard.h"
#include "globals.h"
#include "common.h"

#include "audio.h"
#include "backbuffer.h"

int waitforbutton() {
    SDL_Event event;
    int waiting = 1;
    while (waiting > 0)
    {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                waiting = -1;

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == KEY_RETURN || event.key.keysym.scancode == KEY_ENTER || event.key.keysym.scancode == KEY_SPACE)
                    waiting = 0;

                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    waiting = -1;

                if (event.key.keysym.scancode == KEY_MUSIC) {
                    AUDIOMODE++;
                    if (AUDIOMODE > 1) {
                        AUDIOMODE = 0;
                    }
                    if (AUDIOMODE == 1) {
                        startmusic();
                    }
                }
            }
            if (event.type == SDL_WINDOWEVENT) {
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                    case SDL_WINDOWEVENT_MAXIMIZED:
                    case SDL_WINDOWEVENT_RESTORED:
                        SDL_Flip(screen);
                    default:
                        break;
                }
            }
        }
        titus_sleep();

    }
    return (waiting);
}

