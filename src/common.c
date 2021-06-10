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

/* common.c
 * Common functions
 *
 * Global functions:
 * int subto0(uint8 *number): Decreases a number until it reaches 0
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "definitions.h"
#include "common.h"
#include "audio.h"

int subto0(uint8 *number) {
    //Decreases a value with 1 until it's zero
    if (*number > 0) {
        *number = *number - 1;
    }
}

int titus_sleep() {
    //refreshaudio();
    SDL_Delay(1);
}
