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

/* level.c
 * Contains level functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL2/SDL.h"
#include "sqz.h"
#include "settings.h"
#include "fonts.h"
#include "window.h"
#include "engine.h"
#include "tituserror.h"
#include "original.h"
#include "sprites.h"
#include "level.h"
#include "globals.h"

unsigned int loaduint16(unsigned char c1, unsigned char c2);
int loadint16(unsigned char c1, unsigned char c2);

int loadlevel(TITUS_level *level, unsigned char *leveldata, int leveldatasize, TITUS_spritedata **spritedata, TITUS_spritecache *spritecache, TITUS_objectdata **objectdata){
    int i;
    int j;
    unsigned int offset;
    unsigned int pre_size;

    level->player.inithp = 16;
    level->player.cageX = 0;
    level->player.cageY = 0;

    //Pre-malloc zeroing, to avoid overflow during a freelevel call in case of this function returns an error
    level->height = 0;
    level->objectcount = 0;
    level->enemycount = 0;
    level->elevatorcount = 0;
/*
    level->player.sprite.buffer = NULL;
    level->player.sprite2.buffer = NULL;
    level->player.sprite3.buffer = NULL;
*/
    pre_size = (leveldatasize - 35828) >> 8; //level->height
    level->width = 256;

    level->tilemap = (unsigned char **)SDL_malloc(sizeof(unsigned char *) * pre_size);
    if (level->tilemap == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    for (i = 0; i < pre_size; i++) {
        level->tilemap[i] = (unsigned char *)SDL_malloc(sizeof(unsigned char) * level->width);
        if (level->tilemap[i] == NULL) {
            sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
            return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
        }
        level->height++;
        for (j = 0; j < level->width; j++)
            level->tilemap[i][j] = leveldata[i * level->width + j];
    }

    level->pixelformat->palette->colors[14].r = orig_palette_level_colour[level->levelnumber].r;
    level->pixelformat->palette->colors[14].g = orig_palette_level_colour[level->levelnumber].g;
    level->pixelformat->palette->colors[14].b = orig_palette_level_colour[level->levelnumber].b;

    offset = level->height * level->width;
    j = 256; //j is used for "last tile with animation flag"
    for (i = 0; i < 256; i++) {
        level->tile[i].tiledata = SDL_LoadTile(leveldata, offset + i * 128, level->pixelformat);
        level->tile[i].current = i;
        level->tile[i].horizflag = leveldata[offset + 32768 + i] & 0xFF;
        level->tile[i].floorflag = leveldata[offset + 32768 + 256 + i] & 0xFF;
        level->tile[i].ceilflag = leveldata[offset + 32768 + 512 + i] & 0x7F;

        level->tile[i].animated = true;
        level->tile[i].animation[0] = i;
        if (j == i - 1) { //Check if this is the second tile after animation flag
            level->tile[i].animation[1] = i + 1;
            level->tile[i].animation[2] = i - 1;
            PERMUT_FLAG = true;
        } else if (j == i - 2) { //Check if this is the third tile after animation flag
            level->tile[i].animation[1] = i - 2;
            level->tile[i].animation[2] = i - 1;
            PERMUT_FLAG = true;
        } else if ((leveldata[offset + 32768 + 512 + i] & 0x80) == 0x80) { //Animation flag
            level->tile[i].animation[1] = i + 1;
            level->tile[i].animation[2] = i + 2;
            j = i;
            PERMUT_FLAG = true;
        } else {
            level->tile[i].animation[1] = i;
            level->tile[i].animation[2] = i;
            level->tile[i].animated = false;
        }
    }
    level->spritedata = spritedata;
    level->spritecache = spritecache;
    level->objectdata = objectdata;

    level->player.initX = loadint16(leveldata[level->height * level->width + 33779], leveldata[level->height * level->width + 33778]);
    level->player.initY = loadint16(leveldata[level->height * level->width + 33781], leveldata[level->height * level->width + 33780]);

    level->finishX = loadint16(leveldata[level->height * level->width + 35825], leveldata[level->height * level->width + 35824]);
    level->finishY = loadint16(leveldata[level->height * level->width + 35827], leveldata[level->height * level->width + 35826]);

    pre_size = 40; //level->objectcount

    level->object = (TITUS_object *)SDL_malloc(sizeof(TITUS_object) * pre_size);
    if (level->object == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    level->objectcount = pre_size;
    offset = level->height * level->width + 33536;
    for (i = 0; i < level->objectcount; i++) {
        //level->object[i].sprite.buffer = NULL;
        level->object[i].initsprite = loaduint16(leveldata[offset + i * 6 + 1], leveldata[offset + i * 6 + 0]);
        level->object[i].init_enabled = (level->object[i].initsprite != 0xFFFF);
        if (level->object[i].init_enabled) {
            level->object[i].initX = loadint16(leveldata[offset + i * 6 + 3], leveldata[offset + i * 6 + 2]);
            level->object[i].initY = loadint16(leveldata[offset + i * 6 + 5], leveldata[offset + i * 6 + 4]);
        }
    }

    pre_size = 50; //level->enemycount

    level->enemy = (TITUS_enemy *)SDL_malloc(sizeof(TITUS_enemy) * pre_size);
    if (level->enemy == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    level->enemycount = pre_size;
    offset = level->height * level->width + 33782;
    for (i = 0; i < level->enemycount; i++) {
        //level->enemy[i].sprite.buffer = NULL;
        level->enemy[i].initspeedY = 0;
        level->enemy[i].initsprite = loaduint16(leveldata[offset + 5], leveldata[offset + 4]);
        level->enemy[i].init_enabled = (level->enemy[i].initsprite != 0xFFFF);
        level->enemy[i].power = 0;
        level->enemy[i].walkspeedX = 0;
        if (level->enemy[i].init_enabled) {
            level->enemy[i].initspeedY = 0;
            level->enemy[i].initX = loadint16(leveldata[offset + 1], leveldata[offset + 0]);
            level->enemy[i].initY = loadint16(leveldata[offset + 3], leveldata[offset + 2]);
            level->enemy[i].type = loaduint16(leveldata[offset + 7], leveldata[offset + 6]) & 0x1FFF;
            level->enemy[i].initspeedX = loadint16(leveldata[offset + 9], leveldata[offset + 8]);
            level->enemy[i].power = loadint16(leveldata[offset + 13], leveldata[offset + 12]);

            switch (level->enemy[i].type) {
            case 0: //Noclip walk
            case 1: //Noclip walk
                level->enemy[i].centerX = loadint16(leveldata[offset + 16], leveldata[offset + 15]);
                level->enemy[i].rangeX = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                break;
            case 2: //Shoot
                level->enemy[i].delay = (unsigned int)leveldata[offset + 16] & 0xFF;
                level->enemy[i].rangeX = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                level->enemy[i].direction = (level->enemy[i].rangeX >> 14) & 0x0003;
                level->enemy[i].rangeX = level->enemy[i].rangeX & 0x3FFF;
                break;
            case 3: //Noclip walk, jump to player
            case 4: //Noclip walk, jump to player
                level->enemy[i].centerX = loadint16(leveldata[offset + 16], leveldata[offset + 15]);
                level->enemy[i].rangeX = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                level->enemy[i].rangeY = (unsigned int)leveldata[offset + 19] & 0xFF;
                break;
            case 5: //Noclip walk, move to player
            case 6: //Noclip walk, move to player
                level->enemy[i].centerX = loadint16(leveldata[offset + 16], leveldata[offset + 15]);
                level->enemy[i].rangeX = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                level->enemy[i].rangeY = (unsigned int)leveldata[offset + 19] & 0xFF;
                break;
            case 7: //Gravity walk, hit when near
                level->enemy[i].walkspeedX = (uint8)leveldata[offset + 19] & 0xFF;
                level->enemy[i].rangeX = loaduint16(leveldata[offset + 24], leveldata[offset + 23]);
                break;
            case 8: //Gravity walk when off-screen
                level->enemy[i].walkspeedX = (uint8)leveldata[offset + 19] & 0xFF;
                break;
            case 9: //Walk and periodically pop-up
                level->enemy[i].walkspeedX = (uint8)leveldata[offset + 19] & 0xFF;
                level->enemy[i].rangeX = loaduint16(leveldata[offset + 24], leveldata[offset + 23]);
                break;
            case 10: //Alert when near, walk when nearer
                level->enemy[i].walkspeedX = (uint8)leveldata[offset + 19] & 0xFF;
                level->enemy[i].rangeX = loaduint16(leveldata[offset + 24], leveldata[offset + 23]);
                break;
            case 11: //Walk and shoot
                level->enemy[i].walkspeedX = (uint8)leveldata[offset + 19] & 0xFF;
                level->enemy[i].rangeX = loaduint16(leveldata[offset + 24], leveldata[offset + 23]);
                break;
            case 12: //Jump (immortal)
                level->enemy[i].rangeY = loaduint16(leveldata[offset + 16], leveldata[offset + 15]);;
                level->enemy[i].delay = (unsigned int)leveldata[offset + 19] & 0xFF;
                break;
            case 13: //Bounce
                level->enemy[i].delay = (unsigned int)leveldata[offset + 20] & 0xFF;
                level->enemy[i].rangeX = loaduint16(leveldata[offset + 24], leveldata[offset + 23]);
                break;
            case 14: //Gravity walk when off-screen (immortal)
                level->enemy[i].walkspeedX = (uint8)leveldata[offset + 19] & 0xFF;
                break;
            case 15: //Nothing (immortal)
                break;
            case 16: //Nothing
                break;
            case 17: //Drop (immortal)
                level->enemy[i].rangeX = loaduint16(leveldata[offset + 16], leveldata[offset + 15]);
                level->enemy[i].delay = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                level->enemy[i].rangeY = loaduint16(leveldata[offset + 22], leveldata[offset + 21]);
                break;
            case 18: //Drop (immortal)
                level->enemy[i].rangeX = loaduint16(leveldata[offset + 16], leveldata[offset + 15]);
                level->enemy[i].rangeY = loaduint16(leveldata[offset + 18], leveldata[offset + 17]);
                level->enemy[i].initspeedY = leveldata[offset + 19];
                break;
            }
        } else {
            level->enemy[i].sprite.enabled = false;
        }
        offset += 26;
    }

    level->bonuscount = 100;

    level->bonus = (TITUS_bonus *)SDL_malloc(sizeof(TITUS_bonus) * level->bonuscount);
    if (level->bonus == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }

    offset = level->height * level->width + 35082;
    for (i = 0; i < level->bonuscount; i++) {
        level->bonus[i].x = leveldata[offset + 2];
        level->bonus[i].y = leveldata[offset + 3];
        level->bonus[i].exists = ((level->bonus[i].x != 0xFF) & (level->bonus[i].y != 0xFF));
        if (level->bonus[i].exists) {
            level->bonus[i].bonustile = leveldata[offset];
            level->bonus[i].replacetile = leveldata[offset + 1];
            level->tilemap[level->bonus[i].y][level->bonus[i].x] = leveldata[offset + 0]; //Overwrite the actual tile
        }
        offset += 4;
    }

    level->gatecount = 20;

    level->gate = (TITUS_gate *)SDL_malloc(sizeof(TITUS_gate) * level->gatecount);
    if (level->gate == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }

    offset = level->height * level->width + 35484;
    for (i = 0; i < level->gatecount; i++) {
        level->gate[i].entranceY = (unsigned int)leveldata[offset + 1] & 0xFF;
        level->gate[i].exists = (level->gate[i].entranceY != 0xFF);
        if (level->gate[i].exists) {
            level->gate[i].entranceX = (unsigned int)leveldata[offset + 0] & 0xFF;
            level->gate[i].screenX = (unsigned int)leveldata[offset + 2] & 0xFF;
            level->gate[i].screenY = (unsigned int)leveldata[offset + 3] & 0xFF;
            level->gate[i].exitX = (unsigned int)leveldata[offset + 4] & 0xFF;
            level->gate[i].exitY = (unsigned int)leveldata[offset + 5] & 0xFF;
            level->gate[i].noscroll = ((uint8)leveldata[offset + 6] != 0);
        }
        offset += 7;
    }

    pre_size = 10; //level->elevatorcount

    level->elevator = (TITUS_elevator *)SDL_malloc(sizeof(TITUS_elevator) * pre_size);
    if (level->elevator == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    level->elevatorcount = pre_size;
    offset = level->height * level->width + 35624;
    for (i = 0; i < level->elevatorcount; i++) {
        //level->elevator[i].sprite.buffer = NULL;
        level->elevator[i].sprite.enabled = false;
        level->elevator[i].initsprite = loaduint16(leveldata[offset + 5], leveldata[offset + 4]);
        level->elevator[i].initspeedX = 0;
        level->elevator[i].initspeedY = 0;
        level->elevator[i].initX = loadint16(leveldata[offset + 13], leveldata[offset + 12]);
        level->elevator[i].initY = loadint16(leveldata[offset + 15], leveldata[offset + 14]);
        j = leveldata[offset + 7]; //Speed
        level->elevator[i].init_enabled = ((level->elevator[i].initsprite != 0xFFFF) && (j < 8) && (level->elevator[i].initX >= -16) && (level->elevator[i].initY >= 0));
        level->elevator[i].enabled = level->elevator[i].init_enabled;

        if (level->elevator[i].enabled) {
            level->elevator[i].range = loaduint16(leveldata[offset + 11], leveldata[offset + 10]);
            level->elevator[i].init_direction = (unsigned int)leveldata[offset + 16] & 0xFF;
            if ((level->elevator[i].init_direction == 0) || (level->elevator[i].init_direction == 3)) { //Up or left
                j = 0 - j;
            }
            if ((level->elevator[i].init_direction == 0) || //up
              (level->elevator[i].init_direction == 2)) { //down
                level->elevator[i].initspeedY = j;
            } else {
                level->elevator[i].initspeedX = j;
            }
        }
        offset += 20;
    }

    offset = level->height * level->width + 33776;
    ALTITUDE_ZERO = loadint16(leveldata[offset + 1], leveldata[offset + 0]); // + 12;
    offset = level->height * level->width + 35482;
    XLIMIT = loadint16(leveldata[offset + 1], leveldata[offset + 0]); // + 20;
    for (i = 0; i < SPRITECOUNT; i++) {
        copypixelformat(level->spritedata[i]->data->format, level->pixelformat);
    }

    pre_size = 4; //level->trashcount

    level->trash = (TITUS_sprite *)SDL_malloc(sizeof(TITUS_sprite) * pre_size);
    if (level->trash == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize level!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    level->trashcount = pre_size;
    /*
    for (i = 0; i < level->trashcount; i++) {
        level->trash[i].buffer = NULL;
    }
*/


    return (0);
}

int freelevel(TITUS_level *level){
    int i;

    for (i = 0; i < level->height; i++)
        free (level->tilemap[i]);

    free (level->tilemap);

    for (i = 0; i < 256; i++)
        SDL_FreeSurface(level->tile[i].tiledata);
/*
    SDL_FreeSurface(level->player.sprite.buffer);
    SDL_FreeSurface(level->player.sprite2.buffer);
    SDL_FreeSurface(level->player.sprite3.buffer);

    for (i = 0; i < level->objectcount; i++) {
        if (level->object[i].sprite.buffer != NULL)
            SDL_FreeSurface(level->object[i].sprite.buffer);
    }
*/
    free (level->object);
/*
    for (i = 0; i < level->enemycount; i++)
        if (level->enemy[i].sprite.buffer != NULL)
            SDL_FreeSurface(level->enemy[i].sprite.buffer);
*/
    free (level->enemy);

    free (level->bonus);

    free (level->gate);
/*
    for (i = 0; i < level->elevatorcount; i++)
        if (level->elevator[i].sprite.buffer != NULL)
            SDL_FreeSurface(level->elevator[i].sprite.buffer);
*/
    free (level->elevator);
/*
    for (i = 0; i < level->trashcount; i++) {
        if (level->trash[i].buffer != NULL)
            SDL_FreeSurface(level->trash[i].buffer);
    }
*/
    free (level->trash);

    return (0);
}

uint8 get_horizflag(TITUS_level *level, int16 tileY, int16 tileX) {
    if ((tileX < 0) ||
      (tileX >= level->width)) {
        return HFLAG_WALL;
    } else if ((tileY < 0) ||
      (tileY >= level->height)) {
        return HFLAG_NOWALL;
    } else {
        return level->tile[level->tilemap[tileY][tileX]].horizflag;
    }
}

uint8 get_floorflag(TITUS_level *level, int16 tileY, int16 tileX) {
    if ((tileX < 0) ||
      (tileX >= level->width)) {
        return FFLAG_FLOOR;
    } else if ((tileY < 0) ||
      (tileY >= level->height)) {
        return FFLAG_NOFLOOR;
    } else {
        return level->tile[level->tilemap[tileY][tileX]].floorflag;
    }
}

uint8 get_ceilflag(TITUS_level *level, int16 tileY, int16 tileX) {
    if ((tileY < 0) ||
      (tileY >= level->height) ||
      (tileX < 0) ||
      (tileX >= level->width)) {
        return CFLAG_NOCEILING;
    } else {
        return level->tile[level->tilemap[tileY][tileX]].ceilflag;
    }
}

unsigned int loaduint16(unsigned char c1, unsigned char c2){
    return (((unsigned int)c1 * 256) & 0xFF00) + (unsigned int)c2;
}

int loadint16(unsigned char c1, unsigned char c2){
    short int tmpint = ((short int)c1 << 8) + (short int)c2;
    return (int)tmpint;
}
