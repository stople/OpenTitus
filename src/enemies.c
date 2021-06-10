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

/* enemies.c
 * Handles enemies.
 *
 * Global functions:
 * int MOVE_NMI(TITUS_level *level): Move enemies, is called by main game loop
 * int SET_NMI(TITUS_level *level): Collision detection, animation, is called by main game loop
 * int MOVE_TRASH(TITUS_level *level): Move objects thrown by enemies
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "level.h"
#include "globals.h"
#include "definitions.h"
#include "enemies.h"
#include "common.h"
#include "player.h"
#include "settings.h"

int updateenemysprite(TITUS_level *level, TITUS_enemy *enemy, int16 number, bool clearflags);
bool NMI_VS_DROP(TITUS_sprite *enemysprite, TITUS_sprite *sprite);
int KICK_ASH(TITUS_level *level, TITUS_sprite *enemysprite, int16 power);
bool FIND_TRASH(TITUS_level *level, TITUS_sprite **trash);

int MOVE_NMI(TITUS_level *level) {
    TITUS_sprite *bullet;
    int i, j;
    uint8 hflag;
    for (i = 0; i < level->enemycount; i++) {

        if (!(level->enemy[i].sprite.enabled)) continue; //Skip unused enemies

        switch (level->enemy[i].type) {
        case 0:
        case 1:
            //Noclip walk
            if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX; //Move the enemy
            if (abs(level->enemy[i].sprite.x - level->enemy[i].centerX) > level->enemy[i].rangeX) { //If the enemy is rangeX from center, turn direction
                if (level->enemy[i].sprite.x >= level->enemy[i].centerX) { //The enemy is at rightmost edge
                    level->enemy[i].sprite.speedX = abs(level->enemy[i].sprite.speedX);
                } else { //The enemy is at leftmost edge
                    level->enemy[i].sprite.speedX = 0 - abs(level->enemy[i].sprite.speedX);
                }
            }
            break;

        case 2:
            //Shoot
            if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            if (!level->enemy[i].visible) { //Skip if not on screen
                continue;
            }
            //Give directions!
            if (level->enemy[i].direction == 0) { //Both ways
                level->enemy[i].sprite.speedX = 0;
                if (level->enemy[i].sprite.x < level->player.sprite.x) {
                    level->enemy[i].sprite.speedX = -1;
                }
            } else if (level->enemy[i].direction == 2) { //Right only
                level->enemy[i].sprite.speedX = -1; //Flip the sprite
            } else { //Left only
                level->enemy[i].sprite.speedX = 0; //Not flipped (facing left)
            }
            switch (level->enemy[i].phase) { //State dependent actions
            case 0:
                //Scans the horizon!
                subto0(&(level->enemy[i].counter));
                if (level->enemy[i].counter != 0) { //Decrease delay timer
                    continue;
                }
                if (abs(level->player.sprite.y - level->enemy[i].sprite.y) > 24) {
                    continue;
                }
                if (level->enemy[i].rangeX < abs(level->player.sprite.x - level->enemy[i].sprite.x)) { //if too far apart
                    continue;
                }
                if (level->enemy[i].direction != 0) {
                    if (level->enemy[i].direction == 2) { //Right only
                        if (level->enemy[i].sprite.x > level->player.sprite.x) { //Skip shooting if player is in opposite direction
                            continue;
                        }
                    } else {
                        if (level->player.sprite.x > level->enemy[i].sprite.x) { //Skip shooting if player is in opposite direction
                            continue;
                        }
                    }
                }
                level->enemy[i].phase = 30; //change state
                UP_ANIMATION(&(level->enemy[i].sprite));
                break;
            default:
                level->enemy[i].phase--;
                if (!level->enemy[i].trigger) {
                    continue;
                }
                level->enemy[i].sprite.animation += 2;
                if (FIND_TRASH(level, &(bullet))) {
                    PUT_BULLET(level, &(level->enemy[i]), bullet);
                    //level->enemy[i].counter = NMI_FREQ; //set delay timer
                    level->enemy[i].counter = level->enemy[i].delay; //set delay timer
                }
                level->enemy[i].phase = 0;
                break;
            }
            break;

        case 3:
        case 4:
            //Noclip walk, jump to player (fish)
            if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            switch (level->enemy[i].phase) { //State dependent actions
            case 0:
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX; //Move the enemy
                if (abs(level->enemy[i].sprite.x - level->enemy[i].centerX) > level->enemy[i].rangeX) { //If the enemy is rangeX from center, turn direction
                    if (level->enemy[i].sprite.x >= level->enemy[i].centerX) { //The enemy is at rightmost edge
                        level->enemy[i].sprite.speedX = abs(level->enemy[i].sprite.speedX);
                    } else { //The enemy is at leftmost edge
                        level->enemy[i].sprite.speedX = 0 - abs(level->enemy[i].sprite.speedX);
                    }
                }
                if (!level->enemy[i].visible) { //Is the enemy on the screen?
                    continue;
                }
                if ((level->enemy[i].sprite.y < level->player.sprite.y) || (level->enemy[i].sprite.y >= (level->player.sprite.y + 256))) { //Skip if player is below or >= 256 pixels above
                    continue;
                }
                if (level->enemy[i].rangeY < (level->enemy[i].sprite.y - level->player.sprite.y)) { //Skip if player is above jump limit
                    continue;
                }
                //see if the hero is in the direction of movement of fish
                if (level->enemy[i].sprite.x > level->player.sprite.x) { //The enemy is right for the player
                    if (level->enemy[i].sprite.flipped == true) { //The enemy looks right, skip
                        continue;
                    }
                } else { //The enemy is left for the player
                    if (level->enemy[i].sprite.flipped == false) { //The enemy looks left, skip
                        continue;
                    }
                }
                if (abs(level->enemy[i].sprite.x - level->player.sprite.x) >= 48) { //Fast calculation
                    continue;
                }
                //See if the hero is above the area of fish
                if (abs(level->player.sprite.x - level->enemy[i].centerX) > level->enemy[i].rangeX) {
                    continue;
                }
                level->enemy[i].phase = 1; //Change state
                //Calculation speed to the desired height
                level->enemy[i].sprite.speedY = 0;
                j = 0;
                do {
                    level->enemy[i].sprite.speedY++; //Set init jump speed
                    j += level->enemy[i].sprite.speedY;
                } while ((level->enemy[i].sprite.y - level->player.sprite.y) > j); //Make sure the enemy will jump high enough to hit the player
                level->enemy[i].sprite.speedY = 0 - level->enemy[i].sprite.speedY; //Init speed must be negative
                level->enemy[i].delay = level->enemy[i].sprite.y; //Delay: Last Y position, reuse of the delay variable
                UP_ANIMATION(&(level->enemy[i].sprite));
                break;
            case 1:
                if (!level->enemy[i].visible) { //Is the enemy on the screen?
                    continue;
                }
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX << 2;
                level->enemy[i].sprite.y += level->enemy[i].sprite.speedY;
                if (level->enemy[i].sprite.speedY + 1 < 0) {
                    level->enemy[i].sprite.speedY++;
                    if (level->enemy[i].sprite.y > (level->enemy[i].delay - level->enemy[i].rangeY)) { //Delay: Last Y position, reuse of the delay variable
                        continue;
                    }
                }
                UP_ANIMATION(&(level->enemy[i].sprite));
                level->enemy[i].phase = 2;
                level->enemy[i].sprite.speedY = 0;
                if (level->enemy[i].sprite.x <= level->enemy[i].centerX) {
                    level->enemy[i].sprite.speedX = abs(level->enemy[i].sprite.speedX);
                } else {
                    level->enemy[i].sprite.speedX = 0 - abs(level->enemy[i].sprite.speedX);
                }
                break;
            case 2:
                if (!level->enemy[i].visible) { //Is the enemy on the screen?
                    continue;
                }
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                level->enemy[i].sprite.y += level->enemy[i].sprite.speedY; //2: fall!
                level->enemy[i].sprite.speedY++;
                if (level->enemy[i].sprite.y < level->enemy[i].delay) { //3: we hit bottom? //Delay: Last Y position, reuse of the delay variable
                    continue;
                }
                level->enemy[i].sprite.y = level->enemy[i].delay; //Delay: Last Y position, reuse of the delay variable
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                level->enemy[i].phase = 0;
                DOWN_ANIMATION(&(level->enemy[i].sprite));
                DOWN_ANIMATION(&(level->enemy[i].sprite));
                break;
            }
            break;

        case 5:
        case 6:
            //Noclip walk, move to player (fly)
            if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX; //Move the enemy
            if (abs(level->enemy[i].sprite.x - level->enemy[i].centerX) > level->enemy[i].rangeX) { //If the enemy is rangeX from center, turn direction
                if (level->enemy[i].sprite.x >= level->enemy[i].centerX) { //The enemy is at rightmost edge
                    level->enemy[i].sprite.speedX = abs(level->enemy[i].sprite.speedX);
                } else { //The enemy is at leftmost edge
                    level->enemy[i].sprite.speedX = 0 - abs(level->enemy[i].sprite.speedX);
                }
            }
            if (!level->enemy[i].visible) { //Is the enemy on the screen?
                continue;
            }
            switch (level->enemy[i].phase) { //State dependent actions
            case 0:
                //Forward
                if (abs(level->enemy[i].sprite.y - level->player.sprite.y) > level->enemy[i].rangeY) { //Too far away
                    continue;
                }
                if (abs(level->enemy[i].sprite.x - level->player.sprite.x) > 40) { //Too far away
                    continue;
                }
                level->enemy[i].delay = level->enemy[i].sprite.y; //Delay: Last Y position, reuse of the delay variable
                if (level->enemy[i].sprite.y < level->player.sprite.y) { //Player is below the enemy
                    level->enemy[i].sprite.speedY = 2;
                } else { //Player is above the enemy
                    level->enemy[i].sprite.speedY = -2;
                }
                level->enemy[i].phase = 1; //Change state
                UP_ANIMATION(&(level->enemy[i].sprite));
                break;
            case 1:
                //Attack
                level->enemy[i].sprite.y += level->enemy[i].sprite.speedY;
                if (abs(level->enemy[i].sprite.y - level->enemy[i].delay) < level->enemy[i].rangeY) { //Delay: Last Y position, reuse of the delay variable
                    continue;
                }
                level->enemy[i].sprite.speedY = 0 - level->enemy[i].sprite.speedY;
                UP_ANIMATION(&(level->enemy[i].sprite));
                level->enemy[i].phase = 2;
                break;
            case 2:
                //Back up!
                level->enemy[i].sprite.y += level->enemy[i].sprite.speedY;
                if (level->enemy[i].sprite.y != level->enemy[i].delay) { //Delay: Last Y position, reuse of the delay variable
                    continue;
                }
                DOWN_ANIMATION(&(level->enemy[i].sprite));
                DOWN_ANIMATION(&(level->enemy[i].sprite));
                level->enemy[i].phase = 0;
                break;
            }
            break;

        case 7:
            //Gravity walk, hit when near
            if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            switch (level->enemy[i].phase) { //State dependent actions
            case 0:
                //Waiting
                if (level->enemy[i].sprite.y > level->player.sprite.y) {
                    continue;
                }
                if (level->enemy[i].rangeX < abs(level->enemy[i].sprite.x - level->player.sprite.x)) {
                    continue;
                }
                if (abs(level->enemy[i].sprite.y - level->player.sprite.y) > 200) {
                    continue;
                }
                level->enemy[i].phase = 1;
                UP_ANIMATION(&(level->enemy[i].sprite));
                if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                    level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                } else { //Enemy is left for the player
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                }
                break;
            case 1:
                //Gravity walk
                if (get_floorflag(level, (level->enemy[i].sprite.y >> 4), (level->enemy[i].sprite.x >> 4)) == FFLAG_NOFLOOR) {
                    if (level->enemy[i].sprite.speedY < 16) { //16 = Max yspeed
                        level->enemy[i].sprite.speedY++;
                    }
                    level->enemy[i].sprite.y += level->enemy[i].sprite.speedY;
                    continue;
                }
                if (level->enemy[i].sprite.speedY != 0) {
                    if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                        level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                    } else { //Enemy is left for the player
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                    }
                }
                level->enemy[i].sprite.speedY = 0;
                level->enemy[i].sprite.y = level->enemy[i].sprite.y & 0xFFF0;
                if (level->enemy[i].sprite.speedX > 0) {
                    j = -1; //moving left
                } else {
                    j = 1; //moving right
                }
                hflag = get_horizflag(level, (level->enemy[i].sprite.y >> 4) - 1, (level->enemy[i].sprite.x >> 4) + j);
                if ((hflag == HFLAG_WALL) ||
                  (hflag == HFLAG_DEADLY) ||
                  (hflag == HFLAG_PADLOCK)) { //Next tile is wall, change direction
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                }
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                if (level->enemy[i].sprite.x < 0) {
                    level->enemy[i].sprite.speedX =  0 - level->enemy[i].sprite.speedX;
                    level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                }
                if (abs(level->player.sprite.x - level->enemy[i].sprite.x) > 320 * 2) { //Too far away from the player in X, reset
                    level->enemy[i].phase = 2;
                    continue;
                }
                if (abs(level->player.sprite.y - level->enemy[i].sprite.y) >= 200 * 2) { //Too far away from the player in Y, reset
                    level->enemy[i].phase = 2;
                    continue;
                }
                if (abs(level->player.sprite.x - level->enemy[i].sprite.x) > level->enemy[i].sprite.spritedata->data->w + 6) {
                    continue;
                }
                if (abs(level->player.sprite.y - level->enemy[i].sprite.y) > 8) {
                    continue;
                }
                level->enemy[i].phase = 3; //The player is close to the enemy, strike!
                UP_ANIMATION(&(level->enemy[i].sprite));
                break;
            case 2:
                //Reset the enemy
                if (((level->enemy[i].initY >> 4) - BITMAP_Y <= 13) && //13 tiles in Y
                  ((level->enemy[i].initY >> 4) - BITMAP_Y >= 0) &&
                  ((level->enemy[i].initX >> 4) - BITMAP_X < 21) && //21 tiles in X
                  ((level->enemy[i].initX >> 4) - BITMAP_X >= 0)) {
                    continue; //Player is too close to the enemy's spawning point
                }
                level->enemy[i].sprite.y = level->enemy[i].initY;
                level->enemy[i].sprite.x = level->enemy[i].initX;
                level->enemy[i].phase = 0;
                DOWN_ANIMATION(&(level->enemy[i].sprite));
                break;
            case 3:
                //Strike!
                if (level->enemy[i].trigger) { //End of strike animation (TODO: check if this will ever be executed)
                    level->enemy[i].phase = 1;
                    continue;
                }
                //Gravity walk (equal to the first part of "case 1:")
                if (get_floorflag(level, (level->enemy[i].sprite.y >> 4), (level->enemy[i].sprite.x >> 4)) == FFLAG_NOFLOOR) {
                    if (level->enemy[i].sprite.speedY < 16) { //16 = Max yspeed
                        level->enemy[i].sprite.speedY++;
                    }
                    level->enemy[i].sprite.y += level->enemy[i].sprite.speedY;
                    continue;
                }
                if (level->enemy[i].sprite.speedY != 0) {
                    if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                        level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                    } else { //Enemy is left for the player
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                    }
                }
                level->enemy[i].sprite.speedY = 0;
                level->enemy[i].sprite.y = level->enemy[i].sprite.y & 0xFFF0;
                if (level->enemy[i].sprite.speedX > 0) {
                    j = -1; //moving left
                } else {
                    j = 1; //moving right
                }
                hflag = get_horizflag(level, (level->enemy[i].sprite.y >> 4) - 1, (level->enemy[i].sprite.x >> 4) + j);
                if ((hflag == HFLAG_WALL) ||
                  (hflag == HFLAG_DEADLY) ||
                  (hflag == HFLAG_PADLOCK)) { //Next tile is wall, change direction
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                }
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                if (level->enemy[i].sprite.x < 0) {
                    level->enemy[i].sprite.speedX =  0 - level->enemy[i].sprite.speedX;
                    level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                }
                if (abs(level->player.sprite.x - level->enemy[i].sprite.x) > 320 * 2) { //Too far away from the player in X, reset
                    level->enemy[i].phase = 2;
                    continue;
                }
                if (abs(level->player.sprite.y - level->enemy[i].sprite.y) >= 200 * 2) { //Too far away from the player in Y, reset
                    level->enemy[i].phase = 2;
                    continue;
                }
                break;
            }
            break;

        case 8: //Gravity walk when off-screen
        case 14: //Gravity walk when off-screen (immortal)
            if (level->enemy[i].type == 14) {
                level->enemy[i].dying = 0; //Immortal
            } else if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            switch (level->enemy[i].phase) { //State dependent actions
            case 0:
                //waiting
                if ((abs(level->enemy[i].sprite.x - level->player.sprite.x) > 340) ||
                  (abs(level->enemy[i].sprite.y - level->player.sprite.y) >= 230)) {
                    level->enemy[i].phase = 1;
                    UP_ANIMATION(&(level->enemy[i].sprite));
                    if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                        level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                    } else { //Enemy is left for the player
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                    }
                }
                break;
            case 1:
                if (get_floorflag(level, (level->enemy[i].sprite.y >> 4), (level->enemy[i].sprite.x >> 4)) == FFLAG_NOFLOOR) {
                    if (level->enemy[i].sprite.speedY < 16) { //16 = Max yspeed
                        level->enemy[i].sprite.speedY++;
                    }
                    level->enemy[i].sprite.y += level->enemy[i].sprite.speedY;
                    continue;
                }
                if (level->enemy[i].sprite.speedY != 0) {
                    if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                        level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                    } else { //Enemy is left for the player
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                    }
                }
                level->enemy[i].sprite.speedY = 0;
                level->enemy[i].sprite.y = level->enemy[i].sprite.y & 0xFFF0;
                if (level->enemy[i].sprite.speedX > 0) {
                    j = -1; //moving left
                } else {
                    j = 1; //moving right
                }
                hflag = get_horizflag(level, (level->enemy[i].sprite.y >> 4) - 1, (level->enemy[i].sprite.x >> 4) + j);
                if ((hflag == HFLAG_WALL) ||
                  (hflag == HFLAG_DEADLY) ||
                  (hflag == HFLAG_PADLOCK)) { //Next tile is wall, change direction
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                }
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                if (level->enemy[i].sprite.x < 0) {
                    level->enemy[i].sprite.speedX =  0 - level->enemy[i].sprite.speedX;
                    level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                }
                if (abs(level->player.sprite.x - level->enemy[i].sprite.x) < 320 * 2) {
                    continue;
                }
                level->enemy[i].phase = 2;
                break;
            case 2:
                //Reset the enemy
                if (((level->enemy[i].initY >> 4) - BITMAP_Y < 12) && //12 tiles in Y
                  ((level->enemy[i].initY >> 4) - BITMAP_Y >= 0) &&
                  ((level->enemy[i].initX >> 4) - BITMAP_X < 19) && //19 tiles in X
                  ((level->enemy[i].initX >> 4) - BITMAP_X >= 0)) {
                    continue; //Player is too close to the enemy's spawning point
                }
                level->enemy[i].sprite.y = level->enemy[i].initY;
                level->enemy[i].sprite.x = level->enemy[i].initX;
                level->enemy[i].phase = 0;
                DOWN_ANIMATION(&(level->enemy[i].sprite));
                break;
            }
            break;

        case 9:
            //Walk and periodically pop-up
            if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            switch (level->enemy[i].phase) { //State dependent actions
            case 0:
                //wait for its prey!
                if (level->enemy[i].rangeX < abs(level->player.sprite.x - level->enemy[i].sprite.x)) {
                    continue;
                }
                if (abs(level->player.sprite.y - level->enemy[i].sprite.y) > 60) {
                    continue;
                }
                level->enemy[i].phase = 1;
                UP_ANIMATION(&(level->enemy[i].sprite));
                if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                    level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                } else { //Enemy is left for the player
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                }
                break;
            case 1:
                //Special animation?
                TAUPE_FLAG++;
                if (((TAUPE_FLAG & 0x04) == 0) && //xxxxx0xx //true 4 times, false 4 times,
                  ((IMAGE_COUNTER & 0x01FF) == 0)) { //xxxxxxx0 00000000 //true 1 time, false 511 times
                    UP_ANIMATION(&(level->enemy[i].sprite));
                }
                if ((IMAGE_COUNTER & 0x007F) == 0) { //xxxxxxxx x0000000 //true 1 time, false 127 times
                    level->enemy[i].phase = 3;
                    UP_ANIMATION(&(level->enemy[i].sprite));
                    //Same as "case 3:"
                    //Remove the head or Periskop!
                    if (!level->enemy[i].visible) { //Is it on the screen?
                        //Give the sequence # 2 and Phase # 1
                        UP_ANIMATION(&(level->enemy[i].sprite));
                        level->enemy[i].sprite.animation--; //Previous animation frame
                        GAL_FORM(level, &(level->enemy[i]));
                        if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                            level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                        } else { //Enemy is left for the player
                            level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                        }
                        level->enemy[i].phase = 1;
                    } else if (level->enemy[i].trigger) {
                        if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                            level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                        } else { //Enemy is left for the player
                            level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                        }
                        level->enemy[i].phase = 1;
                    }
                    continue;
                }
                if (get_floorflag(level, (level->enemy[i].sprite.y >> 4), (level->enemy[i].sprite.x >> 4)) == FFLAG_NOFLOOR) {
                    level->enemy[i].sprite.speedX = abs(level->enemy[i].sprite.speedX);
                    if (level->enemy[i].initX > level->enemy[i].sprite.x) {
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                    }
                }
                level->enemy[i].sprite.y = level->enemy[i].sprite.y & 0xFFF0;
                if (level->enemy[i].sprite.speedX > 0) {
                    j = -1; //moving left
                } else {
                    j = 1; //moving right
                }
                hflag = get_horizflag(level, (level->enemy[i].sprite.y >> 4) - 1, (level->enemy[i].sprite.x >> 4) + j);
                if ((hflag == HFLAG_WALL) ||
                  (hflag == HFLAG_DEADLY) ||
                  (hflag == HFLAG_PADLOCK)) { //Next tile is wall, change direction
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                }
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                if (level->enemy[i].sprite.x < 0) {
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                    level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                }
                if (abs(level->player.sprite.x - level->enemy[i].sprite.x) < 320 * 4) {
                    continue;
                }
                level->enemy[i].phase = 2;
                break;
            case 2:
                //Reset, if not visible on the screen
                if (((level->enemy[i].initY >> 4) - BITMAP_Y <= 12) && //12 tiles in Y
                  ((level->enemy[i].initY >> 4) - BITMAP_Y >= 0) &&
                  ((level->enemy[i].initX >> 4) - BITMAP_X < 25) && //25 tiles in X
                  ((level->enemy[i].initX >> 4) - BITMAP_X >= 0)) {
                    continue; //Player is too close to the enemy's spawning point
                }
                level->enemy[i].sprite.y = level->enemy[i].initY;
                level->enemy[i].sprite.x = level->enemy[i].initX;
                level->enemy[i].phase = 0;
                DOWN_ANIMATION(&(level->enemy[i].sprite));
                break;
            case 3:
                //Remove the head or Periskop!
                if (!level->enemy[i].visible) { //Is it on the screen?
                    UP_ANIMATION(&(level->enemy[i].sprite));
                    level->enemy[i].sprite.animation--; //Previous animation frame
                    GAL_FORM(level, &(level->enemy[i]));
                    if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                        level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                    } else { //Enemy is left for the player
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                    }
                    level->enemy[i].phase = 1;
                } else if (level->enemy[i].trigger) {
                    if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                        level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                    } else { //Enemy is left for the player
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                    }
                    level->enemy[i].phase = 1;
                }
                break;
            }
            break;

        case 10:
            //Alert when near, walk when nearer
            if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            switch (level->enemy[i].phase) { //State dependent actions
            case 0:
                if (FURTIF_FLAG != 0) {
                    continue;
                }
                if (level->enemy[i].rangeX < abs(level->player.sprite.x - level->enemy[i].sprite.x)) {
                    continue;
                }
                if (abs(level->player.sprite.y - level->enemy[i].sprite.y) > 26) {
                    continue;
                }
                level->enemy[i].phase = 1;
                UP_ANIMATION(&(level->enemy[i].sprite));
                if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                    level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                } else { //Enemy is left for the player
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                }
            case 1:
                //wait
                if (FURTIF_FLAG != 0) {
                    continue;
                }
                if (level->enemy[i].rangeX < abs(level->player.sprite.x - level->enemy[i].sprite.x)) {
                    //Switch back to state 0
                    DOWN_ANIMATION(&(level->enemy[i].sprite));
                    level->enemy[i].phase = 0;
                    continue;
                }
                if ((level->enemy[i].rangeX - 50 >= abs(level->player.sprite.x - level->enemy[i].sprite.x)) &&
                  (abs(level->player.sprite.y - level->enemy[i].sprite.y) <= 60)) {
                    level->enemy[i].phase = 2;
                    UP_ANIMATION(&(level->enemy[i].sprite));
                }
                break;
            case 2:
                //run
                if (get_floorflag(level, (level->enemy[i].sprite.y >> 4), (level->enemy[i].sprite.x >> 4)) == FFLAG_NOFLOOR) {
                    level->enemy[i].sprite.speedX = abs(level->enemy[i].sprite.speedX);
                    if (level->enemy[i].initX > level->enemy[i].sprite.x) {
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                    }
                }
                level->enemy[i].sprite.y = level->enemy[i].sprite.y & 0xFFF0;
                if (level->enemy[i].sprite.speedX > 0) {
                    j = -1; //moving left
                } else {
                    j = 1; //moving right
                }
                hflag = get_horizflag(level, (level->enemy[i].sprite.y >> 4) - 1, (level->enemy[i].sprite.x >> 4) + j);
                if ((hflag == HFLAG_WALL) ||
                  (hflag == HFLAG_DEADLY) ||
                  (hflag == HFLAG_PADLOCK)) { //Next tile is wall, change direction
                    level->enemy[i].sprite.speedX = abs(level->enemy[i].sprite.speedX);
                    if (level->enemy[i].initX > level->enemy[i].sprite.x) {
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                    }
                }
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                if (level->enemy[i].sprite.x < 0) {
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                    level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                }
                if (abs(level->player.sprite.x - level->enemy[i].sprite.x) >= 320 * 2) {
                    level->enemy[i].phase = 3;
                }
                break;
            case 3:
                //Reset, if not visible on the screen
                if (((level->enemy[i].initY >> 4) - BITMAP_Y <= 13) && //13 tiles in Y
                  ((level->enemy[i].initY >> 4) - BITMAP_Y >= 0) &&
                  ((level->enemy[i].initX >> 4) - BITMAP_X < 21) && //21 tiles in X
                  ((level->enemy[i].initX >> 4) - BITMAP_X >= 0)) {
                    continue; //Spawning point is visible on screen
                }
                level->enemy[i].sprite.y = level->enemy[i].initY;
                level->enemy[i].sprite.x = level->enemy[i].initX;
                DOWN_ANIMATION(&(level->enemy[i].sprite));
                DOWN_ANIMATION(&(level->enemy[i].sprite));
                level->enemy[i].phase = 0;
                break;
            }
            break;

        case 11:
            //Walk and shoot
            if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            switch (level->enemy[i].phase) { //State dependent actions
            case 0:
                //wait
                if (level->enemy[i].rangeX < abs(level->player.sprite.x - level->enemy[i].sprite.x)) {
                    continue;
                }
                if (abs(level->player.sprite.y - level->enemy[i].sprite.y) > 26) {
                    continue;
                }
                level->enemy[i].phase = 1;
                UP_ANIMATION(&(level->enemy[i].sprite));
                if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                    level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                } else { //Enemy is left for the player
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                }
                break;
            case 1:
                if (get_floorflag(level, (level->enemy[i].sprite.y >> 4), (level->enemy[i].sprite.x >> 4)) == FFLAG_NOFLOOR) {
                    level->enemy[i].sprite.speedX = abs(level->enemy[i].sprite.speedX);
                    if (level->enemy[i].initX > level->enemy[i].sprite.x) {
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                    }
                }
                level->enemy[i].sprite.y = level->enemy[i].sprite.y & 0xFFF0;
                hflag = get_horizflag(level, (level->enemy[i].sprite.y >> 4) - 1, level->enemy[i].sprite.x >> 4);
                if ((hflag == HFLAG_WALL) ||
                  (hflag == HFLAG_DEADLY) ||
                  (hflag == HFLAG_PADLOCK)) { //Next tile is wall, change direction
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                }
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                if (level->enemy[i].sprite.x < 0) {
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                    level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                }
                if (abs(level->player.sprite.x - level->enemy[i].sprite.x) >= 320 * 2) {
                    level->enemy[i].phase = 2;
                }
                subto0(&(level->enemy[i].counter));
                if (level->enemy[i].counter != 0) {
                    continue;
                }
                if (abs(level->player.sprite.x - level->enemy[i].sprite.x) > 64) {
                    continue;
                }
                if (abs(level->player.sprite.y - level->enemy[i].sprite.y) > 20) {
                    continue;
                }
                if (level->enemy[i].sprite.x > level->player.sprite.x) { //Enemy is right for the player
                    level->enemy[i].sprite.speedX = level->enemy[i].walkspeedX; //Move left
                } else { //Enemy is left for the player
                    level->enemy[i].sprite.speedX = 0 - level->enemy[i].walkspeedX; //Move right
                }
                level->enemy[i].phase = 3; //phase started!
                UP_ANIMATION(&(level->enemy[i].sprite));
                level->enemy[i].counter = 20;
                break;
            case 2:
                //Reset enemy when spawn point isn't visible for the player
                if (((level->enemy[i].initY >> 4) - BITMAP_Y > 13) || //13 tiles in Y (Spawn is below)
                  ((level->enemy[i].initY >> 4) - BITMAP_Y < 0) || // (Spawn is above)
                  ((level->enemy[i].initX >> 4) - BITMAP_X >= 21) || //21 tiles in X (Spawn is to the right)
                  ((level->enemy[i].initX >> 4) - BITMAP_X < 0)) { //(Spawn is to the right)
                    level->enemy[i].sprite.x = level->enemy[i].initX;
                    level->enemy[i].sprite.y = level->enemy[i].initY;
                    DOWN_ANIMATION(&(level->enemy[i].sprite));
                    level->enemy[i].phase = 0;
                }
                break;
            case 3:
                //Shoot
                if (!level->enemy[i].trigger) {
                    continue;
                }
                if (FIND_TRASH(level, &(bullet))) {
                    level->enemy[i].sprite.animation += 2;
                    PUT_BULLET(level, &(level->enemy[i]), bullet);
                }
                DOWN_ANIMATION(&(level->enemy[i].sprite));
                level->enemy[i].phase = 1;
                break;
            }
            break;

        case 12:
            //Jump (fireball) (immortal)
            level->enemy[i].dying = 0; //Immortal
            switch (level->enemy[i].phase) { //State dependent actions
            case 0:
                //init
                UP_ANIMATION(&(level->enemy[i].sprite));
                level->enemy[i].sprite.speedY = level->enemy[i].rangeY;
                level->enemy[i].initY = level->enemy[i].sprite.y;
                level->enemy[i].phase = 1;
                break;
            case 1:
                //Fireball moving up
                level->enemy[i].sprite.y -= level->enemy[i].sprite.speedY;
                level->enemy[i].sprite.speedY--;
                if (level->enemy[i].sprite.speedY == 0) {
                    level->enemy[i].phase = 2;
                }
                break;
            case 2:
                //Fireball falling down
                level->enemy[i].sprite.y += level->enemy[i].sprite.speedY;
                level->enemy[i].sprite.speedY++;
                if (level->enemy[i].sprite.y >= level->enemy[i].initY) {
                    level->enemy[i].sprite.y = level->enemy[i].initY;
                    level->enemy[i].counter = level->enemy[i].delay;
                    level->enemy[i].phase = 3;
                    DOWN_ANIMATION(&(level->enemy[i].sprite));
                }
                break;
            case 3:
                //Fireball delay
                level->enemy[i].counter--;
                if (level->enemy[i].counter == 0) {
                    level->enemy[i].phase = 0;
                }
                break;
            }
            break;

        case 13:
            //Bounce (big baby)
            if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            switch (level->enemy[i].phase) { //State dependent actions
            case 0:
                //remain at rest or attack!
                if (level->player.sprite.x >= level->enemy[i].sprite.x) {
                    level->enemy[i].sprite.speedX = 0 - abs(level->enemy[i].sprite.speedX);
                } else {
                    level->enemy[i].sprite.speedX = abs(level->enemy[i].sprite.speedX);
                }
                if ((abs(level->player.sprite.x - level->enemy[i].sprite.x) <= level->enemy[i].rangeX) &&
                  (abs(level->player.sprite.y - level->enemy[i].sprite.y) <= 40)) {
                    UP_ANIMATION(&(level->enemy[i].sprite));
                    level->enemy[i].phase = 1;
                    level->enemy[i].sprite.speedY = 10;
                }
                break;
            case 1:
                //Jump, move upwards
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                level->enemy[i].sprite.y -= level->enemy[i].sprite.speedY;
                level->enemy[i].sprite.speedY--;
                if (level->enemy[i].sprite.speedY == 0) {
                    UP_ANIMATION(&(level->enemy[i].sprite));
                    level->enemy[i].phase = 2;
                }
                break;
            case 2:
                //Fall down to the ground
                level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                level->enemy[i].sprite.y += level->enemy[i].sprite.speedY;
                level->enemy[i].sprite.speedY++;
                if (level->enemy[i].sprite.speedY > 10) {
                    level->enemy[i].phase = 3;
                    UP_ANIMATION(&(level->enemy[i].sprite));
                    level->enemy[i].counter = level->enemy[i].delay;
                }
                break;
            case 3:
                //Stay on the ground for a while
                level->enemy[i].counter--;
                if (level->enemy[i].counter == 0) {
                    DOWN_ANIMATION(&(level->enemy[i].sprite));
                    DOWN_ANIMATION(&(level->enemy[i].sprite));
                    DOWN_ANIMATION(&(level->enemy[i].sprite));
                    level->enemy[i].phase = 0;
                }
                break;
            }
            break;

        //case 14:
        //Gravity walk when off-screen (immortal)
        //Located at case 8

        case 15:
            //Nothing (immortal)
            level->enemy[i].dying = 0; //Immortal
            break;

        case 16:
            //Nothing
            if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            break;

        case 17:
            //Drop (immortal)
            level->enemy[i].dying = 0; //Immortal
            //Delay
            if (level->enemy[i].counter + 1 < level->enemy[i].delay) {
                level->enemy[i].counter++;
                continue;
            }
            if (level->enemy[i].rangeX < abs(level->enemy[i].sprite.x - level->player.sprite.x)) { //hero too far! at x
                level->enemy[i].counter = 0;
                continue;
            }
            if (level->enemy[i].rangeY < (level->player.sprite.y - level->enemy[i].sprite.y)) { //hero too far! at y
                continue;
            }
            //you attack, so finding a free object
            j = 0;
            do {
                j++;
                if (j > level->objectcount) {
                    level->enemy[i].counter = 0;
                    continue;
                }
            } while (level->object[j].sprite.enabled == true);
            //object[j] is free!
            UP_ANIMATION(&(level->enemy[i].sprite));
            updateobjectsprite(level, &(level->object[j]), *level->enemy[i].sprite.animation & 0x1FFF, true);
            level->object[j].sprite.flipped = true;
            level->object[j].sprite.x = level->enemy[i].sprite.x;
            level->object[j].sprite.y = level->enemy[i].sprite.y;
            level->object[j].sprite.droptobottom = true;
            level->object[j].sprite.killing = true;
            level->object[j].sprite.speedY = 0;
            GRAVITY_FLAG = 4;
            DOWN_ANIMATION(&(level->enemy[i].sprite));
            level->enemy[i].counter = 0;
            break;

        case 18:
            //Guard (helicopter guy)
            if (level->enemy[i].dying != 0) { //If not 0, the enemy is dying or dead, and have special movement
                DEAD1(level, &(level->enemy[i]));
                continue;
            }
            if ((level->player.sprite.x < (int16)(level->enemy[i].initX - level->enemy[i].rangeX)) || //Player is too far left
              (level->player.sprite.x > (int16)(level->enemy[i].initX + level->enemy[i].rangeX)) || //Player is too far right
              (level->player.sprite.y < (int16)(level->enemy[i].initY - level->enemy[i].rangeY)) || //Player is too high above
              (level->player.sprite.y > (int16)(level->enemy[i].initY + level->enemy[i].rangeY))) { //Player is too far below
                //The player is too far away, move enemy to center
                if (level->enemy[i].initX != level->enemy[i].sprite.x) {
                    level->enemy[i].sprite.speedX = abs(level->enemy[i].sprite.speedX);
                    if (level->enemy[i].initX > level->enemy[i].sprite.x) {
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                    }
                    level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                }
                if (level->enemy[i].initY != level->enemy[i].sprite.y) {
                    if (level->enemy[i].initY > level->enemy[i].sprite.y) {
                        level->enemy[i].sprite.y += level->enemy[i].sprite.speedY;
                    } else {
                        level->enemy[i].sprite.y -= level->enemy[i].sprite.speedY;
                    }
                }
            } else {
                //The player is inside the guarded area, move enemy to player
                if (level->player.sprite.x != level->enemy[i].sprite.x) {
                    level->enemy[i].sprite.speedX = abs(level->enemy[i].sprite.speedX);
                    if (level->player.sprite.x > level->enemy[i].sprite.x) {
                        level->enemy[i].sprite.speedX = 0 - level->enemy[i].sprite.speedX;
                    }
                    level->enemy[i].sprite.x -= level->enemy[i].sprite.speedX;
                }
                if (level->player.sprite.y != level->enemy[i].sprite.y) {
                    if (level->player.sprite.y > level->enemy[i].sprite.y) {
                        level->enemy[i].sprite.y += level->enemy[i].sprite.speedY;
                    } else {
                        level->enemy[i].sprite.y -= level->enemy[i].sprite.speedY;
                    }
                }
            }
            break;
        } //switch (level->enemy[i].NMI_ACTION & 0x1FFF)
    } //for (i = 0; i < NMI_BY_LEVEL; i++)
}

DEAD1(TITUS_level *level, TITUS_enemy *enemy) {
    if (((enemy->dying & 0x01) != 0) || //00000001 or 00000011
      (enemy->dead_sprite == -1)) {
        if ((enemy->dying & 0x01) == 0) {
            enemy->dying = enemy->dying | 0x01;
            enemy->sprite.speedY = -10;
            enemy->phase = 0;
        }
        if (enemy->phase != 0xFF) {
            enemy->sprite.y += enemy->sprite.speedY;
            if (SEECHOC_FLAG != 0) {
                level->player.sprite2.y += enemy->sprite.speedY;
            }
            if (enemy->sprite.speedY < MAX_SPEED_DEAD) {
                enemy->sprite.speedY++;
            }
        }
    } else {
        enemy->dying = enemy->dying | 0x01;
        updateenemysprite(level, enemy, enemy->dead_sprite, false);
        enemy->sprite.flash = false;
        enemy->sprite.visible = false;
        enemy->sprite.speedY = 0;
        enemy->phase = -1;
    }
}

int updateenemysprite(TITUS_level *level, TITUS_enemy *enemy, int16 number, bool clearflags){
    updatesprite(level, &(enemy->sprite), number, clearflags);

    if ((number >= 101) && (number <= 105)) { //Walking man
        enemy->carry_sprite = 105;
    } else if ((number >= 126) && (number <= 130)) { //Fly
        enemy->carry_sprite = 130;
    } else if ((number >= 149) && (number <= 153)) { //Skeleton
        enemy->carry_sprite = 149;
    } else if ((number >= 157) && (number <= 158)) { //Worm
        enemy->carry_sprite = 158;
    } else if ((number >= 159) && (number <= 167)) { //Guy with sword
        enemy->carry_sprite = 167;
    } else if ((number >= 185) && (number <= 191)) { //Zombie
        enemy->carry_sprite = 186;
    } else if ((number >= 197) && (number <= 203)) { //Woman with pot
        enemy->carry_sprite = 203;
    } else {
        enemy->carry_sprite = -1;
    }

    if ((number >= 172) && (number <= 184)) { //Periscope
        enemy->dead_sprite = 184;
    } else if ((number >= 192) && (number <= 196)) { //Camel
        enemy->dead_sprite = 196;
    } else if ((number >= 210) && (number <= 213)) { //Old man with TV
        enemy->dead_sprite = 213;
    } else if ((number >= 214) && (number <= 220)) { //Snake in pot
        enemy->dead_sprite = 220;
    } else if ((number >= 221) && (number <= 226)) { //Man throwing knives
        enemy->dead_sprite = 226;
    } else if ((number >= 242) && (number <= 247)) { //Carnivorous plant in pot
        enemy->dead_sprite = 247;
    } else {
        enemy->dead_sprite = -1;
    }

    if (((number >= 248) && (number <= 251)) || //Man throwing rocks (3rd level)
      ((number >= 252) && (number <= 256)) || //Big baby (11th level)
      ((number >= 257) && (number <= 261)) || //Big woman (7th level)
      ((number >= 263) && (number <= 267)) || //Big man (15th level on Moktar only)
      ((number >= 284) && (number <= 288)) || //Mummy (9th level)
      ((number >= 329) && (number <= 332))) { //Ax man (5th level)
        enemy->boss = true;
    } else {
        enemy->boss = false;
    }
    
    return (0);
}


int SET_NMI(TITUS_level *level) {
    //Clear enemy sprites
    //If an enemy is on the screen
    // - Set bit 13
    // - Animate
    // - Collision with player
    //   - Loose life and fly
    // - Collision with object
    //   - Decrease enemy's life

    int16 i, k, hit;
    for (i = 0; i < level->enemycount; i++) { //50
        if (!(level->enemy[i].sprite.enabled)) continue; //Skip unused enemies
        level->enemy[i].visible = false;
        //Is the enemy on the screen?
        if ((level->enemy[i].sprite.x + 32 < BITMAP_X << 4) || //Left for the screen?
          (level->enemy[i].sprite.x - 32 > (BITMAP_X << 4) + screen_width * 16) || //Right for the screen?
          (level->enemy[i].sprite.y < BITMAP_Y << 4) || //Above the screen?
          (level->enemy[i].sprite.y - 32 > (BITMAP_Y << 4) + screen_height * 16)) { //Below the screen?
            if ((level->enemy[i].dying & 0x03) != 0) { //If the enemy is dying or dead and not on the screen, remove from the list!
                level->enemy[i].sprite.enabled = false;
            }
            continue;
        }
        level->enemy[i].visible = true;
        GAL_FORM(level, &(level->enemy[i])); //Animation
        if ((level->enemy[i].dying & 0x03) != 0) { //If the enemy is dying or dead and not on the screen, remove from the list!
            continue;
        }
        if ((KICK_FLAG == 0) && !GODMODE) { //Collision with the hero?
            if (level->enemy[i].sprite.invisible) {
                continue;
            }
            ACTIONC_NMI(level, &(level->enemy[i]));
        }
        hit = 0;
        if (GRAVITY_FLAG != 0) { //Collision with a moving object?
            for (k = 0; k < level->objectcount; k++) {
                if (level->object[k].sprite.speedX == 0) {
                    if (level->object[k].sprite.speedY == 0) {
                        continue;
                    }
                    if (level->object[k].mass < 10) {
                        continue;
                    }
                }
                if (level->object[k].objectdata->no_damage) { //Is the object a weapon (false) or not (true)?
                    continue;
                }
                if (NMI_VS_DROP(&(level->enemy[i].sprite), &(level->object[k].sprite))) {
                    hit = 1;
                    break;
                }
            }
        }
        if ((hit == 0) &&
          (DROP_FLAG != 0) &&
          (CARRY_FLAG == 0) &&
          (level->player.sprite2.enabled)) {
            if (NMI_VS_DROP(&(level->enemy[i].sprite), &(level->player.sprite2))) {
                INVULNERABLE_FLAG = 0;
                level->player.sprite2.enabled = false;
                SEE_CHOC(level);
                hit = 2;
            }
        }
        if (hit != 0) {
            if (hit == 1) {
                if (level->object[k].sprite.number != 73) { //Change direction of the object, except if the object is a small iron ball
                    level->object[k].sprite.speedX = 0 - level->object[k].sprite.speedX;
                }
            }
            //If final enemy, remove energy
            FX_START(1);
            DROP_FLAG = 0;
            if (level->enemy[i].boss) {
                if (INVULNERABLE_FLAG != 0) {
                    //j++;
                    continue;
                }
                INVULNERABLE_FLAG = 10;
                level->enemy[i].sprite.flash = true; //flash
                BIGNMI_POWER--;
                if (BIGNMI_POWER != 0) {
                    //j++;
                    continue;
                }
                boss_alive = false;
            }
            level->enemy[i].dying = level->enemy[i].dying | 0x02; //Kill the enemy
        }
    }
}


void GAL_FORM(TITUS_level *level, TITUS_enemy *enemy) { //Enemy animation
    int16 *image;
    enemy->sprite.invisible = false;
    if ((enemy->dying & 0x03) != 0) {
        enemy->sprite.visible = false;
        enemy->visible = true;
        return;
    }
    enemy->trigger = false;
    image = enemy->sprite.animation; //Animation pointer
    while (*image < 0) {
        image += (*image >> 1); //jump back to start of animation
    }
    if (*image == 0x55AA) {
        enemy->sprite.invisible = true;
        return;
    }
    enemy->trigger = *image & 0x2000;
    updateenemysprite(level, enemy, (*image & 0x00FF) + FIRST_NMI, true);
    enemy->sprite.flipped = (enemy->sprite.speedX < 0) ? true : false;
    image++;
    if (*image < 0) {
        image += (*image >> 1); //jump back to start of animation
    }
    enemy->sprite.animation = image;
    enemy->visible = true;
}


void ACTIONC_NMI(TITUS_level *level, TITUS_enemy *enemy) {
    switch (enemy->type) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 18:
        if (NMI_VS_DROP(&(enemy->sprite), &(level->player.sprite))) {
            if (enemy->type != 11) { //Walk and shoot
                if (enemy->sprite.number != 178) { //Periscope
                    enemy->sprite.speedX = 0 - enemy->sprite.speedX;
                }
            }
            if ((enemy->sprite.number >= FIRST_NMI + 53) &&
              (enemy->sprite.number <= FIRST_NMI + 55)) { //Fireball
                GRANDBRULE_FLAG = 1;
            }
            if (enemy->power != 0) {
                KICK_ASH(level, &(enemy->sprite), enemy->power);
            }
        }
        break;
    }
}


int KICK_ASH(TITUS_level *level, TITUS_sprite *enemysprite, int16 power) {
    FX_START(4);
    TITUS_sprite *p_sprite = &(level->player.sprite);
    DEC_ENERGY(level);
    DEC_ENERGY(level);
    KICK_FLAG = 24;
    CHOC_FLAG = 0;
    LAST_ORDER = 0;
    p_sprite->speedX = power;
    if (p_sprite->x <= enemysprite->x) {
        p_sprite->speedX = 0 - p_sprite->speedX;
    }
    p_sprite->speedY = -8*16;
    FORCE_POSE(level);
}


bool NMI_VS_DROP(TITUS_sprite *enemysprite, TITUS_sprite *sprite) {
    if (abs(sprite->x - enemysprite->x) >= 64) {
        return false;
    }
    if (abs(sprite->y - enemysprite->y) >= 70) {
        return false;
    }

    if (sprite->y < enemysprite->y) {
        //Enemy is below the offending object
        if (sprite->y <= enemysprite->y - enemysprite->spritedata->collheight + 3) return false; //The offending object is too high for collision
    } else {
        //Offending object is below the enemy
        if (enemysprite->y <= sprite->y - sprite->spritedata->collheight + 3) return false; //The enemy is too high for collision
    }
    int16 enemyleft = enemysprite->x - enemysprite->spritedata->refwidth;
    int16 objectleft = sprite->x - sprite->spritedata->refwidth;
    if (enemyleft >= objectleft) {
        //The object is left for the enemy
        if ((objectleft + (sprite->spritedata->collwidth >> 1)) <= enemyleft) {
            return false; //The object is too far left
        }
    } else {
        //Enemy is left for the object
        if ((enemyleft + (enemysprite->spritedata->collwidth >> 1)) <= objectleft) {
            return false; //The enemy is too far left
        }
    }
    return true; //Collision!
}


void SEE_CHOC(TITUS_level *level) {
    updatesprite(level, &(level->player.sprite2), FIRST_OBJET + 15, true); //Hit (a throw hits an enemy)
    level->player.sprite2.speedX = 0;
    level->player.sprite2.speedY = 0;
    SEECHOC_FLAG = 5;
}

int MOVE_TRASH(TITUS_level *level) {
    int16 i, tmp;
    for (i = 0; i < level->trashcount; i++) {
        if (!level->trash[i].enabled) continue;
        if (level->trash[i].speedX != 0) {
            level->trash[i].x += (level->trash[i].speedX >> 4);
            tmp = (level->trash[i].x >> 4) - BITMAP_X;
            if ((tmp < 0) || (tmp > screen_width)) {
                level->trash[i].enabled = false;
                continue;
            }
            if (tmp != 0) { //Bug in the code
                level->trash[i].y += (level->trash[i].speedY >> 4);
                tmp = (level->trash[i].y >> 4) - BITMAP_Y;
                if ((tmp < 0) || (tmp > screen_height*16)) { //Bug?
                    level->trash[i].enabled = false;
                    continue;
                }
            }
        }
        if (!GODMODE && NMI_VS_DROP(&(level->trash[i]), &(level->player.sprite))) { //Trash vs player
            level->trash[i].x -= level->trash[i].speedX;
            KICK_ASH(level, &(level->trash[i]), 70);
            level->trash[i].enabled = false;
            continue;
        }
    }
}

bool FIND_TRASH(TITUS_level *level, TITUS_sprite **trash) {
    int i;
    for (i = 0; i < level->trashcount; i++) {
        if (level->trash[i].enabled == false) {
            *trash = &(level->trash[i]);
            return true;
        }
    }
    return false;
}

void PUT_BULLET(TITUS_level *level, TITUS_enemy *enemy, TITUS_sprite *bullet) {
    bullet->x = enemy->sprite.x;
    bullet->y = enemy->sprite.y - (int8)(*(enemy->sprite.animation - 1) & 0x00FF);
    updatesprite(level, bullet, (*(enemy->sprite.animation - 2) & 0x1FFF) + FIRST_OBJET, true);
    if (enemy->sprite.x < level->player.sprite.x) {
        bullet->speedX = 16*11;
        bullet->flipped = true;
    } else {
        bullet->speedX = -16*11;
        bullet->flipped = false;
    }
    bullet->speedY = 0;
    bullet->x += bullet->speedX >> 4;
}

void UP_ANIMATION(TITUS_sprite *sprite) {
    do {
        sprite->animation++;
    } while (*sprite->animation >= 0);
    sprite->animation++;
}

void DOWN_ANIMATION(TITUS_sprite *sprite) {
    do {
        sprite->animation--;
    } while (*sprite->animation >= 0);
    sprite->animation--;
}
