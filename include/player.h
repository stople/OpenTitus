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

/* player.h
 * Handles player movement and keyboard handling
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "SDL/SDL.h"
#include "level.h"

extern uint8 RESETLEVEL_FLAG;
extern uint8 BAR_FLAG; //timer for health bar
extern bool X_FLAG; //true if left or right key is pressed
extern bool Y_FLAG; //true if up or down key is pressed
extern uint8 CHOC_FLAG; //headache timer
extern uint8 action; //player sprite array
extern uint8 KICK_FLAG; //hit/burn timer
extern bool GRANDBRULE_FLAG; //If set, player will be "burned" when hit (fireballs)
extern bool LADDER_FLAG; //True if in a ladder
extern bool PRIER_FLAG; //True if player is forced into kneestanding because of low ceiling
extern uint8 SAUT_FLAG; //6 if free fall or in the middle of a jump, decremented if on solid surface. Must be 0 to initiate a jump.
extern uint8 LAST_ORDER; //Last action (kneestand + jump = silent walk)
extern uint8 FURTIF_FLAG; //Silent walk timer
extern bool DROP_FLAG; //True if an object is throwed forward
extern bool DROPREADY_FLAG;
extern bool CARRY_FLAG; //true if carrying something (add 16 to player sprite)
extern bool POSEREADY_FLAG;
extern uint8 ACTION_TIMER; //Frames since last action change
extern uint8 CROSS_FLAG; //When non-zero, fall through certain floors (after key down)
extern uint8 FUME_FLAG; //Smoke when object hits the floor
extern Uint8 *keystate; //Keyboard state
extern uint8 YFALL;
extern bool POCKET_FLAG;
extern int8 SENSX; //1: walk right, 0: stand still, -1: walk left, triggers the ACTION_TIMER if it changes
extern uint8 SAUT_COUNT; //Incremented from 0 to 3 when accelerating while jumping, stop acceleration upwards if >= 3

int move_player(TITUS_level *level);
TITUS_object *FORCE_POSE(TITUS_level *level);
void DEC_ENERGY(TITUS_level *level);

#endif

