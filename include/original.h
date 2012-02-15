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

/* original.h
 * Contains data from the original game
 */

#ifndef ORIGINAL_H
#define ORIGINAL_H

#include "SDL/SDL.h"
#include "definitions.h"

#define SPRITECOUNT 356
#define ANIM_PLAYER_MAX 15
#define ANIM_PLAYER_COUNT 30
#define ORIG_ANIM_MAX 20
#define ORIG_OBJECT_COUNT 71
#define ORIG_NMI_COUNT 230
#define ORIG_LEVEL_COUNT 20
#define NMI_ANIM_TABLE_COUNT 1758

int initoriginal();
int16 getlevelid(int16 levelnumber);

SDL_Color orig_palette_colour[16];
SDL_Color orig_palette_level_colour[16];
SDL_Color orig_palette_font_colour[16];

uint8 spritewidth[SPRITECOUNT];
uint8 spriteheight[SPRITECOUNT];
uint8 spritecollwidth[SPRITECOUNT];
uint8 spritecollheight[SPRITECOUNT];
uint8 spriterefwidth[SPRITECOUNT];
uint8 spriterefheight[SPRITECOUNT];

int16 anim_player[ANIM_PLAYER_COUNT][ANIM_PLAYER_MAX];
int16 anim_enemy[NMI_ANIM_TABLE_COUNT];
uint8 NMI_POWER[ORIG_LEVEL_COUNT];
uint8 LEVEL_MUSIC[ORIG_LEVEL_COUNT];

int16 anim_zoubida[ORIG_ANIM_MAX];
int16 anim_moktar[ORIG_ANIM_MAX];
int16 anim_smoke[ORIG_ANIM_MAX];
int16 COEUR_POS[ORIG_ANIM_MAX * 2];

uint8 object_maxspeedY[ORIG_OBJECT_COUNT];
bool object_support[ORIG_OBJECT_COUNT]; //not support/support
bool object_bounce[ORIG_OBJECT_COUNT]; //not bounce/bounce against floor + player bounces (ball, all spring, yellow stone, squeezed ball, skateboard)
bool object_gravity[ORIG_OBJECT_COUNT]; //no gravity on throw/gravity (ball, all carpet, trolley, squeezed ball, garbage, grey stone, scooter, yellow bricks between the statues, skateboard, cage)
bool object_droptobottom[ORIG_OBJECT_COUNT]; //on drop, lands on ground/continue below ground(cave spikes, rolling rock, ambolt, safe, dead man with helicopter)
bool object_no_damage[ORIG_OBJECT_COUNT]; //weapon/not weapon(cage)

#endif
