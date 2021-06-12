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

#pragma once

#include "SDL2/SDL.h"
#include "definitions.h"

#define SPRITECOUNT 356
#define ANIM_PLAYER_MAX 15
#define ANIM_PLAYER_MAX_MOKTAR 12
#define ANIM_PLAYER_COUNT 30
#define ORIG_ANIM_MAX 20
#define ORIG_OBJECT_COUNT 71
#define ORIG_NMI_COUNT 230
#define ORIG_LEVEL_COUNT 20
#define NMI_ANIM_TABLE_COUNT 879 //1758

int initoriginal();
int16 getlevelid(int16 levelnumber);

extern SDL_Color orig_palette_colour[16];
extern SDL_Color orig_palette_level_colour[16];
extern SDL_Color orig_palette_font_colour[16];

extern uint8 spritewidth[SPRITECOUNT];
extern uint8 spriteheight[SPRITECOUNT];
extern uint8 spritecollwidth[SPRITECOUNT];
extern uint8 spritecollheight[SPRITECOUNT];
extern uint8 spriterefwidth[SPRITECOUNT];
extern uint8 spriterefheight[SPRITECOUNT];

extern int16 anim_player[ANIM_PLAYER_COUNT][ANIM_PLAYER_MAX];
extern int16 anim_enemy[NMI_ANIM_TABLE_COUNT];
extern uint8 NMI_POWER[ORIG_LEVEL_COUNT];
extern uint8 LEVEL_MUSIC[ORIG_LEVEL_COUNT];

extern int16 anim_zoubida[ORIG_ANIM_MAX];
extern int16 anim_moktar[ORIG_ANIM_MAX];
extern int16 anim_smoke[ORIG_ANIM_MAX];
extern int16 COEUR_POS[ORIG_ANIM_MAX * 2];

extern uint8 object_maxspeedY[ORIG_OBJECT_COUNT];
extern bool object_support[ORIG_OBJECT_COUNT]; //not support/support
extern bool object_bounce[ORIG_OBJECT_COUNT]; //not bounce/bounce against floor + player bounces (ball, all spring, yellow stone, squeezed ball, skateboard)
extern bool object_gravity[ORIG_OBJECT_COUNT]; //no gravity on throw/gravity (ball, all carpet, trolley, squeezed ball, garbage, grey stone, scooter, yellow bricks between the statues, skateboard, cage)
extern bool object_droptobottom[ORIG_OBJECT_COUNT]; //on drop, lands on ground/continue below ground(cave spikes, rolling rock, ambolt, safe, dead man with helicopter)
extern bool object_no_damage[ORIG_OBJECT_COUNT]; //weapon/not weapon(cage)
