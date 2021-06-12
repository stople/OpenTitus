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

/* gates.c
 * Handles gates (kneestand) and checks if the player have completed the level
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "globals.h"
#include "backbuffer.h"
#include "sprites.h"
#include "settings.h"
#include "audio.h"
#include "gates.h"
#include "scroll.h"
#include "draw.h"

void copytiles(int16 destX, int16 destY, int16 width, int16 height);

void check_finish(TITUS_level *level) {
    TITUS_player *player = &(level->player);
    if (boss_alive) { //There is still a boss that needs to be killed!
        return;
    }
    if (level->levelid == 9) { //The level with a cage
        if ((level->player.sprite2.number != FIRST_OBJET + 26) &&
          (level->player.sprite2.number != FIRST_OBJET + 27)) {
            return;
        }
    }
    if (((player->sprite.x & 0xFFF0) != level->finishX) &&
      ((player->sprite.x & 0xFFF0) - 16 != level->finishX)) {
        return;
    }
    if (((player->sprite.y & 0xFFF0) != level->finishY) &&
      ((player->sprite.y & 0xFFF0) - 16 != level->finishY)) {
        return;
    }
    SELECT_MUSIC(4);
    WAIT_SONG();
    CLOSE_SCREEN();
    NEWLEVEL_FLAG = true;
}

void check_gates(TITUS_level *level) {
    TITUS_player *player = &(level->player);
    uint8 i;
    if ((CROSS_FLAG == 0) || //not kneestanding
      (NEWLEVEL_FLAG)) { //the player has finished the level
        return;
    }
    for (i = 0; i < level->gatecount; i++) {
        if ((level->gate[i].exists) &&
          (level->gate[i].entranceX == (player->sprite.x >> 4)) &&
          (level->gate[i].entranceY == (player->sprite.y >> 4))) {
            break;
        }
    }
    if (i == level->gatecount) {
        return; //No matching gate
    }
    player->sprite.speedX = 0;
    player->sprite.speedY = 0;
    CLOSE_SCREEN();
    uint8 orig_xlimit = XLIMIT;
    XLIMIT = level->width - screen_width;
    player->sprite.x = level->gate[i].exitX << 4;
    player->sprite.y = level->gate[i].exitY << 4;
    while (BITMAP_Y < level->gate[i].screenY) {
        D_SCROLL(level);
    }
    while (BITMAP_Y > level->gate[i].screenY) {
        U_SCROLL(level);
    }
    while (BITMAP_X < level->gate[i].screenX) {
        R_SCROLL(level);
    }
    while (BITMAP_X > level->gate[i].screenX) {
        L_SCROLL(level);
    }
    XLIMIT = orig_xlimit;
    NOSCROLL_FLAG = level->gate[i].noscroll;
    OPEN_SCREEN();
}

void CROSSING_GATE(TITUS_level *level) { //Check and handle level completion, and if the player does a kneestand on a secret entrance
    check_finish(level);
    check_gates(level);
}

void CLOSE_SCREEN() {
    SDL_Rect dest;
    uint8 step_count = 10;
    uint16 rwidth = 320; //TODO: make this global
    uint16 rheight = 192;
    uint16 incX = rwidth / (step_count * 2);  //16
    uint16 incY = rheight / (step_count * 2); //10
    uint8 i;
    for (i = 0; i < step_count; i++) {
        flip_screen(false); //quick flip TODO: move to other end of loop?

        //Clear top
        dest.x = 0;
        dest.y = 0;
        dest.w = screen_width * 16;
        dest.h = i * incY;
        SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0, 0, 0));

        //Clear left
        dest.x = 0;
        dest.y = 0;
        dest.w = i * incX;
        dest.h = screen_height * 16;
        SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0, 0, 0));

        //Clear bottom
        dest.x = 0;
        dest.y = rheight - (i * incY);
        dest.w = screen_width * 16;
        dest.h = i * incY;
        SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0, 0, 0));

        //Clear right
        dest.x = rwidth - (i * incX);
        dest.y = 0;
        dest.w = i * incX;
        dest.h = screen_height * 16;
        SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0, 0, 0));
    }
}


void OPEN_SCREEN() {
    SDL_Rect dest;
    int8 step_count = 10;
    int16 blockX = 320 / (step_count * 2);  //16 (320: resolution width)
    int16 blockY = 192 / (step_count * 2); //9 (192: resolution height)
    int8 i, j;
    TFR_SCREENM(); //Draw tiles on the backbuffer

    //BLACK_SCREEN
    dest.x = 0;
    dest.y = 0;
    dest.w = 320;
    dest.h = 192;
    SDL_FillRect(screen, &dest, SDL_MapRGB(screen->format, 0, 0, 0));

    j = step_count;
    for (i = 2; i <= step_count * 2; i += 2) {
        j--;
        flip_screen(false); //quick flip TODO: move to other end of loop?
        copytiles( j * blockX, j * blockY, (i * blockX) - blockX, blockY); //Upper tiles
        copytiles((j * blockX) + (i * blockX) - blockX, j * blockY, blockX, i * blockY); //Right tiles
        copytiles((j * blockX) + blockX, ((j + 1) * blockY) + (i * blockY) - blockY, (i * blockX) - blockX, blockY); //Bottom tiles
        copytiles( j * blockX, (j * blockY) + blockY, blockX, i * blockY); //Left tiles
    }
}

void copytiles(int16 destX, int16 destY, int16 width, int16 height) {
    SDL_Rect src, dest;
    int16 sepX = BITMAP_XM * 16;
    int16 sepY = BITMAP_YM * 16;
    int16 sepXi = (S_COLUMNS - BITMAP_XM) * 16;
    int16 sepYi = (S_LINES - BITMAP_YM) * 16;

    // Tile screen:  | Output screen:
    //               |
    // D | C         | A | B
    // -   -         | -   -
    // B | A         | C | D
    //
    // The screens are splitted in 4 parts by BITMAP_XM and BITMAP_YM
    // The code below will move the 4 rectangles with tiles to their right place on the output screen

    //Upper left on screen (A)
    if ((destX < sepXi) &&
      (destY < sepYi)) {
        src.x = sepX + destX;
        src.y = sepY + destY;
        dest.x = destX;
        dest.y = destY;
        src.w = width;
        src.h = height;
        if (destX + width > sepXi) { //Both A and B
            src.w = sepXi - destX;
        }
        if (destY + height > sepYi) { //Both A and C
            src.h = sepYi - destY;
        }
        SDL_BlitSurface(tilescreen, &src, screen, &dest);
    }

    //Upper right on screen (B)
    if ((destY < sepYi) &&
      (destX + width > sepXi)) {
        src.x = destX - sepXi;
        src.y = sepY + destY;
        src.w = width;
        src.h = height;
        dest.x = destX;
        dest.y = destY;
        if (destX < sepXi) { //Both B and A
            src.x = 0;
            src.w = width - (sepXi - destX);
            dest.x = sepXi;
        }
        if (destY + height > sepYi) { //Both B and D
            src.h = sepYi - destY;
        }
        SDL_BlitSurface(tilescreen, &src, screen, &dest);
    }

    //Lower left on screen (C)
    if ((destX < sepXi) &&
      (destY + height > sepYi)) {
        src.x = sepX + destX;
        src.y = destY - sepYi;
        src.w = width;
        src.h = height;
        dest.x = destX;
        dest.y = destY;
        if (destX + width > sepXi) { //Both C and D
            src.w = sepXi - destX;
        }
        if (destY < sepYi) { //Both C and A
            src.y = 0;
            src.h = height - (sepYi - destY);
            dest.y = sepYi;
        }
        SDL_BlitSurface(tilescreen, &src, screen, &dest);
    }

    //Lower right on screen (D)
    if (((destX + width) > sepXi) &&
      ((destY + height) > sepYi)) {
        src.x = destX - sepXi;
        src.y = destY - sepYi;
        src.w = width;
        src.h = height;
        dest.x = destX;
        dest.y = destY;
        if (destX < sepXi) { //Both D and C
            src.x = 0;
            src.w = width - (sepXi - destX);
            dest.x = sepXi;
        }
        if (destY < sepYi) { //Both D and B
            src.y = 0;
            src.h = height - (sepYi - destY);
            dest.y = sepYi;
        }
        SDL_BlitSurface(tilescreen, &src, screen, &dest);
    }
}
