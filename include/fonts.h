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

/* fonts.h
 * Font functions
 */

 #ifndef FONTS_H
#define FONTS_H

#include "definitions.h"

#define CHAR_a 97
#define CHAR_A 65
#define CHAR_f 102
#define CHAR_F 70
#define CHAR_z 122
#define CHAR_Z 90
#define CHAR_0 48
#define CHAR_9 57
#define CHAR_EXCLAMATION 33
#define CHAR_QUESTION 63
#define CHAR_DOT 46
#define CHAR_DOLLAR 36
#define CHAR_UNDERSCORE 95
#define CHAR_SPACE 32

typedef struct _TITUS_font TITUS_font;

struct _TITUS_font {
    uint8 type[256]; //0: not in use, 1: malloced sub, 2: malloced surface, 3: surface pointer, 4: invalid UTF-8
    void *sub[256]; //May be malloced
};

TITUS_font *font; //Malloced
SDL_Surface *font_undefined; //Pointer

int loadfonts(void);
int freefonts(void);

//SDL_Surface * SDL_Text(char *text, bool inv_back);
//int SDL_Print_Text(char *text, int x, int y, bool inv_back);
int SDL_Print_Text(uint8 *text, int x, int y);

int viewintrotext();

#endif

