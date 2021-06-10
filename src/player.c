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

/* player.c
 * Handles player movement and keyboard handling
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "level.h"
#include "player.h"
#include "globals.h"
#include "definitions.h"
#include "tituserror.h"
#include "draw.h"
#include "original.h"
#include "common.h"
#include "settings.h"

static int TAKE_BLK_AND_YTEST(TITUS_level *level, int16 tileY, uint8 tileX);
static void BLOCK_YYPRGD(TITUS_level *level, uint8 ceil, uint8 tileY, uint8 tileX);
static int BLOCK_XXPRG(TITUS_level *level, uint8 horiz, uint8 tileY, uint8 tileX);
static int XACCELERATION(TITUS_player *player, int16 maxspeed);
static int YACCELERATION(TITUS_player *player, int16 maxspeed);
static void BLOCK_YYPRG(TITUS_level *level, uint8 floor, uint8 floor_above, uint8 tileY, uint8 tileX);
static int CASE_BONUS(TITUS_level *level, uint8 tileY, uint8 tileX);
static int CASE_PASS(TITUS_level *level, uint8 viewlevel, uint8 tileY, uint8 tileX);
static int CASE_SECU(TITUS_level *level, uint8 tileY, uint8 tileX);
static int NEW_FORM(TITUS_player *player, uint8 action);
static int GET_IMAGE(TITUS_level *level);
static int YACCELERATION_NEG(TITUS_player *player, int16 maxspeed);
static int ACTION_PRG(TITUS_level *level, uint8 action);
int16 add_carry();

int move_player(TITUS_level *level) {
    //Part 1: Check keyboard input
    //Part 2: Determine the player's action, and execute action dependent code
    //Part 3: Move the player + collision detection
    //Part 4: Move the throwed/carried object
    //Part 5: decrease the timers

    int retval;
    int8 newsensX; //temporary SENSX
    SDL_Event event;
    TITUS_player *player = &(level->player);
    int16 newX, newY;
    bool pause = false;
    
    //Part 1: Check keyboard input
    SDL_PumpEvents(); //Update keyboard state
    keystate = SDL_GetKeyState(NULL);

    while(SDL_PollEvent(&event)) { //Check all events
        if (event.type == SDL_QUIT) {
            return TITUS_ERROR_QUIT;
        } else if (event.type == SDL_KEYDOWN) {
#ifdef _DINGUX
            if (event.key.keysym.sym == KEY_CHEAT && (devmode == 1)) {
                if (keystate[KEY_SKIPLEVEL]) {
                    NEWLEVEL_FLAG = true;
                } else if (!GODMODE && !NOCLIP) {
                    GODMODE = true;
                    NOCLIP = false;
                } else if (GODMODE && !NOCLIP) {
                    GODMODE = true;
                    NOCLIP = true;
                } else {
                    GODMODE = false;
                    NOCLIP = false;
                }
            } else if (event.key.keysym.sym == KEY_STATUS) {
                if (BAR_FLAG == 0) {
                    BAR_FLAG = 50; //Display energy
                } else {
                    viewstatus(level, false); //View status screen
                }
#else
            if ((event.key.keysym.sym == KEY_GODMODE) && (devmode == 1)) {
                if (GODMODE) {
                    GODMODE = false;
                    NOCLIP = false;
                } else {
                    GODMODE = true;
                }
            } else if ((event.key.keysym.sym == KEY_NOCLIP) && (devmode == 1)) {
                if (NOCLIP) {
                    NOCLIP = false;
                } else {
                    NOCLIP = true;
                    GODMODE = true;
                }
#endif

#ifdef DEBUG_VERSION
//Will display debug information
            } else if (event.key.keysym.sym == KEY_DEBUG) {
                DISPLAYLOOPTIME = !DISPLAYLOOPTIME;
#endif

            } else if (event.key.keysym.sym == KEY_MUSIC) {
                AUDIOMODE++;
                if (AUDIOMODE > 1) {
                    AUDIOMODE = 0;
                }
				if (AUDIOMODE == 1) {
					startmusic();
				}
            } else if (event.key.keysym.sym == KEY_P) {
                pause = true;
            }
        }
    }
    if (keystate[KEY_ESC]) {
        return TITUS_ERROR_QUIT;
    }
#ifdef _DINGUX

#else
    if (keystate[KEY_F1] && (RESETLEVEL_FLAG == 0)) { //F1 = suicide
        CASE_DEAD_IM(level);
        RESETLEVEL_FLAG--;
        return;
    }
    if (keystate[KEY_F2]) { //F2 = game over
        GAMEOVER_FLAG = true;
        return;
    }
    if (keystate[KEY_E]) { //E = display energy
        BAR_FLAG = 50;
    }
    if (keystate[SDLK_s] && (devmode == 1)) { //S = skip to next level
        NEWLEVEL_FLAG = true;
    }
    if (keystate[KEY_F4]) { //F4 = view status page
        viewstatus(level, false);
    }
#endif
// TODO: ADD!    SCREEN_3(); //Test for hidden credits screen
    if (pause) {
        retval = t_pause(level); //Apply pause
        if (retval < 0) {
            return retval;
        }
    }
    //JOYSTICK(); //Handle both joystick and keyboard movement keys


    //Part 2: Determine the player's action, and execute action dependent code
    X_FLAG = keystate[KEY_LEFT] | keystate[KEY_RIGHT]; //Set if either is true
    Y_FLAG = keystate[KEY_UP] | keystate[KEY_JUMP] | keystate[KEY_DOWN]; //Set if either is true
    if (NOCLIP) {
        if (keystate[KEY_LEFT]) {
            player->sprite.speedX = -100;
        } else if (keystate[KEY_RIGHT]) {
            player->sprite.speedX = 100;
        } else {
            player->sprite.speedX = 0;
        }
        if (keystate[KEY_UP] || keystate[KEY_JUMP]) {
            player->sprite.speedY = -100;
        } else if (keystate[KEY_DOWN]) {
            player->sprite.speedY = 100;
        } else {
            player->sprite.speedY = 0;
        }
        player->sprite.x += (player->sprite.speedX >> 4);
        player->sprite.y += (player->sprite.speedY >> 4);
        return;
    }

    if (CHOC_FLAG != 0) {
        action = 11; //Headache
    } else if (KICK_FLAG != 0) {
        if (GRANDBRULE_FLAG) {
            action = 13; //Hit (burn)
        } else {
            action = 12; //Hit
        }
    } else {
        GRANDBRULE_FLAG = false;
        if (LADDER_FLAG) {
            action = 6; //Action: climb
        } else if (!PRIER_FLAG && (keystate[KEY_UP] || keystate[KEY_JUMP]) && (SAUT_FLAG == 0)) {
            action = 2; //Action: jump
            if (LAST_ORDER == 5) { //Test if last order was kneestanding
                FURTIF_FLAG = 100; //If jump after kneestanding, init silent walk timer
            }
        } else if (PRIER_FLAG || ((SAUT_FLAG != 6) && keystate[KEY_DOWN])) {
            if (X_FLAG) { //Move left or right
                action = 3; //Action: crawling
            } else {
                action = 5; //Action: kneestand
            }
        } else if (X_FLAG) {
            action = 1; //Action: walk
        } else {
            action = 0;  //Action: rest (no action)
        }
        //Is space button pressed?
        if (keystate[KEY_SPACE] && !PRIER_FLAG) {
            if (!DROP_FLAG) {
                if ((action == 3) || (action == 5)) { //Kneestand
                    DROPREADY_FLAG = false;
                    action = 7; //Grab an object
                } else if (CARRY_FLAG && DROPREADY_FLAG) { //Fall
                    action = 8; //Drop the object
                }
            }
        } else {
            DROPREADY_FLAG = true;
            POSEREADY_FLAG = false;
        }
    }
    if (CARRY_FLAG) {
        action += 16;
    }

    if ((CHOC_FLAG != 0) || (KICK_FLAG != 0)) {
        if (SENSX < 0) { // -1
            newsensX = -1;
        } else { // 0 or 1
            newsensX = 0;
        }
    } else if (keystate[KEY_LEFT]) {
        newsensX = -1;
    } else if (keystate[KEY_RIGHT]) {
        newsensX = 1;
    } else if (SENSX == -1) {
        newsensX = -1;
    } else if (action == 0) {
        newsensX = 0;
    } else {
        newsensX = 1;
    }

    if (SENSX != newsensX) {
        SENSX = newsensX;
        ACTION_TIMER = 1;
    } else {
        if (((action == 0) || (action == 1)) && (FURTIF_FLAG != 0)) {
            //Silent walk?
            action += 9;
        }
        if (action != LAST_ORDER) {
            ACTION_TIMER = 1;
        } else if (ACTION_TIMER < 0xFF) {
            ACTION_TIMER += 1;
        }
    }
    ACTION_PRG(level, action); //call movement function based on ACTION

    //Part 3: Move the player + collision detection
    //Move the player in X if the new position doesn't exceed 8 pixels from the edges
    if (((player->sprite.speedX < 0) && ((player->sprite.x + (player->sprite.speedX >> 4)) >= 8)) || //Going left
      ((player->sprite.speedX > 0) && ((player->sprite.x + (player->sprite.speedX >> 4)) <= (level->width << 4) - 8))) { //Going right
        player->sprite.x += player->sprite.speedX >> 4;
    }
    //Move player in Y
    player->sprite.y += (player->sprite.speedY >> 4);
    //Test for collisions
    BRK_COLLISION(level);

    //Part 4: Move the throwed/carried object
    //Move throwed/carried object
    if (DROP_FLAG) {
        //sprite2: throwed or dropped object
        newX = (player->sprite2.speedX >> 4) + player->sprite2.x;
        if ((newX < (level->width << 4)) && //Left for right level edge
          (newX >= 0) && //Right for level left edge
          (newX >= (BITMAP_X << 4) - GESTION_X) && //Max 40 pixels left for screen (bug: the purpose was probably one screen left for the screen)
          (newX <= (BITMAP_X << 4) + (screen_width << 4) + GESTION_X)) { //Max 40 pixels right for screen
            player->sprite2.x = newX;
            newY = (player->sprite2.speedY >> 4) + player->sprite2.y;
            if ((newY < (level->height << 4)) && //Above bottom edge of level
              (newY >= 0) && //Below top edge of level
              (newY >= (BITMAP_Y << 4) - GESTION_Y) && //Max 20 pixels above the screen (bug: the purpose was probably one screen above the screen)
              (newY <= (BITMAP_Y << 4) + (screen_height << 4) + GESTION_Y)) { //Max 20 pixels below the screen
                player->sprite2.y = newY;
            } else {
                player->sprite2.enabled = false;
                DROP_FLAG = false;
            }
        } else {
            player->sprite2.enabled = false;
            DROP_FLAG = false;
        }
    } else if (CARRY_FLAG) { //Place the object on top of or beside the player
        if (!LADDER_FLAG && ((LAST_ORDER == 16+5) || (LAST_ORDER == 16+7))) { //Kneestand or take
            player->sprite2.y = player->sprite.y - 4;
            if (player->sprite.flipped) {
                player->sprite2.x = player->sprite.x - 10;
            } else {
                player->sprite2.x = player->sprite.x + 12;
            }
        } else {
            if ((player->sprite.number == 14) ||  //Sliding down the ladder OR
              (((LAST_ORDER & 0x0F) != 7) &&  //Not taking
              ((LAST_ORDER & 0x0F) != 8))) { //Not throwing/dropping
                player->sprite2.x = player->sprite.x + 2;
                if ((player->sprite.number == 23) || //Climbing (c)
                  (player->sprite.number == 24)) { //Climbing (c) (2nd sprite)
                    player->sprite2.x -= 10;
                    if (player->sprite.flipped) {
                        player->sprite2.x += 18;
                    }
                }
                player->sprite2.y = player->sprite.y - player->sprite.spritedata->collheight + 1;
            }
        }
    }
    if (SEECHOC_FLAG != 0) {
        SEECHOC_FLAG--;
        if (SEECHOC_FLAG == 0) {
            player->sprite2.enabled = false;
        }
    }

    //Part 5: decrease the timers
    subto0(&INVULNERABLE_FLAG);
    subto0(&RESETLEVEL_FLAG);
    subto0(&TAPISFLY_FLAG);
    subto0(&CROSS_FLAG);
    subto0(&GRAVITY_FLAG);
    subto0(&FURTIF_FLAG);
    subto0(&KICK_FLAG);
    if (player->sprite.speedY == 0) {
        subto0(&CHOC_FLAG);
    } 
    if ((player->sprite.speedX == 0) && (player->sprite.speedY == 0)) {
        KICK_FLAG = 0;
    }
    subto0(&FUME_FLAG); //Smoke when object hits the floor
    if ((FUME_FLAG != 0) && ((FUME_FLAG & 0x03) == 0)) {
        //Smoke animation
        updatesprite(level, &(player->sprite2), player->sprite2.number + 1, false);
        if (player->sprite2.number == FIRST_OBJET + 19) {
            //Remove smoke
            player->sprite2.enabled = false;
            FUME_FLAG = 0;
        }
    }
}

CASE_DEAD_IM (TITUS_level *level) {
    //Kill the player immediately (spikes/water/flames etc.
    //Sets RESET_FLAG to 2, in opposite to being killed as a result of 0 HP (then RESET_FLAG is 10)
    DEC_LIFE(level);
    RESETLEVEL_FLAG = 2;
}


DEC_LIFE (TITUS_level *level) {
    //Kill the player, check for gameover, hide the energy bar
    RESETLEVEL_FLAG = 10;
    BAR_FLAG = 0;
    if (level->lives == 0) {
        GAMEOVER_FLAG = true;
    }
}
    
t_pause (TITUS_level *level) {
    bool pass;
    TITUS_player *player = &(level->player);
    SDL_Event event;
    TITUS_sprite tmp;
    //tmp.buffer = NULL;

    TFR_SCREENM(); //Draw tiles
    copysprite(level, &(tmp), &(player->sprite));
    updatesprite(level, &(player->sprite), 29, true); //Pause tile
    DISPLAY_SPRITES(level); //Draw sprites
    flip_screen(true); //Display it
    copysprite(level, &(player->sprite), &(tmp)); //Reset player sprite
    //SDL_FreeSurface(tmp.buffer);
    do {
        titus_sleep();
        keystate = SDL_GetKeyState(NULL);
        while(SDL_PollEvent(&event)) { //Check all events
            if (event.type == SDL_QUIT) {
                return TITUS_ERROR_QUIT;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == KEY_ESC) {
                    return TITUS_ERROR_QUIT;
                } else if (event.key.keysym.sym == KEY_MUSIC) {
					AUDIOMODE++;
					if (AUDIOMODE > 1) {
						AUDIOMODE = 0;
					}
					if (AUDIOMODE == 1) {
						startmusic();
					}
                } else if (event.key.keysym.sym == KEY_P) {
                    return 0;
                }
            }
        }
    } while (1);
}

BRK_COLLISION(TITUS_level *level) { //Collision detection between player and tiles/objects/elevators
    //Point the foot on the block!
    TITUS_player *player = &(level->player);
    int16 changeX;
    int16 height;
    int16 initY;
    uint8 tileX;
    int16 tileY;
    int16 colltest;
    uint8 left_tileX;
    bool first;
    uint8 hflag;
    
    tileX = (player->sprite.x >> 4);
    tileY = (player->sprite.y >> 4) - 1;
    initY = tileY;

    //if too low then die!
    if ((player->sprite.y > ((level->height + 1) << 4)) && !NOCLIP) {
        CASE_DEAD_IM(level);
    }

    
    //Test under the feet of the hero and on his head! (In y)
    YFALL = 0;
    //Find the left tile
    //colltest can be 0 to 15 +- 8 (-1 to -8 will change into 255 to 248)
    colltest = player->sprite.x & 0x0F;
    if (colltest < TEST_ZONE) {
        colltest += 256;
        tileX--;
    }
    colltest -= TEST_ZONE;

    left_tileX = tileX;
    TAKE_BLK_AND_YTEST(level, tileY, tileX); //Test the tile for vertical blocking
    if (YFALL == 1) { //Have the fall stopped?
        //No! Is it necessary to test the right tile?
        colltest += TEST_ZONE * 2; //4 * 2
//      if (colltest > 255) {
//          colltest -= 256;
//          tileX++;
//      } elseif
        if (colltest > 15) {
            tileX++;
        }
        if (tileX != left_tileX) {
            TAKE_BLK_AND_YTEST(level, tileY, tileX); //Also test the left tile
        }
        if (YFALL == 1) {
            if ((CROSS_FLAG == 0) && (CHOC_FLAG == 0)) {
                COLLISION_TRP(level); //Player versus elevators
                if (YFALL == 1) {
                    COLLISION_OBJET(level); //Player versus objects
                    if (YFALL == 1) {
                        ARAB_TOMBE(level); //No wall/elevator/object under the player; fall down!
                    } else {
                        player->GLISSE = 0;
                    }
                }
            } else {
                ARAB_TOMBE(level); //Fall down!
            }
        }
    }

    //How will the player move in X?
    changeX = TEST_ZONE + MAX_X; //4 + 4 ??? + max_speed_x
    if (player->sprite.speedX < 0) {
        changeX = 0 - changeX;
    } else if (player->sprite.speedX == 0) {
        changeX = 0;
    }

    //Test the hero (in x)
    height = player->sprite.spritedata->collheight;
    if ((player->sprite.y > MAP_LIMIT_Y + 1) && (initY >= 0) && (initY < level->height)) {
        tileX = ((player->sprite.x + changeX) >> 4);
        tileY = initY;
        first = true;
        do {
            hflag = get_horizflag(level, tileY, tileX);
            if (first) {
                BLOCK_XXPRG(level, hflag, tileY, tileX);
                first = false;
            } else if ((hflag == HFLAG_CODE) || (hflag == HFLAG_BONUS)) { //level code or HP
                BLOCK_XXPRG(level, hflag, tileY, tileX);
            }
            if (tileY == 0) {
                return;
            }
            tileY--;
            height -= 16;
        } while (height > 0);
    }
}

static int TAKE_BLK_AND_YTEST(TITUS_level *level, int16 tileY, uint8 tileX) {
    //Test the current tile for vertical blocking

    TITUS_player *player = &(level->player);
    POCKET_FLAG = false;
    PRIER_FLAG = false;
    LADDER_FLAG = false;
    uint8 horiz, ceil;
    int8 change;
    uint8 floor, floor_above;
    if ((player->sprite.y <= MAP_LIMIT_Y) || (tileY < -1)) { //if player is too high (<= -1), skip test
        ARAB_TOMBE(level);
        YFALL = 0xFF;
        return;
    }
    if (tileY + 1 >= level->height) { //if player is too low, skip test
        ARAB_TOMBE(level);
        YFALL = 0xFF;
        return;
    }
    if (tileY == -1) { //In order to fall down in the right chamber if jumping above level 8
        tileY = 0;
    }
    floor = get_floorflag(level, tileY + 1, tileX);
    floor_above = get_floorflag(level, tileY, tileX);

    if ((LAST_ORDER & 0x0F) != 2) { //2=SAUTER
        BLOCK_YYPRG(level, floor, floor_above, tileY + 1, tileX); //Player versus floor
    }
    //Test the tile on his head
    if ((tileY < 1) || (player->sprite.speedY > 0)) {
        return;
    }
    horiz = get_horizflag(level, tileY, tileX);
    ceil = get_ceilflag(level, tileY - 1, tileX);
    BLOCK_YYPRGD(level, ceil, tileY - 1, tileX); //Player versus ceiling

    if (((horiz == HFLAG_WALL) || (horiz == HFLAG_DEADLY) || (horiz == HFLAG_PADLOCK)) && //Step on a hard tile?
      (player->sprite.y > MAP_LIMIT_Y + 1)) {
        if (player->sprite.speedX > 0) {
            change = -1;
        } else {
            change = 1;
        }
        tileX += change;
        horiz = get_horizflag(level, tileY, tileX);
        if (horiz == 0) { //No wall
            player->sprite.x += change << 1;
        } else {
            change = 0 - change;
            tileX += change + change;
            horiz = get_horizflag(level, tileY, tileX);
            if (horiz == 0) {
                player->sprite.x += change << 1;
            }
        }
    }
}

static void BLOCK_YYPRGD(TITUS_level *level, uint8 ceil, uint8 tileY, uint8 tileX) {
    TITUS_player *player = &(level->player);
    TITUS_object *object;
    uint8 hflag;
    //Action on different ceiling flags
    switch (ceil) {

    case 0: //No ceiling
        break;

    case 1: //Ceiling
    case 4: //Deadly
        if ((ceil == 4) && !GODMODE) {
            CASE_DEAD_IM(level);
        } else if (player->sprite.speedY != 0) {
            //Stop movement
            player->sprite.speedY = 0;
            player->sprite.y = (player->sprite.y & 0xFFF0) + 16;
            SAUT_COUNT = 0xFF;
        } else if ((player->sprite.number != 10) && //10 = Free fall
          (player->sprite.number != 21) && //21 = Free fall (c)
          (SAUT_FLAG != 6)) { //
            PRIER_FLAG = true;
            if (CARRY_FLAG) {
                object = FORCE_POSE(level);
                if (object != NULL) {
                    tileX = object->sprite.x >> 4;
                    tileY = object->sprite.y >> 4;
                    hflag = get_horizflag(level, tileY, tileX);
                    if ((hflag == HFLAG_WALL) || (hflag == HFLAG_DEADLY) || (hflag == HFLAG_PADLOCK)) {
                        tileX--;
                        hflag = get_horizflag(level, tileY, tileX);
                        if ((hflag == HFLAG_WALL) || (hflag == HFLAG_DEADLY) || (hflag == HFLAG_PADLOCK)) {
                            object->sprite.x += 16;
                        } else {
                            object->sprite.x -= 16;
                        }
                    }
                }
            }
        }
        break;

    case 2: //Ladder
        if ((player->sprite.speedY < 0) && (player->sprite.speedX == 0)) {
            SAUT_COUNT = 10;
            LADDER_FLAG = 1;
        }
        break;

    case 3: //Padlock
        CASE_SECU(level, tileY, tileX);
        break;

    //case 4: on case 1
    }
}

static int BLOCK_XXPRG(TITUS_level *level, uint8 horiz, uint8 tileY, uint8 tileX) {
    //Action on different horizontal flags
    switch (horiz) {
    case 0: //No wall
        break;

    case 1: //Wall
        ARAB_BLOCKX(level);
        break;

    case 2: //HP
        CASE_BONUS(level, tileY, tileX);
        break;

    case 3: //Unknown, looks deadly
        if (!GODMODE) {
            CASE_DEAD_IM(level);
        } else {
            ARAB_BLOCKX(level); //Godmode: wall
        }
        break;

    case 4: //Level code
        CASE_PASS(level, level->levelnumber, tileY, tileX);
        break;

    case 5: //Padlock
        CASE_SECU(level, tileY, tileX);
        break;

    case 6: //Level 14 code
        CASE_PASS(level, 14 - 1, tileY, tileX);
        break;
    }
}

ARAB_BLOCKX(TITUS_level *level) {
    TITUS_player *player = &(level->player);
    //Horizontal hit (wall), stop the player
    player->sprite.x -= player->sprite.speedX >> 4;
    player->sprite.speedX = 0;
   if ((KICK_FLAG != 0) && (SAUT_FLAG != 6)) {
        CHOC_FLAG = 20;
        KICK_FLAG = 0;
    }
}




TITUS_object *FORCE_POSE(TITUS_level *level) {
    //Move object from sprite2 to TMPFP
    TITUS_sprite *sprite2 = &(level->player.sprite2);
    TITUS_object *object;
    int16 i;
    if ((sprite2->enabled) && (CARRY_FLAG)) {
        //Drop the carried object
        i = 0;
        do {
            if (i > level->objectcount) {
                printf("ERROR: FORCE_POSE returned NULL\n");
                return NULL;
            }
            if (!(level->object[i].sprite.enabled)) break;
            i++;
        } while (1);
        object = &(level->object[i]);
        updateobjectsprite(level, object, sprite2->number, true);
        sprite2->enabled = false;
        object->sprite.killing = false;
        if (object->sprite.number < FIRST_NMI) {
            object->sprite.droptobottom = false;
        } else {
            object->sprite.droptobottom = true;
        }
        object->sprite.x = sprite2->x;
        object->sprite.y = sprite2->y;
        object->mass = 0;
        object->sprite.speedY = 0;
        object->sprite.speedX = 0;
        object->sprite.UNDER = 0;
        object->sprite.ONTOP = NULL;
        POSEREADY_FLAG = true;
        GRAVITY_FLAG = 4;
        CARRY_FLAG = false;
        return object;
    }
}



ARAB_TOMBE(TITUS_level *level) {
    //No wall under the player; fall down!
    TITUS_player *player = &(level->player);
    SAUT_FLAG = 6;
    if (KICK_FLAG != 0) {
        return;
    }
    XACCELERATION(player, MAX_X*16);
    YACCELERATION(player, MAX_Y*16);
    if (CHOC_FLAG != 0) {
        updatesprite(level, &(player->sprite), 15, true); //sprite when hit
    } else if (CARRY_FLAG == 0) {
        updatesprite(level, &(player->sprite), 10, true); //position while falling  (jump sprite?)
    } else {
        updatesprite(level, &(player->sprite), 21, true); //position falling and carry  (jump and carry sprite?)
    }
    player->sprite.flipped = (SENSX < 0);
}

static int XACCELERATION(TITUS_player *player, int16 maxspeed) {
    //Sideway acceleration
    int16 changeX;
    if (X_FLAG) {
        changeX = (SENSX << 4) >> player->GLISSE;
    } else {
        changeX = 0;
    }

    if (player->sprite.speedX + changeX >= maxspeed) {
        player->sprite.speedX = maxspeed;
    } else if (player->sprite.speedX + changeX <= 0 - maxspeed) {
        player->sprite.speedX = 0 - maxspeed;
    } else {
        player->sprite.speedX += changeX;
    }
}

static int YACCELERATION(TITUS_player *player, int16 maxspeed) {
    //Accelerate downwards
    if ((player->sprite.speedY + 32/2) < maxspeed) {
        player->sprite.speedY = player->sprite.speedY + (32/2);
    } else {
        player->sprite.speedY = maxspeed;
    }
}


static void BLOCK_YYPRG(TITUS_level *level, uint8 floor, uint8 floor_above, uint8 tileY, uint8 tileX) {
    //Action on different floor flags
    TITUS_player *player = &(level->player);
    uint8 order;
    switch (floor) {

    case 0: //No floor
        ARAB_TOMBE_F();
        break;

    case 1: //Floor
        ARAB_BLOCK_YU(player);
        break;

    case 2: //Slightly slippery floor
        ARAB_BLOCK_YU(player);
        player->GLISSE = 1;
        break;

    case 3: //Slippery floor
        ARAB_BLOCK_YU(player);
        player->GLISSE = 2;
        break;

    case 4: //Very slippery floor
        ARAB_BLOCK_YU(player);
        player->GLISSE = 3;
        break;

    case 5: //Drop-through if kneestanding
        player->GLISSE = 0;
        if (CROSS_FLAG == 0) {
            ARAB_BLOCK_YU(player);
        } else {
            ARAB_TOMBE_F(); //Drop through!
        }
        break;

    case 6: //Ladder
        //Fall if hit
        //Skip if walking/crawling
        if (CHOC_FLAG != 0) {
            ARAB_TOMBE_F(); //Free fall
            return;
        }
        order = LAST_ORDER & 0x0F;
        if ((order == 1) ||
          (order == 3) ||
          (order == 7) ||
          (order == 8)) {
            ARAB_BLOCK_YU(player); //Stop fall
            return;
        }
        if (order == 5) { // action baisse
            ARAB_TOMBE_F(); //Free fall
            updatesprite(level, &(player->sprite), 14, true); //sprite: start climbing down
            player->sprite.y += 8;
        }
        if (floor_above != 6) { //ladder
            if (order == 0) { //action repos
                ARAB_BLOCK_YU(player); //Stop fall
                return;
            }
            if ((keystate[KEY_UP] || keystate[KEY_JUMP]) && (order == 6)) { //action UP + climb ladder
                ARAB_BLOCK_YU(player); //Stop fall
                return;
            }
        }

        subto0(&SAUT_FLAG);
        SAUT_COUNT = 0;
        YFALL = 2;

        LADDER_FLAG = 1;
        break;

    case 7:
        CASE_BONUS(level, tileY, tileX);
        break;

    case 8:
    case 9:
    case 10:
        if (!GODMODE) {
            CASE_DEAD_IM(level);
        } else {
            ARAB_BLOCK_YU(player); //If godmode; ordinary floor
        }
        break;

    case 11:
        CASE_PASS(level, level->levelnumber, tileY, tileX);
        break;

    case 12:
        CASE_SECU(level, tileY, tileX);
        break;

    case 13:
        CASE_PASS(level, 14 - 1, tileY, tileX);
        break;
    }
}

void ARAB_TOMBE_F() {
    //Player free fall (doesn't touch floor)
    YFALL = YFALL | 0x01;
}

void ARAB_BLOCK_YU(TITUS_player *player) {
    //Floor; the player will not fall through
    POCKET_FLAG = true;
    player->GLISSE = 0;
    if (player->sprite.speedY < 0) {
        YFALL = YFALL | 0x01;
        return;
    }
    player->sprite.y = player->sprite.y & 0xFFF0;
    player->sprite.speedY = 0;
    subto0(&SAUT_FLAG);
    SAUT_COUNT = 0;
    YFALL = 2;
}

static int CASE_BONUS(TITUS_level *level, uint8 tileY, uint8 tileX) {
    //Handle bonuses. Increase energy if HP, and change the bonus tile to normal tile
    uint16 i = 0;
    do { //Is the bonus in the list?
        if (i >= MAXIMUM_BONUS) {
            return false; //Bonus not found
        }
        i++;
    } while ((level->bonus[i - 1].x != tileX) || (level->bonus[i - 1].y != tileY));
    i--;
    //If the bonus is 253-255, it's HP. Increase life!
    if (level->bonus[i].bonustile >= 255 - 2) {
        //Increase HP if tile number is >= 253
        SELECT_MUSIC(6);
        INC_ENERGY(level);
    }
    //Return the original tile underneath the bonus
    level->tilemap[tileY][tileX] = level->bonus[i].replacetile;
    
    DISPLAY_CHAR(level, level->bonus[i].replacetile, tileY % screen_height, tileX % screen_width);
    GRAVITY_FLAG = 4;
    PERMUT_FLAG = true;
    return true; //No problems, bonus handling done correctly!
}

static int CASE_PASS(TITUS_level *level, uint8 level_index, uint8 tileY, uint8 tileX) {
    //Codelamp
    SELECT_MUSIC(7);
    if (CASE_BONUS(level, tileY, tileX)) { //if the bonus is found in the bonus list
        view_password(level, level_index);
    }
}

static int CASE_SECU(TITUS_level *level, uint8 tileY, uint8 tileX) {
    TITUS_player *player = &(level->player);
    //Padlock, store X/Y coordinates
    SELECT_MUSIC(5);
    if (CASE_BONUS(level, tileY, tileX)) { //if the bonus is found in the bonus list
        player->initX = player->sprite.x;
        player->initY = player->sprite.y;
        if ((player->sprite2.number == FIRST_OBJET+26) || (player->sprite2.number == FIRST_OBJET+27)) {
            //If carrying the cage, save the position
            player->cageX = player->sprite.x;
            player->cageY = player->sprite.y;
        }
    }
}

void INC_ENERGY(TITUS_level *level) {
    TITUS_player *player = &(level->player);
    BAR_FLAG = 50;
    player->hp++;
    if (player->hp > MAXIMUM_ENERGY) {
        player->hp = MAXIMUM_ENERGY;
        level->extrabonus++;
    }
}

void DEC_ENERGY(TITUS_level *level) {
    TITUS_player *player = &(level->player);
	BAR_FLAG = 50;
    if (RESETLEVEL_FLAG == 0) {
        player->hp--;
        if (player->hp < 0) {
            player->hp = 0;
            DEC_LIFE(level);
        }
    }
}

static int ACTION_PRG(TITUS_level *level, uint8 action) {
    //Action dependent code
    TITUS_player *player = &(level->player);
    uint8 tileX, tileY, fflag;
    TITUS_object *object;
    int16 tmpY, tmpX, i, diffX, speedX, speedY;

    switch (action) {
    case 0:
    case 9:
    case 16:
        //Rest. Handle deacceleration and slide
        LAST_ORDER = action;
        DECELERATION(player);
        if ((abs(player->sprite.speedX) >= 1 * 16) &&
           (player->sprite.flipped == (player->sprite.speedX < 0))) {
            player->sprite.animation = anim_player[4 + add_carry()];
        } else {
            player->sprite.animation = anim_player[action];
        }
        updatesprite(level, &(player->sprite), *(player->sprite.animation), true);
        player->sprite.flipped = (SENSX < 0);
        break;

    case 1:
    case 17:
    case 19:
        //Handle walking
        XACCELERATION(player, MAX_X * 16);
        NEW_FORM(player, action); //Update last order and action (animation)
        GET_IMAGE(level); //Update player sprite
        break;

    case 2:
    case 18:
        //Handle a jump
        if (SAUT_COUNT >= 3) {
            SAUT_FLAG = 6; //Stop jump animation and acceleration
        } else {
            SAUT_COUNT++;
            YACCELERATION_NEG(player, MAX_Y * 16 / 4);
            XACCELERATION(player, MAX_X * 16);
            NEW_FORM(player, action);
            GET_IMAGE(level);
        }
        break;
    
    case 3:
        //Handle crawling
        NEW_FORM(player, action);
        GET_IMAGE(level);
        XACCELERATION(player, MAX_X * 16 / 2);
        if (abs(player->sprite.speedX) < (2 * 16)) {
            updatesprite(level, &(player->sprite), 6, true); //Crawling but not moving
            player->sprite.flipped = (SENSX < 0);
        }
        break;

    case 4:
    case 14:
    case 15:
    case 20:
    case 25:
    case 26:
        //No action?
        break;

    case 5:
        //Kneestand
        NEW_FORM(player, action);
        GET_IMAGE(level);
        DECELERATION(player);
        if (ACTION_TIMER == 15) {
            CROSS_FLAG = 6;
            player->sprite.speedY = 0;
        }
        break;

    case 6:
    case 22:
        //Climb a ladder
        if (X_FLAG != 0) {
            XACCELERATION(player, MAX_X * 16);
        } else {
            DECELERATION(player);
        }
        if (ACTION_TIMER <= 1) {
            if (CARRY_FLAG == 0) {
                updatesprite(level, &(player->sprite), 12, true); //Last climb sprite
            } else {
                updatesprite(level, &(player->sprite), 23, true); //First climb sprite (c)
            }
        }
        if (Y_FLAG != 0) {
            NEW_FORM(player, 6 + add_carry()); //Action: Climb
            GET_IMAGE(level);
            //Place the player on the ladder
            player->sprite.x = (player->sprite.x & 0xFFF0) + 8;
            tileX = (player->sprite.x) >> 4;
            tileY = (player->sprite.y & 0xFFF0) >> 4;
            if (get_floorflag(level, tileY, tileX) != FFLAG_LADDER) {

                if (get_floorflag(level, tileY, tileX - 1) == FFLAG_LADDER) {
                    player->sprite.x -= 16;
                } else if (get_floorflag(level, tileY, tileX + 1) == FFLAG_LADDER) {
                    player->sprite.x += 16;
                }
            }
            if (!keystate[KEY_UP] && !keystate[KEY_JUMP]) {
                player->sprite.speedY = 4 * 16;
            } else {
                player->sprite.speedY = 0 - (4 * 16);
            }
        } else {
            player->sprite.speedY = 0;
        }
        break;

    case 7:
    case 23:
        //Take a box
        NEW_FORM(player, action);
        GET_IMAGE(level);
        DECELERATION(player);
        if (!POSEREADY_FLAG) {
            if ((ACTION_TIMER == 1) && (CARRY_FLAG)) {
                //If the object is placed in a block, fix a speedX
                object = FORCE_POSE(level);
                if (object != NULL) {
                    tileX = object->sprite.x >> 4;
                    tileY = object->sprite.y >> 4;
                    fflag = get_floorflag(level, tileY, tileX);
                    if ((fflag != FFLAG_NOFLOOR) && (fflag != FFLAG_WATER)) { //this test is really floor flag & 0x07
                        tileX++;
                        fflag = get_floorflag(level, tileY, tileX);
                        if ((fflag != FFLAG_NOFLOOR) && (fflag != FFLAG_WATER)) {
                            object->sprite.speedX = 16 * 3;
                        } else {
                            object->sprite.speedX = 0 - (16 * 3);
                        }
                    }
                }
            } else {
                if (!CARRY_FLAG) {
                    tmpY = player->sprite.y;
                    tmpX = player->sprite.x;
                    for (i = 0; i < level->objectcount; i++) {
                        //First do a quick test
                        if (!(level->object[i].sprite.enabled) ||
                          (abs(player->sprite.y - level->object[i].sprite.y) >= 20)) {
                            continue;
                        }
                        diffX = player->sprite.x - level->object[i].sprite.x;
                        if (!player->sprite.flipped) {
                            diffX = 0 - diffX;
                        }
                        if (game == 1) { //Moktar
                            if (diffX >= 25) {
                                continue;
                            }
                        } else if (game == 0) { //TTF
                            if (diffX >= 20) {
                                continue;
                            }
                        }
                        //The ordinary test
                        //X
/*
                        if (player->sprite.x > player->sprite.x) { //Bug: the first expression should have been level->object[i]
                            if (player->sprite.x + 32 < player->sprite.x) { //This will never execute
                                continue;
                            }
                        } else {
                            if (player->sprite.x + level->object[i].sprite.spritedata->collwidth < player->sprite.x) {
                                continue; //This should not execute
                            }
                        }
*/

                        //What it probably should have been
                      if (level->object[i].sprite.x > player->sprite.x) { //The object is right
                          if (level->object[i].sprite.x > player->sprite.x + 32) {
                              continue; //The object is too far right
                          }
                      } else { //The object is left
                          if (level->object[i].sprite.x + level->object[i].sprite.spritedata->collwidth < player->sprite.x) {
                              continue; //The object is too far left
                          }
                      }

                        //Y
                        if (level->object[i].sprite.y < player->sprite.y) { //The object is above
                            if (level->object[i].sprite.y <= player->sprite.y - 10) {
                                continue;
                            }
                        } else { //The object is below
                            if (level->object[i].sprite.y - level->object[i].sprite.spritedata->collheight + 1 >= player->sprite.y) {
                                continue;
                            }
                        }

                        //Take the object
                        FX_START(9); //Sound effect
                        FUME_FLAG = 0;
                        level->object[i].sprite.speedY = 0;
                        level->object[i].sprite.speedX = 0;
                        GRAVITY_FLAG = 4;
                        copysprite(level, &(player->sprite2), &(level->object[i].sprite));
                        level->object[i].sprite.enabled = false;
                        CARRY_FLAG = true;
                        SEECHOC_FLAG = 0;
                        if (player->sprite2.number == FIRST_OBJET+19) { //flying carpet
                            TAPISWAIT_FLAG = 0;
                        }
                        player->sprite2.y = player->sprite.y - 4;
                        if (player->sprite.flipped) {
                            player->sprite2.x = player->sprite.x - 10;
                        } else {
                            player->sprite2.x = player->sprite.x + 12;
                        }

                        break;
                    }
                    if (!CARRY_FLAG) { //No objects taken, check if he picks up an enemy!
                        for (i = 0; i < level->enemycount; i++) {
                            //First do a quick test
                            if ((!level->enemy[i].sprite.enabled) ||
                              (abs(player->sprite.y - level->enemy[i].sprite.y) >= 20)) {
                                continue;
                            }
                            diffX = player->sprite.x - level->enemy[i].sprite.x;
                            if (!player->sprite.flipped) {
                                diffX = 0 - diffX;
                            }
                            if (game == 1) { //Moktar
                                if (diffX >= 25) {
                                    continue;
                                }
                            } else if (game == 0) { //TTF
                                if (diffX >= 20) {
                                    continue;
                                }
                            }
                            //The ordinary test
                            //X
                            if (level->enemy[i].carry_sprite == -1) { //Test if the enemy can be picked up from behind
                                continue;
                            }

                         //   if (player->x > player->x) { //Bug: the first expression should have been enemy[i]
                         //       if (player->x + 32 < player->x) { //This will never execute
                         //           continue;
                         //       }
                         //   } else {
                         //       if (player->x + enemy[i].sprite.spritedata->collwidth < player->x) {
                         //           continue; //This should not execute
                         //       }
                         //   }
                            

                            //What it probably should have been
                            if (level->enemy[i].sprite.x > player->sprite.x) { //The enemy is right
                                if (level->enemy[i].sprite.x > player->sprite.x + 32) {
                                    continue; //The enemy is too far right
                                }
                            } else { //The object is left
                                if (level->enemy[i].sprite.x + level->enemy[i].sprite.spritedata->collwidth < player->sprite.x) {
                                    continue; //The enemy is too far left
                                }
                            }

                            //Y
                            if (level->enemy[i].sprite.y < player->sprite.y) { //The enemy is above
                                if (level->enemy[i].sprite.y <= player->sprite.y - 10) {
                                    continue;
                                }
                            } else { //The enemy is below
                                if (level->enemy[i].sprite.y - level->enemy[i].sprite.spritedata->collheight - 1 >= player->sprite.y) {
                                    continue;
                                }
                            }

                            if (level->enemy[i].sprite.number >= FIRST_NMI) {
                                diffX = player->sprite.x - level->enemy[i].sprite.x;
                                if (level->enemy[i].sprite.flipped) {
                                    diffX = 0 - diffX;
                                }
                                if (diffX < 0) {
                                    continue;
                                }
                            }

                            FX_START(9); //Sound effect
                            FUME_FLAG = 0;
                            level->enemy[i].sprite.speedY = 0;
                            level->enemy[i].sprite.speedX = 0;
                            GRAVITY_FLAG = 4;
                            player->sprite2.flipped = level->enemy[i].sprite.flipped;
                            player->sprite2.flash = level->enemy[i].sprite.flash;
                            player->sprite2.visible = level->enemy[i].sprite.visible;
                            updatesprite(level, &(player->sprite2), level->enemy[i].carry_sprite, false);
                            level->enemy[i].sprite.enabled = false;
                            CARRY_FLAG = true;
                            SEECHOC_FLAG = 0;
                            player->sprite2.y = player->sprite.y - 4;
                            if (player->sprite.flipped) {
                                player->sprite2.x = player->sprite.x - 10;
                            } else {
                                player->sprite2.x = player->sprite.x + 12;
                            }

                            break;
                        } //for loop, enemy
                    } //condition (!CARRY_FLAG), check for enemy pickup
                } //condition (!CARRY_FLAG), check for object/enemy pickup
            } //condition ((ACTION_TIMER == 1) && (CARRY_FLAG)), 
        } //condition (POSEREADY_FLAG == 0)
        POSEREADY_FLAG = true;
        break;

    case 8:
    case 24:
        //Throw
        NEW_FORM(player, action);
        GET_IMAGE(level);
        DECELERATION(player);
        if (CARRY_FLAG) {
            if (!keystate[KEY_UP] && !keystate[KEY_JUMP]) { //Ordinary throw
                speedX = 0x0E * 16;
                speedY = 0;
                if (player->sprite.flipped) {
                    speedX = 0 - speedX;
                }
                player->sprite2.y = player->sprite.y - 16;
            } else { //Throw up
                speedX = 0;
                speedY = 0 - 0x0A * 16;
            }
            if (speedY != 0) {
                //Throw up
                object = FORCE_POSE(level);
                if (object != NULL) {
                    object->sprite.speedY = speedY;
                    object->sprite.speedX = speedX - (speedX >> 2);
                }
            } else {
                if (player->sprite2.number < FIRST_NMI) { //level->objectdata can only be tested against an object sprite
                    if (level->objectdata[player->sprite2.number - FIRST_OBJET]->gravity) {
                        //Gravity throw
                        object = FORCE_POSE(level);
                        if (object != NULL) {
                            object->sprite.speedY = speedY;
                            object->sprite.speedX = speedX - (speedX >> 2);
                        }
                    } else { //Ordinary throw
                        DROP_FLAG = true;
                        player->sprite2.speedX = speedX;
                        player->sprite2.speedY = speedY;
                        FX_START(3); //Sound effect
                    }
                } else { //Ordinary throw
                    DROP_FLAG = true;
                    player->sprite2.speedX = speedX;
                    player->sprite2.speedY = speedY;
                    FX_START(3); //Sound effect
                }
            }
            updatesprite(level, &(player->sprite), 10, true); //The same as in free fall
            player->sprite.flipped = (SENSX < 0);
            CARRY_FLAG = false;
        }
        break;

    //case 9: same as case 0

    case 10:
        //Silent walk?
        XACCELERATION(player, (MAX_X - 1) * 16);
        NEW_FORM(player, action);
        GET_IMAGE(level);
        break;

    case 11:
        //Headache
        player->sprite.speedX = 0;
        NEW_FORM(player, action);
        GET_IMAGE(level);
        break;

    case 12:
    case 13:
    case 28:
    case 29:
        //Hit
        YACCELERATION(player, MAX_Y * 16);
        NEW_FORM(player, action);
        GET_IMAGE(level);
        break;

    //case 14, 15: //No action

    //case 16: same as case 0

    //case 17: same as case 1

    //case 18: same as case 2

    //case 19: same as case 1

    //case 20: //No action

    case 21:
        //Kneestand with box
        NEW_FORM(player, action);
        GET_IMAGE(level);
        DECELERATION(player);
        break;

    //case 22: same as case 6

    //case 23: same as case 7

    //case 24: same as case 8

    //case 25, 26: //No action

    case 27:
        //Headache (c)
        FORCE_POSE(level);
        player->sprite.speedX = 0;
        NEW_FORM(player, action);
        GET_IMAGE(level);
        break;

    //case 28, 29: same sa case 12

    }

}

void DECELERATION(TITUS_player *player) {
    //Stop acceleration
    uint8 friction = (3 * 4) >> player->GLISSE;
    int16 speed;
    if (player->sprite.speedX < 0) {
        speed = player->sprite.speedX + friction;
        if (speed > 0) {
            speed = 0;
        }
    } else {
        speed = player->sprite.speedX - friction;
        if (speed < 0) {
            speed = 0;
        }
    }
    player->sprite.speedX = speed;
}

static int NEW_FORM(TITUS_player *player, uint8 action) {
    //if the order is changed, change player animation
    if ((LAST_ORDER != action) || (player->sprite.animation == NULL)) {
        LAST_ORDER = action;
        player->sprite.animation = anim_player[action];
    }
}

static int GET_IMAGE(TITUS_level *level) {
    TITUS_player *player = &(level->player);
    //animate the player sprite
    int16 frame = *(player->sprite.animation);
    if (frame < 0) { //frame is a negative number, telling how many bytes to jump back
        player->sprite.animation += (frame / 2); //jump back to first frame of animation
        frame = *(player->sprite.animation);
        
    }
    updatesprite(level, &(player->sprite), frame, true);
    player->sprite.flipped = (SENSX < 0);
    player->sprite.animation++; //Advance in the animation
}

static int YACCELERATION_NEG(TITUS_player *player, int16 maxspeed) {
    //Accelerate upwards
    maxspeed = 0 - maxspeed; //maxspeed is negative
    int16 speed = player->sprite.speedY - 32;
    if (speed >= maxspeed) {
        speed = maxspeed;
    }
    player->sprite.speedY = speed;

}

int16 add_carry() {
    if (CARRY_FLAG) {
        return 16;
    } else {
        return 0;
    }
}

void COLLISION_TRP(TITUS_level *level) {
    //Player versus elevators
    //Change player's location according to the elevator
    uint8 i;
    TITUS_player *player = &(level->player);
    TITUS_elevator *elevator = level->elevator;
    if ((player->sprite.speedY >= 0) && (CROSS_FLAG == 0)) {
        for (i = 0; i < level->elevatorcount; i++) {
            //Quick test
            if (!(elevator[i].enabled) ||
              !(elevator[i].sprite.visible) ||
              (abs(elevator[i].sprite.x - player->sprite.x) >= 64) ||
              (abs(elevator[i].sprite.y - player->sprite.y) >= 16)) {
                continue;
            }

            //Real test
            if (player->sprite.x - level->spritedata[0]->refwidth < elevator[i].sprite.x) { //The elevator is right
                if (player->sprite.x - level->spritedata[0]->refwidth + level->spritedata[0]->collwidth <= elevator[i].sprite.x) { //player->sprite must be 0
                    continue; //The elevator is too far right
                }
            } else { //The elevator is left
                if (player->sprite.x - level->spritedata[0]->refwidth >= elevator[i].sprite.x + elevator[i].sprite.spritedata->collwidth) {
                    continue; //The elevator is too far left
                }
            }

            if (player->sprite.y - 6 < elevator[i].sprite.y) { //The elevator is below
                if (player->sprite.y - 6 + 8 <= elevator[i].sprite.y) {
                    continue; //The elevator is too far below
                }
            } else { //The elevator is above
                if (player->sprite.y - 6 >= elevator[i].sprite.y + elevator[i].sprite.spritedata->collheight) {
                    continue; //The elevator is too far above
                }
            }

            //Skip fall-through-tile action (ACTION_TIMER == 15)
            if (ACTION_TIMER == 14) {
                ACTION_TIMER = 16;
            }

            YFALL = 0;
            player->sprite.y = elevator[i].sprite.y;

            player->sprite.speedY = 0;
            subto0(&(SAUT_FLAG));
            SAUT_COUNT = 0;
            YFALL = 2;

            player->sprite.x += elevator[i].sprite.speedX;
            if (elevator[i].sprite.speedY > 0) { //Going down
                player->sprite.y += elevator[i].sprite.speedY;
            }
            return;
        }
    }
}

void COLLISION_OBJET(TITUS_level *level) {
    //Player versus objects
    //Collision, spring state, speed up carpet/scooter/skateboard, bounce bouncy objects
    TITUS_player *player = &(level->player);
    TITUS_object *off_object;
    if (player->sprite.speedY < 0) {
        return;
    }
    //Collision with a sprite
    if (!(SPRITES_VS_SPRITES(level, &(player->sprite), &(level->spritedata[0]), &off_object))) { //check if player stands on an object, use sprite[0] (rest) as collision size (first player tile)
        return;
    }
    player->sprite.y = off_object->sprite.y - off_object->sprite.spritedata->collheight;
    //If the foot is placed on a spring, it must be soft!
    if ((off_object->sprite.number == FIRST_OBJET + 24) || (off_object->sprite.number == FIRST_OBJET + 25)) {
        off_object->sprite.UNDER = off_object->sprite.UNDER | 0x02;
        off_object->sprite.ONTOP = &(player->sprite);
    }
    //If we jump on the flying carpet, let it fly
    if ((off_object->sprite.number == FIRST_OBJET + 21) || (off_object->sprite.number == FIRST_OBJET + 22)) { //Flying carpet
        if (!(player->sprite.flipped)) {
            off_object->sprite.speedX = 6 * 16;
        } else {
            off_object->sprite.speedX = 0 - 6 * 16;
        }
        off_object->sprite.flipped = player->sprite.flipped;
        GRAVITY_FLAG = 4;
        TAPISWAIT_FLAG = 0;
    } else if ((ACTION_TIMER > 10) && //delay
      ((LAST_ORDER & 0x0F) == 0) && //Player rests
      (player->sprite.speedY == 0) && //stable Y
      ((off_object->sprite.number == 83) || //scooter
      (off_object->sprite.number == 94))) { //skateboard

        //If you put your foot on a scooter or a skateboard
        if (!(player->sprite.flipped)) {
            off_object->sprite.speedX = 16 * 3;
        } else {
            off_object->sprite.speedX = 0 - 16 * 3;
        }
        off_object->sprite.flipped = player->sprite.flipped;
        GRAVITY_FLAG = 4;
    }

    if (off_object->sprite.speedX < 0) {
        player->sprite.speedX = off_object->sprite.speedX;
    } else if (off_object->sprite.speedX > 0) {
        player->sprite.speedX = off_object->sprite.speedX + 16;
    }

    //If we want to CROSS (cross) it does not bounce
    if ((CROSS_FLAG == 0) && //No long kneestand
      (player->sprite.speedY > (16 * 3)) &&
      (off_object->objectdata->bounce)) {
        //Bounce on a ball if no long kneestand (down key)
        if (keystate[KEY_DOWN]) {
            player->sprite.speedY = 0;
        } else {
            if (keystate[KEY_UP] || keystate[KEY_JUMP]) {
                player->sprite.speedY += 16 * 3; //increase speed
            } else {
                player->sprite.speedY -= 16; //reduce speed
            }
            player->sprite.speedY = 0 - player->sprite.speedY;
            if (player->sprite.speedY > 0) {
                player->sprite.speedY = 0;
            }
        }
        ACTION_TIMER = 0;

        //If the ball lies on the ground
        if (off_object->sprite.speedY == 0) {
            FX_START(12); //Sound effect
            off_object->sprite.speedY = 0 - player->sprite.speedY;
            off_object->sprite.y -= off_object->sprite.speedY >> 4;
            GRAVITY_FLAG = 4;
        }
    } else {
        if (off_object->sprite.speedY != 0) {
            player->sprite.speedY = off_object->sprite.speedY;
        } else {
            player->sprite.speedY = 0;
        }
        subto0(&(SAUT_FLAG));
        SAUT_COUNT = 0;
        YFALL = 2;
    }
}
