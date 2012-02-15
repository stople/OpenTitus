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

/* engine.c
 * Engine functions
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "sqz.h"
#include "settings.h"
#include "fonts.h"
#include "backbuffer.h"
#include "engine.h"
#include "tituserror.h"
#include "original.h"
#include "sprites.h"
#include "level.h"
#include "keyboard.h"
#include "globals.h"
#include "player.h"
#include "definitions.h"
#include "scroll.h"
#include "draw.h"
#include "reset.h"
#include "gates.h"
#include "elevators.h"
#include "objects.h"
#include "enemies.h"

//Probably not the best way, but it works...
#define HAVE_CONFIG_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef AUDIO_ENABLED
#include "audio.h"
#endif

static int playlevel(TITUS_level *level);
static int displaylevel(TITUS_level *level);
static int movescreen(TITUS_level *level);
static int animate(TITUS_level *level);
static int movetitus(TITUS_level *level);
static int collision_detection_player(TITUS_level *level);
static int getpolarity(int number);
int gettick(int *tick, uint8 index);

int playtitus(int firstlevel){
    int startx, starty;
    int retval;

    TITUS_level level;
    unsigned char *leveldata;
    unsigned char *spritedata;
    TITUS_spritecache spritecache;
    TITUS_spritedata **sprites;
    uint16 sprite_count;
    TITUS_objectdata **objects;
    uint16 object_count;
    int leveldatasize;
    bool first;

    level.lives = 2;
    level.extrabonus = 0;

    retval = loadpixelformat(&(level.pixelformat));
    if (retval < 0) {
        freepixelformat(&(level.pixelformat));
        return retval;
    }

    tilescreen = SDL_CreateRGBSurface(SDL_SWSURFACE, 20 * 16, 12 * 16, 8, 0, 0, 0, 0);
    copypixelformat(tilescreen->format, level.pixelformat);

    retval = unSQZ(spritefile, &spritedata);
    if (retval < 0) {
        free (spritedata);
        freepixelformat(&(level.pixelformat));
        return retval;
    }
    retval = loadsprites(&sprites, spritedata, retval, level.pixelformat, &sprite_count);
    free (spritedata);
    if (retval < 0) {
        freesprites(&sprites, sprite_count);
        freepixelformat(&(level.pixelformat));
        return retval;
    }
    retval = initspritecache(&spritecache, 100, 3); //Cache size: 100 surfaces, 3 temporary
    if (retval < 0) {
        freespritecache(&spritecache);
        freesprites(&sprites, sprite_count);
        freepixelformat(&(level.pixelformat));
        return retval;
    }

    retval = loadobjects(&objects, &object_count);
    if (retval < 0) {
        freeobjects(&objects, object_count);
        freespritecache(&spritecache);
        freesprites(&sprites, sprite_count);
        freepixelformat(&(level.pixelformat));
        return retval;
    }

    for (level.levelnumber = firstlevel; level.levelnumber < levelcount; level.levelnumber++) {
        //strcpy (level.title, leveltitle[level.levelnumber]);
        level.levelid = getlevelid(level.levelnumber);
        retval = unSQZ(levelfiles[level.levelnumber], &leveldata);
        if (retval < 0) {
            free (leveldata);
            freeobjects(&objects, object_count);
            freespritecache(&spritecache);
            freesprites(&sprites, sprite_count);
            freepixelformat(&(level.pixelformat));
            return retval;
        }
        leveldatasize = retval;

        retval = loadlevel(&level, leveldata, leveldatasize, sprites, &(spritecache), objects);
        free (leveldata);
        if (retval < 0) {
            freelevel (&level);
            freeobjects(&objects, object_count);
            freespritecache(&spritecache);
            freesprites(&sprites, sprite_count);
            freepixelformat(&(level.pixelformat));
            return retval;
        }
        first = true;
        while (1) {
#ifdef AUDIO_ENABLED
            SELECT_MUSIC(0);
#endif
            CLEAR_DATA(&level);

            GODMODE = false;
            NOCLIP = false;
            DISPLAYLOOPTIME = false;

            retval = viewstatus(&level, first);
            first = false;
            if (retval < 0) {
                freelevel (&level);
                freeobjects(&objects, object_count);
				freespritecache(&spritecache);
                freesprites(&sprites, sprite_count);
                freepixelformat(&(level.pixelformat));
                return retval;
            }

            //retval = initlevel (&level);

#ifdef AUDIO_ENABLED
            //changemusic(modulelevelfile[modulelevel[level.levelnumber] - 1], modulelevelfileloop[modulelevel[level.levelnumber] - 1]);
            SELECT_MUSIC(LEVEL_MUSIC[level.levelid]);
#endif

            INIT_SCREENM(&level); //Todo: comment, DOCUMENTED! (reset_level_simplified)
            TFR_SCREENM(); //Draws tiles
            flip_screen(true); //Flip backbuffer

            
            retval = playlevel (&level);
            if (retval < 0) {
                freelevel (&level);
                freeobjects(&objects, object_count);
				freespritecache(&spritecache);
                freesprites(&sprites, sprite_count);
                freepixelformat(&(level.pixelformat));
                return retval;
            }

            if (NEWLEVEL_FLAG) {
                break;
            }
            if (RESETLEVEL_FLAG == 1) {
                if (level.lives == 0) {
                    GAMEOVER_FLAG = true;
                } else {
                    level.lives--;
                    death(&level);
                }
            }
            
            
            
            
            if (GAMEOVER_FLAG) {
                gameover(&level);
            
                freelevel (&level);
                freeobjects(&objects, object_count);
				freespritecache(&spritecache);
                freesprites(&sprites, sprite_count);
                freepixelformat(&(level.pixelformat));
                return 0;
            }
            /*
            if (level.player.globalstatus == PLAYER_GLOBAL_STATUS_DEAD) {
                if (level.lives > 0) {
                    level.player.globalstatus = PLAYER_GLOBAL_STATUS_GAMEOVER;
                } else {
                    level.lives--;
                }
            }

            if (level.player.globalstatus == PLAYER_GLOBAL_STATUS_GAMEOVER) {
            }
            */
            
        }

        freelevel (&level);

        if (retval < 0) {
            freeobjects(&objects, object_count);
            freespritecache(&spritecache);
            freesprites(&sprites, sprite_count);
            freepixelformat(&(level.pixelformat));
            return retval;
        }

    }
    freeobjects(&objects, object_count);
    freespritecache(&spritecache);
    freesprites(&sprites, sprite_count);
    freepixelformat(&(level.pixelformat));
    SDL_FreeSurface(tilescreen);
    if (game == 0) { //Titus
        retval = viewimage(titusfinishfile, titusfinishformat, 1, 0);
        if (retval < 0)
            return retval;
    }

    return (0);
}


#ifdef DEBUG_VERSION

//Debug version - equal to the ordinary main loop, will additionally measure the time used in each sub function
static int playlevel(TITUS_level *level) {
    int retval = 0;
    SDL_Event event;
    bool firstrun = true;
    int tick;
    do {
        if (!firstrun) {
            gettick(&tick, 12);
            DISPLAY_COUNT(level); //Draw energy to the backbuffer
            gettick(&tick, 13);
#ifdef AUDIO_ENABLED
            RETURN_MUSIC(level); //Restart music if the song is finished
#endif
            flip_screen(true);
        }
        firstrun = false;
        IMAGE_COUNTER = (IMAGE_COUNTER + 1) & 0x0FFF; //Cycle from 0 to 0x0FFF
        tick = SDL_GetTicks();
        MOVE_TRP(level); //Move elevators
        gettick(&tick, 0);
        move_objects(level); //Object gravity
        gettick(&tick, 1);
        retval = move_player(level); //Key input, update and move player, handle carried object and decrease timers
        if (retval == TITUS_ERROR_QUIT) {
            return retval;
        }
        gettick(&tick, 2);
        MOVE_NMI(level); //Move enemies
        gettick(&tick, 3);
        MOVE_TRASH(level); //Move enemy throwed objects
        gettick(&tick, 4);
        SET_NMI(level); //Handle enemies on the screen
        gettick(&tick, 5);
        CROSSING_GATE(level); //Check and handle level completion, and if the player does a kneestand on a secret entrance
        gettick(&tick, 6);
        SPRITES_ANIMATION(level); //Animate player and objects
        gettick(&tick, 7);
        scroll(level); //X- and Y-scrolling
        gettick(&tick, 8);
        BLOC_ANIMATION(level); //Animate tiles
        gettick(&tick, 9);
        TFR_SCREENM(); //Draws tiles on the backbuffer
        gettick(&tick, 10);
        DISPLAY_SPRITES(level); //Draws sprites on the backbuffer
        gettick(&tick, 11);
        retval = RESET_LEVEL(level); //Check terminate flags (finishlevel, gameover, death or theend)
        if (retval < 0) {
            return retval;
        }
    } while (retval == 0); //Exits the game loop if the previous function wants to
    return (0);
}

#else

//Ordinary main loop
static int playlevel(TITUS_level *level) {
    int retval = 0;
    SDL_Event event;
    bool firstrun = true;
    do {
        if (!firstrun) {
            DISPLAY_COUNT(level); //Draw energy to the backbuffer
#ifdef AUDIO_ENABLED
            RETURN_MUSIC(level); //Restart music if the song is finished
#endif
            flip_screen(true);
        }
        firstrun = false;
        IMAGE_COUNTER = (IMAGE_COUNTER + 1) & 0x0FFF; //Cycle from 0 to 0x0FFF
        MOVE_TRP(level); //Move elevators
        move_objects(level); //Object gravity
        retval = move_player(level); //Key input, update and move player, handle carried object and decrease timers
        if (retval == TITUS_ERROR_QUIT) {
            return retval;
        }
        MOVE_NMI(level); //Move enemies
        MOVE_TRASH(level); //Move enemy throwed objects
        SET_NMI(level); //Handle enemies on the screen
        CROSSING_GATE(level); //Check and handle level completion, and if the player does a kneestand on a secret entrance
        SPRITES_ANIMATION(level); //Animate player and objects
        scroll(level); //X- and Y-scrolling
        BLOC_ANIMATION(level); //Animate tiles
        TFR_SCREENM(); //Draws tiles on the backbuffer
        DISPLAY_SPRITES(level); //Draws sprites on the backbuffer
        retval = RESET_LEVEL(level); //Check terminate flags (finishlevel, gameover, death or theend)
        if (retval < 0) {
            return retval;
        }
    } while (retval == 0); //Exits the game loop if the previous function wants to
    return (0);
}

#endif


int gettick(int *tick, uint8 index) {
    int oldtick = *tick;
    *tick = SDL_GetTicks();
    //SUBTIME[index] = *tick - oldtick;
}

static int getpolarity(int number){
    if (number < 0)
        return (-1);
    if (number > 0)
        return (1);
    return (0);
}

int death(TITUS_level *level) {
    TITUS_player *player = &(level->player);
    int i;
#ifdef AUDIO_ENABLED
    SELECT_MUSIC(1);
#endif
    FORCE_POSE(level);
    updatesprite(level, &(player->sprite), 13, true); //Death
    player->sprite.speedY = 15;
    for (i = 0; i < 60; i++) {
        TFR_SCREENM();
        //TODO! GRAVITY();
        DISPLAY_SPRITES(level);
        flip_screen(true);
        player->sprite.speedY--;
        if (player->sprite.speedY < -16) {
            player->sprite.speedY = -16;
        }
        player->sprite.y -= player->sprite.speedY;
    }

#ifdef AUDIO_ENABLED
    WAIT_SONG();
    SELECT_MUSIC(0);
#endif
        
    /* TODO: remove because REPLACED
    SCREEN_1();
    //TODO: SELECT_MUSIC(LEVEL_MUSIC[FNAMEB]);
    INIT_SCREENM(level);
    return 3; //Do not break main loop
    */
    CLOSE_SCREEN();
}

int gameover(TITUS_level *level) {
    TITUS_player *player = &(level->player);
    int i, retval;
#ifdef AUDIO_ENABLED
    SELECT_MUSIC(2);
#endif
    updatesprite(level, &(player->sprite), 13, true); //Death
    updatesprite(level, &(player->sprite2), 333, true); //Game
    player->sprite2.x = (BITMAP_X << 4) - (120-2);
    player->sprite2.y = (BITMAP_Y << 4) + 100;
    //over
    updatesprite(level, &(player->sprite3), 334, true); //Over
    player->sprite3.x = (BITMAP_X << 4) + (320+120-2);
    player->sprite3.y = (BITMAP_Y << 4) + 100;
    for (i = 0; i < 31; i++) {
        TFR_SCREENM();
        DISPLAY_SPRITES(level);
        flip_screen(true);
        player->sprite2.x += 8;
        player->sprite3.x -= 8;
    }
    SCREEN_5C(); //Secret: display picture if LCtrl+LAlt+E is pressed
    retval = waitforbutton();
    if (retval < 0)
        return retval;

    fadeout();
}

int SCREEN_5C() {
    uint16 key;
    int retval;
    if (keystate[SDLK_LCTRL] && //LCtrl
      keystate[SDLK_LALT] && //LAlt
      keystate[SDLK_e]) { //E
        for (key = SDLK_FIRST; key <= SDLK_LAST; key++) {
            if (key == SDLK_LCTRL) continue;
            if (key == SDLK_LALT) continue;
            if (key == SDLK_e) continue;
            if (keystate[key]) return;
        }
        if (game == 0) { //Titus
            retval = viewimage(titusfinishfile, titusfinishformat, 1, 0);
            if (retval < 0)
                return retval;
        }
#ifdef AUDIO_ENABLED
        SELECT_MUSIC(9);
#endif
    }
}
