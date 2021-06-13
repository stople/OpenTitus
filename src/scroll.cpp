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

/* scroll.c
 * Scroll functions
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "level.h"
#include "globals.h"
#include "definitions.h"
#include "window.h"
#include "scroll.h"

static uint8 BARRYCENTRE(TITUS_level *level) {
    //If an enemy is behind the player, max. 12.5 tiles away horizontally, scroll until player is in the middle
    //If not, scroll until player is in the 3rd screen tile
    int16 enemy_left, i;
    for (i = 0; i < level->enemycount; i++) {
        if (!level->enemy[i].sprite.enabled || !level->enemy[i].visible) {
            continue;
        }
        enemy_left = (level->enemy[i].sprite.x < level->player.sprite.x); //True if enemy is left for the player
        if ((enemy_left != level->player.sprite.flipped) && //Enemy is behind the player
          (abs(level->enemy[i].sprite.x - level->player.sprite.x) < 200)) { //Enemy is max. 12.5 tiles away
            return (screen_width / 2);
        }
    }
    if (!level->player.sprite.flipped) {
        return 3;
    } else {
        return (screen_width - 3);
    }
}

static void X_ADJUST(TITUS_level *level) {
    bool block;
    TITUS_player *player = &(level->player);
    int16 pstileX = (player->sprite.x >> 4) - BITMAP_X; //Player screen tile X (0 to 19)
    if (!XSCROLL_CENTER) { //if not scrolling, check if it should scroll
        //If scroll timer isn't activated, scroll if the player is close to the screen edge
        if (ACTION_TIMER < LIMIT_TIMER) { //LIMIT_TIMER = 22
            //Scroll timer is not reached yet
            if ((!XSCROLL_CENTER) &&
              ((pstileX > screen_width - ALERT_X / 16) || //Player is on the right screen edge
              (pstileX < ALERT_X/16))) { //Player is on the left screen edge
                XLIMIT_SCROLL = screen_width / 2; //Scroll until player is in center
                XSCROLL_CENTER = true; //start scrolling
            }
        } else if ((LAST_ORDER & 0x0F) == 1) { //Walking, scroll timer is activated
            //Is the player at max speed?
            if (abs(player->sprite.speedX) != MAX_X*16) {
                //No! (walking against a wall?)
                if (!player->sprite.flipped) { //Player is in right direction
                    XLIMIT_SCROLL = 2; //Scroll until player is in 2nd screen tile
                    XSCROLL_CENTER = true; //start scrolling
                } else { //Player is in left direction
                    XLIMIT_SCROLL = screen_width - 2; //Scroll until player is in 18th screen tile
                    XSCROLL_CENTER = true; //start scrolling
                }
            } else { //Walking at full speed
                if (player->sprite.speedX < 0) { //Walking left
                    if (pstileX <= screen_width / 2 - 3) { //Center tile - 3
                        XLIMIT_SCROLL = screen_width - 2; //Scroll until player is in 18th screen tile
                        XSCROLL_CENTER = true; //start scrolling
                    }
                } else if (pstileX >= screen_width / 2 + 3) { //Center tile + 3
                    XLIMIT_SCROLL = 2; //Scroll until player is in 2nd screen tile
                    XSCROLL_CENTER = true; //start scrolling
                }
            }
        } else { //Not walking
            if ((player->sprite.speedX == 0) &&
              (player->sprite.speedY == 0) &&
              (ACTION_TIMER == (LIMIT_TIMER * 3 / 2))) { //X * 2 seconds?
                XLIMIT_SCROLL = BARRYCENTRE(level); //Center if visible enemy behind, if not, scroll until 3rd tile 
                XSCROLL_CENTER = true;
            } else {
                //Scroll until player is in center
                if ((!XSCROLL_CENTER) &&
                  ((pstileX > screen_width - ALERT_X / 16) ||
                  (pstileX < ALERT_X / 16))) {
                    XLIMIT_SCROLL = screen_width / 2; //Place the player in the center
                    XSCROLL_CENTER = true;
                }
            }
        }
    }

    if (XSCROLL_CENTER) {
        if (XLIMIT_SCROLL == pstileX) {
            XSCROLL_CENTER = false; //Stop scrolling
        } else if (XLIMIT_SCROLL > pstileX) {
            block = L_SCROLL(level);
            if (block) { //Carry flag
                XSCROLL_CENTER = false; //Stop scrolling
            }
        } else {
            block = R_SCROLL(level);
            if (block) { //Carry flag
                XSCROLL_CENTER = false; //Stop scrolling
            }
        }
    }
}

static void Y_ADJUST(TITUS_level *level) {
    TITUS_player *player = &(level->player);
    if (player->sprite.speedY == 0) {
        YSCROLL_CENTER = false;
    }
    int16 pstileY = (player->sprite.y >> 4) - BITMAP_Y; //Player screen tile Y (0 to 11)
    if (!YSCROLL_CENTER) {
        if ((player->sprite.speedY == 0) &&
          (LADDER_FLAG == 0)) {
            if (pstileY >= screen_height - 1) {
                YLIMIT_SCROLL = screen_height - 2;
                YSCROLL_CENTER = true;
            } else if (pstileY <= 2) {
                YLIMIT_SCROLL = screen_height - 3;
                YSCROLL_CENTER = true;
            }
        } else {
            if (pstileY >= screen_height - 2) { //The player is at the bottom of the screen, scroll down!
                YLIMIT_SCROLL = 3;
                YSCROLL_CENTER = true;
            } else if (pstileY <= 2) { //The player is at the top of the screen, scroll up!
                YLIMIT_SCROLL = screen_height - 3;
                YSCROLL_CENTER = true;
            }
        }
    }

    if ((player->sprite.y <= ((ALTITUDE_ZERO + screen_height) << 4)) && //If the player is above the horizontal limit
      (BITMAP_Y > ALTITUDE_ZERO + 1)) { //... and the screen have scrolled below the the horizontal limit
        if (U_SCROLL(level)) { //Scroll up
            YSCROLL_CENTER = false;
        }
    } else if ((BITMAP_Y > ALTITUDE_ZERO - 5) && //If the screen is less than 5 tiles above the horizontal limit
      (BITMAP_Y <= ALTITUDE_ZERO) && //... and still above the horizontal limit
      (player->sprite.y + (7 * 16) > ((ALTITUDE_ZERO + screen_height) << 4))) {
        if (D_SCROLL(level)) { //Scroll down
            YSCROLL_CENTER = false;
        }
    } else if (YSCROLL_CENTER) {
        if (YLIMIT_SCROLL == pstileY) {
            YSCROLL_CENTER = false;
        } else if (YLIMIT_SCROLL > pstileY) {
            if (U_SCROLL(level)) {
                YSCROLL_CENTER = false;
            }
        } else if ((player->sprite.y <= ((ALTITUDE_ZERO + screen_height) << 4)) && //If the player is above the horizontal limit
          (BITMAP_Y > ALTITUDE_ZERO)) { //... and the screen is below the horizontal limit
            YSCROLL_CENTER = false; //Stop scrolling
        } else {
            if (D_SCROLL(level)) { //Scroll down
                YSCROLL_CENTER = false;
            }
        }
    }
}

void scroll(TITUS_level *level) {
    //Scroll screen and update tile animation
    loop_cycle++; //Cycle from 0 to 3
    if (loop_cycle > 3) {
        loop_cycle = 0;
    }
    if (loop_cycle == 0) { //Every 4th call
        tile_anim++; //Cycle tile animation (0-1-2)
        if (tile_anim > 2) {
            tile_anim = 0;
        }
    }
    //Scroll
    if (!NOSCROLL_FLAG) {
        X_ADJUST(level);
        Y_ADJUST(level);
    }
}

bool L_SCROLL(TITUS_level *level) {
    //Scroll left
    if (BITMAP_X == 0) {
        return true; //Stop scrolling
    }
    BITMAP_X--; //Scroll 1 tile left
    return false; //Continue scrolling
}


bool R_SCROLL(TITUS_level *level) {
    //Scroll right
    uint8 maxX;
    if (((level->player.sprite.x >> 4) - screen_width) > XLIMIT) { //Scroll limit
        maxX = level->width - screen_width; //256 - 20
    } else {
        maxX = XLIMIT;
    }
    if (BITMAP_X >= maxX) {
        return true; //Stop scrolling
    }
    BITMAP_X++; //Increase pointer
    return false;
}


bool U_SCROLL(TITUS_level *level) {
    //Scroll up
    if (BITMAP_Y == 0) {
        return true;
    }
    BITMAP_Y--; //Scroll 1 tile up
    return false;
}


bool D_SCROLL(TITUS_level *level) {
    //Scroll down
    if (BITMAP_Y >= (level->height - screen_height)) { //The screen is already at the bottom
        return true; //Stop scrolling
    }
    BITMAP_Y++; //Increase pointer
    return false;
}
