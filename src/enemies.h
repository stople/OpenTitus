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

/* enemies.h
 * Handles enemies.
 *
 * Global functions:
 * void MOVE_NMI(TITUS_level *level): Move enemies, is called by main game loop
 * void SET_NMI(TITUS_level *level): Collision detection, animation, is called by main game loop
 * void MOVE_TRASH(TITUS_level *level): Move objects thrown by enemies
*/

#pragma once

#include "SDL2/SDL.h"
#include "level.h"


void MOVE_NMI(TITUS_level *level);
void SET_NMI(TITUS_level *level);
void MOVE_TRASH(TITUS_level *level);

int updateenemysprite(TITUS_level *level, TITUS_enemy *enemy, int16 number, bool clearflags);
