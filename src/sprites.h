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

/* sprites.h
 * Sprite functions
 */

#ifndef SPRITES_H
#define SPRITES_H

#include "SDL/SDL.h"
#include "level.h"
#include "definitions.h"

SDL_Surface * copysurface(SDL_Surface * original, bool flip, bool flash);
SDL_Surface * SDL_LoadSprite(unsigned char * first, char width, char height, unsigned int offset, SDL_PixelFormat * pixelformat);
int loadsprites(TITUS_spritedata ***sprites, char * spritedata, int spritedatasize, SDL_PixelFormat * pixelformat, uint16 *count);
SDL_Surface * SDL_LoadTile(unsigned char * first, int i, SDL_PixelFormat * pixelformat);
int copypixelformat(SDL_PixelFormat * destformat, SDL_PixelFormat * srcformat);
int freesprites(TITUS_spritedata ***sprites, uint16 count);
int updatesprite(TITUS_level *level, TITUS_sprite *spr, int16 number, bool clearflags);
int copysprite(TITUS_level *level, TITUS_sprite *dest, TITUS_sprite *src);
int initspritecache(TITUS_spritecache *spritecache, uint16 count, uint16 tmpcount);
int freespritecache(TITUS_spritecache *spritecache);

#endif

