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

/* draw.h
 * Draw functions
 *
 * Global functions:
 * int TFR_SCREENM(): Draw tiles on the backbuffer (copy from the tile screen)
 * int viewstatus(TITUS_level *level, bool countbonus): View status screen (F4)
 * int flip_screen(bool slow): Flips the screen and a short delay
 * int INIT_SCREENM(TITUS_level *level): Initialize backbuffer
 * int DISPLAY_COUNT(TITUS_level *level): Draw energy
 * int fadeout(): Fade the screen to black
 * int view_password(TITUS_level *level, uint8 level_index): Display the password
 */

#ifndef DRAW_H
#define DRAW_H

#include "SDL2/SDL.h"
#include "level.h"
#include "definitions.h"

int TFR_SCREENM(); //Draw tiles on the backbuffer (copy from the tile screen)
int viewstatus(TITUS_level *level, bool countbonus);
int flip_screen(bool slow);
int INIT_SCREENM(TITUS_level *level);
int DISPLAY_COUNT(TITUS_level *level);
int fadeout();
int view_password(TITUS_level *level, uint8 level_index);
int loadpixelformat(SDL_PixelFormat **pixelformat);
int loadpixelformat_font(SDL_PixelFormat **pixelformat);
int freepixelformat(SDL_PixelFormat **pixelformat);

#endif

