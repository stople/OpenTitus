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

#pragma once

#include "SDL2/SDL.h"
#include "definitions.h"
#include "level.h"

#define KEY_F1 SDL_SCANCODE_F1 //Loose a life
#define KEY_F2 SDL_SCANCODE_F2 //Game over
#define KEY_E SDL_SCANCODE_E //Display energy
#define KEY_F4 SDL_SCANCODE_F4 //Status page
#define KEY_LEFT SDL_SCANCODE_LEFT //Left
#define KEY_RIGHT SDL_SCANCODE_RIGHT //Right
#define KEY_UP SDL_SCANCODE_UP //Up
#define KEY_DOWN SDL_SCANCODE_DOWN //Down
#define KEY_JUMP SDL_SCANCODE_UP //Up
#define KEY_SPACE SDL_SCANCODE_SPACE //Space
#define KEY_ENTER SDL_SCANCODE_KP_ENTER //Enter
#define KEY_RETURN SDL_SCANCODE_RETURN //Return
#define KEY_ESC SDL_SCANCODE_ESCAPE //Quit
#define KEY_P SDL_SCANCODE_P //Toggle pause
#define KEY_NOCLIP SDL_SCANCODE_N //Toggle noclip
#define KEY_GODMODE SDL_SCANCODE_G //Toggle godmode
#define KEY_DEBUG SDL_SCANCODE_D //Toggle debug mode
#define KEY_MUSIC SDL_SCANCODE_F3 //Toggle music
#define KEY_FULLSCREEN SDL_SCANCODE_F11 //Toggle fullscreen

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

typedef struct {
    bool enabled;
    uint16 NUM;
} SPRITE;

typedef struct {
    bool enabled;
    uint16 NUM;
} SPRITEDATA;

extern uint8 RESETLEVEL_FLAG;
extern bool GAMEOVER_FLAG; //triggers a game over
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
//extern TITUS_sprite sprite; //Player sprite
//extern TITUS_sprite sprite2; //Secondary player sprite (throwed objects, "hit" when object hits an enemy, smoke when object hits the floor)
extern uint8 INVULNERABLE_FLAG; //When non-zero, boss is invulnerable
extern uint8 TAPISFLY_FLAG; //When non-zero, the flying carpet is flying
extern uint8 CROSS_FLAG; //When non-zero, fall through certain floors (after key down)
extern uint8 GRAVITY_FLAG; //When zero, skip object gravity function
extern uint8 FUME_FLAG; //Smoke when object hits the floor
extern const Uint8 *keystate; //Keyboard state
extern uint8 YFALL;
extern bool POCKET_FLAG;
extern bool PERMUT_FLAG; //If false, there are no animated tiles on the screen?
extern uint8 loop_cycle; //Increased every loop in game loop
extern uint8 tile_anim; //Current tile animation (0-1-2), changed every 4th game loop cycle
extern uint8 BITMAP_X; //Screen offset (X) in tiles
// extern uint8 BITMAP_XM; //Point to the left tile in the tile screen (0 to 19)
extern uint8 BITMAP_Y; //Screen offset (Y) in tiles
// extern uint8 BITMAP_YM; //Point to the top tile in the tile screen (0 to 11)
extern bool XSCROLL_CENTER; //If true, the screen will scroll in X
extern int16 XLIMIT_SCROLL; //If scrolling: scroll until player is in this tile (X)
extern int16 XLIMIT; //The engine will not scroll past this tile before the player have crossed the line (X)
extern bool YSCROLL_CENTER; //If true, the screen will scroll in Y
extern uint8 YLIMIT_SCROLL; //If scrolling: scroll until player is in this tile (Y)
extern uint8 ALTITUDE_ZERO; //The engine will not scroll below this tile before the player have gone below (Y)
extern int LAST_CLOCK; //Used for fixed framerate
extern uint16 IMAGE_COUNTER; //Increased every loop in game loop (0 to 0x0FFF)
extern int8 SENSX; //1: walk right, 0: stand still, -1: walk left, triggers the ACTION_TIMER if it changes
extern uint8 SAUT_COUNT; //Incremented from 0 to 3 when accelerating while jumping, stop acceleration upwards if >= 3
extern bool NOSCROLL_FLAG;
extern bool NEWLEVEL_FLAG; //Finish a level
extern uint8 BIGNMI_NBR; //Number of bosses that needs to be killed to finish
extern uint8 TAUPE_FLAG; //Used for enemies walking and popping up
extern uint8 TAPISWAIT_FLAG; //Flying carpet state
extern uint8 SEECHOC_FLAG; //Counter when hit
extern uint8 BIGNMI_POWER; //Lives of the boss
extern bool boss_alive; //True if the boss is alive
extern uint8 AUDIOMODE;

extern bool GODMODE; //If true, the player will not interfere with the enemies
extern bool NOCLIP; //If true, the player will move noclip
extern bool DISPLAYLOOPTIME; //If true, display loop time in milliseconds
extern uint8 LOOPTIME; //Loop time
extern uint8 SUBTIME[16]; //Sub time
extern uint16 FPS; //Frames pr second
extern uint16 FPS_LAST; //Frames pr second
extern uint16 LAST_CLOCK_CORR; //Correction to LAST_CLOCK

extern SPRITE sprites[256];

extern SPRITEDATA spritedata[256];

extern uint16 level_code[16];
