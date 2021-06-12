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

/* tile_animation.c
 * Animate tiles
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "level.h"
#include "globals.h"
#include "definitions.h"
#include "scroll.h"
#include "tile_animation.h"

void BLOC_ANIMATION(TITUS_level *level) {
    //Draw animated sprites on OFS_SCREENM
    uint8 tmp_ym, curY, tmp_xm, curX;
    uint8 i, j;

    if ((PERMUT_FLAG) && (loop_cycle == 0)) {
        PERMUT_FLAG = false;
        tmp_ym = BITMAP_YM; //0-11
        curY = BITMAP_Y;
        //20 tiles width, 12 tiles height
        for (i = 0; i < screen_height; i++) {
            tmp_xm = BITMAP_XM; //0-19
            curX = BITMAP_X;
            for (j = 0; j < screen_width; j++) {
                if (level->tile[level->tilemap[curY][curX]].animated) { //Animated tile
                    PERMUT_FLAG = true;
                    DISPLAY_CHAR(level, level->tilemap[curY][curX], tmp_ym, tmp_xm); //Draw tile
                }
                tmp_xm++;
                if (tmp_xm >= screen_width) {
                    tmp_xm = 0;
                }
                curX++;
            }
            tmp_ym++;
            if (tmp_ym >= screen_height) {
                tmp_ym = 0;
            }
            curY++;
        }
    }
}


