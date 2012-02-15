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

/* tituserror.h
 * Error handling functions
 */

#ifndef TITUS_ERROR_H
#define TITUS_ERROR_H

#define TITUS_ERROR_QUIT -1
#define TITUS_ERROR_NOT_ENOUGH_MEMORY -2
#define TITUS_ERROR_FILE_NOT_FOUND -3
#define TITUS_ERROR_INVALID_FILE -4
#define TITUS_ERROR_SDL_ERROR -5
#define TITUS_ERROR_AUDIO_ERROR -6
#define TITUS_ERROR_INVALID_UTF8 -4
char lasterror[200];
int lasterrornr; //Only to be used when needed, f.ex. when return value is not int (f.ex. in function SDL_Text) (maybe this always should be used?)
void checkerror(void);

#endif
