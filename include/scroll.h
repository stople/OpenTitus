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

/* scroll.h
 * Scroll functions
 */

#ifndef SCROLL_H
#define SCROLL_H

#include "SDL/SDL.h"

extern bool PERMUT_FLAG; //If false, there are no animated tiles on the screen?
extern uint8 loop_cycle; //Increased every loop in game loop
extern uint8 tile_anim; //Current tile animation (0-1-2), changed every 4th game loop cycle
extern uint8 BITMAP_X; //Screen offset (X) in tiles
extern uint8 BITMAP_XM; //Point to the left tile in the tile screen (0 to 19)
extern uint8 BITMAP_Y; //Screen offset (Y) in tiles
extern uint8 BITMAP_YM; //Point to the top tile in the tile screen (0 to 11)
extern bool XSCROLL_CENTER; //If true, the screen will scroll in X
extern int16 XLIMIT_SCROLL; //If scrolling: scroll until player is in this tile (X)
extern bool YSCROLL_CENTER; //If true, the screen will scroll in Y
extern uint8 YLIMIT_SCROLL; //If scrolling: scroll until player is in this tile (Y)

int scroll(TITUS_level *level);
int DISPLAY_CHAR(TITUS_level *level, uint8 tile, uint8 y, uint8 x);
bool L_SCROLL(TITUS_level *level);
bool R_SCROLL(TITUS_level *level);
bool U_SCROLL(TITUS_level *level);
bool D_SCROLL(TITUS_level *level);

#endif

