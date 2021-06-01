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

/* objects.h
 * Handle objects
 */

#ifndef OBJECT_H
#define OBJECT_H

#include "SDL/SDL.h"
#include "level.h"

extern uint8 TAPISFLY_FLAG; //When non-zero, the flying carpet is flying
extern uint8 GRAVITY_FLAG; //When zero, skip object gravity function

void move_objects(TITUS_level *level);
bool SPRITES_VS_SPRITES (TITUS_level *level, TITUS_sprite *sprite1, TITUS_sprite *sprite1ref, TITUS_object **object2); //check if there is an object below that can support the input object
int updateobjectsprite(TITUS_level *level, TITUS_object *obj, int16 number, bool clearflags);
int loadobjects(TITUS_objectdata ***objects, uint16 *count);
int freeobjects(TITUS_objectdata ***objects, uint8 count);


#endif

