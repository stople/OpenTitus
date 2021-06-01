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

/* globals.h
 * Global variables
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "SDL/SDL.h"
#include "definitions.h"
#include "level.h"
#include "dingoo.h"

//To simplify porting:
#ifdef _DINGUX

/*
#define DINGOO_BUTTON_UP            SDLK_UP
#define DINGOO_BUTTON_DOWN          SDLK_DOWN
#define DINGOO_BUTTON_RIGHT         SDLK_RIGHT
#define DINGOO_BUTTON_LEFT          SDLK_LEFT
#define DINGOO_BUTTON_R             SDLK_BACKSPACE
#define DINGOO_BUTTON_L             SDLK_TAB
#define DINGOO_BUTTON_A             SDLK_LCTRL
#define DINGOO_BUTTON_B             SDLK_LALT
#define DINGOO_BUTTON_X             SDLK_SPACE
#define DINGOO_BUTTON_Y             SDLK_LSHIFT
#define DINGOO_BUTTON_SELECT        SDLK_ESCAPE
#define DINGOO_BUTTON_START         SDLK_RETURN
#define DINGOO_BUTTON_END           SDLK_UNKNOWN
*/

#define KEY_F1          SDLK_F1 //Loose a life, not in use
#define KEY_F2          SDLK_F2 //Game over, not in use
#define KEY_STATUS      DINGOO_BUTTON_A //Energy + status page
#define KEY_LEFT        DINGOO_BUTTON_LEFT //Left
#define KEY_RIGHT       DINGOO_BUTTON_RIGHT //Right
#define KEY_UP          DINGOO_BUTTON_UP //Up
#define KEY_DOWN        DINGOO_BUTTON_DOWN //Down
#define KEY_JUMP        DINGOO_BUTTON_X //Jump
#define KEY_SPACE       DINGOO_BUTTON_B //Space
#define KEY_ENTER       DINGOO_BUTTON_START //Enter
#define KEY_RETURN      DINGOO_BUTTON_START //Return
#define KEY_ESC         DINGOO_BUTTON_SELECT //Quit
#define KEY_P           DINGOO_BUTTON_START //Toggle pause (A button)
#define KEY_CHEAT       DINGOO_BUTTON_L //Toggle cheat noclip/godmode
#define KEY_SKIPLEVEL   DINGOO_BUTTON_R //Skip level
#define KEY_DEBUG       DINGOO_BUTTON_Y //Toggle debug mode
#define KEY_MUSIC       DINGOO_BUTTON_Y //Toggle music

#else

#define KEY_F1 SDLK_F1 //Loose a life
#define KEY_F2 SDLK_F2 //Game over
#define KEY_E SDLK_e //Display energy
#define KEY_F4 SDLK_F4 //Status page
#define KEY_LEFT SDLK_LEFT //Left
#define KEY_RIGHT SDLK_RIGHT //Right
#define KEY_UP SDLK_UP //Up
#define KEY_DOWN SDLK_DOWN //Down
#define KEY_JUMP SDLK_UP //Up
#define KEY_SPACE SDLK_SPACE //Space
#define KEY_ENTER SDLK_KP_ENTER //Enter
#define KEY_RETURN SDLK_RETURN //Return
#define KEY_ESC SDLK_ESCAPE //Quit
#define KEY_P SDLK_p //Toggle pause
#define KEY_NOCLIP SDLK_n //Toggle noclip
#define KEY_GODMODE SDLK_g //Toggle godmode
#define KEY_DEBUG SDLK_d //Toggle debug mode
#define KEY_MUSIC SDLK_F3 //Toggle music

#endif


#define TEST_ZONE 4
#define MAX_X 4
#define MAX_Y 12
#define MAP_LIMIT_Y -1
#define S_LINES 12
#define S_COLUMNS 20
#define LIMIT_TIMER 22
#define ALERT_X 32
#define screen_width 20
#define screen_height 12
#define FIRST_OBJET 30
#define FIRST_NMI 101
#define MAXIMUM_BONUS 100
#define MAXIMUM_ENERGY 16
#define MAXIMUM_DTRP 4
#define GESTION_X 40
#define GESTION_Y 20
#define MAX_SPEED_DEAD 20

#define HFLAG_NOWALL 0
#define HFLAG_WALL 1
#define HFLAG_BONUS 2
#define HFLAG_DEADLY 3
#define HFLAG_CODE 4
#define HFLAG_PADLOCK 5
#define HFLAG_LEVEL14 6

#define FFLAG_NOFLOOR 0
#define FFLAG_FLOOR 1
#define FFLAG_SSFLOOR 2
#define FFLAG_SFLOOR 3
#define FFLAG_VSFLOOR 4
#define FFLAG_DROP 5
#define FFLAG_LADDER 6
#define FFLAG_BONUS 7
#define FFLAG_WATER 8
#define FFLAG_FIRE 9
#define FFLAG_SPIKES 10
#define FFLAG_CODE 11
#define FFLAG_PADLOCK 12
#define FFLAG_LEVEL14 13

#define CFLAG_NOCEILING 0
#define CFLAG_CEILING 1
#define CFLAG_LADDER 2
#define CFLAG_PADLOCK 3
#define CFLAG_DEADLY 4

#define uint8 unsigned char
#define uint16 unsigned short int
#define int8 signed char
#define int16 signed short int


extern bool NOSCROLL_FLAG;
extern uint8 TAUPE_FLAG; //Used for enemies walking and popping up
extern uint8 TAPISWAIT_FLAG; //Flying carpet state
extern uint8 SEECHOC_FLAG; //Counter when hit
extern uint8 BIGNMI_POWER; //Lives of the boss
extern bool boss_alive; //True if the boss is alive
extern uint8 AUDIOMODE;

extern bool GODMODE; //If true, the player will not interfere with the enemies
extern bool NOCLIP; //If true, the player will move noclip
extern bool DISPLAYLOOPTIME; //If true, display loop time in milliseconds

typedef struct {
    bool enabled;
    uint16 NUM;
} SPRITE;

//SPRITE sprites[256]; 

typedef struct {
    bool enabled;
    uint16 NUM;
} SPRITEDATA;

//SPRITEDATA spritedata[256]; 

#endif

