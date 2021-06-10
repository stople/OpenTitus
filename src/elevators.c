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

/* elevators.c
 * Handles elevators.
 *
 * Global functions:
 * int MOVE_TRP(TITUS_level *level): Move elevators, is called by main game loop
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "level.h"
#include "globals.h"
#include "definitions.h"
#include "elevators.h"

void MTSBR(TITUS_elevator *elevator) {
    elevator->counter++;
    if (elevator->counter >= elevator->range) {
        elevator->counter = 0;
        elevator->sprite.speedX = 0 - elevator->sprite.speedX;
        elevator->sprite.speedY = 0 - elevator->sprite.speedY;
    }
}

void MOVE_TRP(TITUS_level *level) {
    TITUS_elevator *elevator = level->elevator;
    uint8 i;
    for (i = 0; i < level->elevatorcount; i++) {
        //Find elevators on the screen
        if (elevator[i].enabled == false) {
            continue;
        }
        elevator[i].sprite.x += elevator[i].sprite.speedX;
        elevator[i].sprite.y += elevator[i].sprite.speedY;
        MTSBR(&(elevator[i]));
        if (((elevator[i].sprite.x + 16 - (BITMAP_X << 4)) >= 0) && // +16: closer to center
          ((elevator[i].sprite.x - 16 - (BITMAP_X << 4)) <= screen_width * 16) && // -16: closer to center
          ((elevator[i].sprite.y - (BITMAP_Y << 4)) >= 0) &&
          ((elevator[i].sprite.y - (BITMAP_Y << 4)) - 16 <= screen_height * 16)) {
            elevator[i].sprite.invisible = false;
        } else {
            elevator[i].sprite.invisible = true; //Not necessary, but to mimic the original game
        }
    }
}

