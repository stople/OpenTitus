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

extern char spritefile[256];
extern char levelfiles[16][256]; //16 levels in moktar, 15 levels in titus
extern char tituslogofile[256];
extern int tituslogoformat;
extern char titusintrofile[256];
extern int titusintroformat;
extern char titusmenufile[256];
extern int titusmenuformat;
extern char titusfinishfile[256];
extern int titusfinishformat;
extern char fontfile[256];
extern int levelcount;
extern int devmode;
extern int reswidth;
extern int resheight;
extern int bitdepth;
extern int ingamewidth;
extern int ingameheight;
extern int videomode;
extern int game;

extern char levelcode[16][5];
extern char leveltitle[16][41];

extern char moduleintrofile[256]; //.mod file
extern int  moduleintrofileloop; //loop info
extern char moduleprelevelfile[256];
extern int  moduleprelevelfileloop;
extern char modulelevelfile[6][256]; //6 different level files
extern int  modulelevelfileloop[6];
extern char modulegameoverfile[256];
extern int  modulegameoverfileloop;
extern char modulelevel[16]; //Link to modulelevelfiles

#endif
