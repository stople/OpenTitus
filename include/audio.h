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

/* audio.h
 * Handles audio.
 */

#ifndef AUDIO_H
#define AUDIO_H

#ifdef AUDIO_ENABLED
//#include "SDL/SDL_mixer.h"

int initaudio();
int freeaudio();
int SELECT_MUSIC(int song_number);
int WAIT_SONG();
int FX_START(int fx_number);
int RETURN_MUSIC();
int startmusic();
int refreshaudio();


/*
#ifdef AUDIO_SDL_MIXER
#endif //AUDIO_SDL_MIXER


#ifdef AUDIO_MIKMOD_SINGLETHREAD
#include <mikmod.h>
MODULE *module;
int checkmodule(void);
#endif //AUDIO_MIKMOD_SINGLETHREAD


#ifdef AUDIO_MIKMOD_MULTITHREAD
#include <mikmod.h>
#include <pthread.h>
MODULE *module;

pthread_mutex_t modulemutex;
pthread_t module_thd_id;

int startmodulethread(void);

typedef struct {
    char command;
    char filename[256];
    int loopinfo;
    char pausestatus;
} MODULE_CONTROL;

MODULE_CONTROL mod_ctrl; 

#endif //AUDIO_MIKMOD_MULTITHREAD
*/
#endif //AUDIO_ENABLED

#endif //AUDIO_H
