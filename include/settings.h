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

/* settings.h
 * Handles settings loaded from titus.conf
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#define OPENTITUS_CONFIG_FILE "titus.conf"
#define OPENTITUS_WINDOW_TEXT "OpenTitus"

//Probably not the best way, but it works...
#define HAVE_CONFIG_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

int readconfig(char *configfile);

int initcodes();

int initleveltitles();

char spritefile[256];
char levelfiles[16][256]; //16 levels in moktar, 15 levels in titus
char tituslogofile[256];
int tituslogoformat;
char titusintrofile[256];
int titusintroformat;
char titusmenufile[256];
int titusmenuformat;
char titusfinishfile[256];
int titusfinishformat;
char fontfile[256];
int levelcount;
int devmode;
int reswidth;
int resheight;
int bitdepth;
int ingamewidth;
int ingameheight;
int videomode;
int game;

char levelcode[16][5];
char leveltitle[16][41];

char moduleintrofile[256]; //.mod file
int  moduleintrofileloop; //loop info
char moduleprelevelfile[256];
int  moduleprelevelfileloop;
char modulelevelfile[6][256]; //6 different level files
int  modulelevelfileloop[6];
char modulegameoverfile[256];
int  modulegameoverfileloop;
char modulelevel[16]; //Link to modulelevelfiles

#endif
