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

/* config.h
 * Compile time configurations for audio, debug etc.
 */

#ifndef CONFIG_H
#define CONFIG_H

/* Define to 1 if you want to compile with debug support */
//#undef DEBUG_VERSION
//#define DEBUG_VERSION 1

/* Define to 1 if you want to compile with audio support */
//#undef AUDIO_ENABLED
#define AUDIO_ENABLED 1

/*
// Define to 1 if you want to use libmikmod (amiga .mod playback) directly, in the same thread as the game (laggy)
#undef AUDIO_MIKMOD_SINGLETHREAD
//#define AUDIO_MIKMOD_SINGLETHREAD 1

// Define to 1 if you want to use libmikmod directly, in a separate thread
#undef AUDIO_MIKMOD_MULTITHREAD
//#define AUDIO_MIKMOD_MULTITHREAD 1

// Define to 1 if you want to use sdl-mixer (which includes libmikmod)
#undef AUDIO_SDL_MIXER
//#define AUDIO_SDL_MIXER 1

// Mikmod md_mode, read mikmod documentation for info
#define AUDIO_MIKMOD_MODE DMODE_STEREO | DMODE_SURROUND | DMODE_16BITS | DMODE_SOFT_MUSIC | DMODE_SOFT_SNDFX

// Check for config errors, DO NOT EDIT BELOW!

#ifndef AUDIO_ENABLED
#undef AUDIO_SDL_MIXER
#undef AUDIO_MIKMOD_MULTITHREAD
#undef AUDIO_MIKMOD_SINGLETHREAD
#endif //AUDIO_ENABLED

#ifdef AUDIO_ENABLED

#ifdef AUDIO_SDL_MIXER
#undef AUDIO_MIKMOD_MULTITHREAD
#undef AUDIO_MIKMOD_SINGLETHREAD
#endif //AUDIO_SDL_MIXER

#ifdef AUDIO_MIKMOD_MULTITHREAD
#undef AUDIO_MIKMOD_SINGLETHREAD
#undef AUDIO_SDL_MIXER
#endif //AUDIO_MIKMOD_MULTITHREAD

#ifndef AUDIO_SDL_MIXER
#ifndef AUDIO_MIKMOD_MULTITHREAD
#ifndef AUDIO_MIKMOD_SINGLETHREAD
#define AUDIO_SDL_MIXER 1
#endif //AUDIO_MIKMOD_SINGLETHREAD
#endif //AUDIO_MIKMOD_MULTITHREAD
#endif //AUDIO_SDL_MIXER

#endif //AUDIO_ENABLED
*/

#endif //CONFIG_H
