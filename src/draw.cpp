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

/* draw.c
 * Draw functions
 *
 * Global functions:
 * void DISPLAY_TILES(): Draw map tiles
 * int viewstatus(TITUS_level *level, bool countbonus): View status screen (F4)
 * void flip_screen(bool slow): Flips the screen and a short delay
 * void INIT_SCREENM(TITUS_level *level): Initialize screen
 * void draw_health_bars(TITUS_level *level): Draw energy
 * void fadeout(): Fade the screen to black
 * int view_password(TITUS_level *level, uint8 level_index): Display the password
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL.h"
#include "globals.h"
#include "window.h"
#include "sprites.h"
#include "draw.h"
#include "settings.h"
#include "common.h"
#include "tituserror.h"
#include "original.h"
#include "fonts.h"
#include "keyboard.h"
#include "gates.h"
#include "scroll.h"
#include "audio.h"

static SDL_Surface *sprite_from_cache(TITUS_level *level, TITUS_sprite *spr);
static void display_sprite(TITUS_level *level, TITUS_sprite *spr);

void DISPLAY_TILES(TITUS_level *level) {
    //First of all: make the screen black, at least the lower part of the screen
    SDL_Rect bottom_bar;
    bottom_bar.x = 0;
    bottom_bar.y = screen_height * 16;
    bottom_bar.w = screen_width * 16;
    bottom_bar.h = 200 - screen_height * 16;
    Window::clear(&bottom_bar);

    SDL_Rect src, dest;
    src.x = 0;
    src.y = 0;
    src.w = 16;
    src.h = 16;
    dest.w = src.w;
    dest.h = src.h;

    for (int x = 0; x < 20; x++) {
        for (int y = 0; y < 12; y++) {
            dest.x = x * 16;
            dest.y = y * 16;
            auto tile = level->tilemap[BITMAP_Y + y][BITMAP_X + x];
            SDL_BlitSurface(level->tile[level->tile[tile].animation[tile_anim]].tiledata, &src, Window::screen, &dest);
        }
    }
}


//Loop through all sprites, and draw the sprites that should be visible on the screen (NOT by using the visible flag, it uses the coordinates)
//If the flash bit is set, the first 3 planes will be 0, the last plane will be normal (colour & 0x01, odd colors gets white, even colours gets black)


void DISPLAY_SPRITES(TITUS_level *level) {
    int16 i;

    for (i = level->elevatorcount - 1; i >= 0; i--) {
        display_sprite(level, &(level->elevator[i].sprite));
    }

    for (i = level->trashcount - 1; i >= 0; i--) {
        display_sprite(level, &(level->trash[i]));
    }

    for (i = level->enemycount - 1; i >= 0; i--) {
        display_sprite(level, &(level->enemy[i].sprite));
    }

    for (i = level->objectcount - 1; i >= 0; i--) {
        display_sprite(level, &(level->object[i].sprite));
    }

    display_sprite(level, &(level->player.sprite3));
    display_sprite(level, &(level->player.sprite2));
    display_sprite(level, &(level->player.sprite));

    if (GODMODE) {
        SDL_Print_Text("GODMODE", 30 * 8, 0 * 12);
    }
    if (NOCLIP) {
        SDL_Print_Text("NOCLIP", 30 * 8, 1 * 12);
    }
}

void display_sprite(TITUS_level *level, TITUS_sprite *spr) {
    SDL_Surface *image;
    SDL_Rect src, dest;
    if (!spr->enabled) {
        return;
    }
    if (spr->invisible) {
        return;
    }
    spr->visible = false;
    //At this point, the buffer should be the correct size

    if (!spr->flipped) {
        dest.x = spr->x - spr->spritedata->refwidth - (BITMAP_X << 4);
    } else {
        dest.x = spr->x + spr->spritedata->refwidth - spr->spritedata->data->w - (BITMAP_X << 4);
    }
    dest.y = spr->y + spr->spritedata->refheight - spr->spritedata->data->h + 1 - (BITMAP_Y << 4);

    if ((dest.x >= screen_width * 16) || //Right for the screen
      (dest.x + spr->spritedata->data->w < 0) || //Left for the screen
      (dest.y + spr->spritedata->data->h < 0) || //Above the screen
      (dest.y >= screen_height * 16)) { //Below the screen
        return;
    }

    image = sprite_from_cache(level, spr);

    src.x = 0;
    src.y = 0;
    src.w = image->w;
    src.h = image->h;

    if (dest.x < 0) {
        src.x = 0 - dest.x;
        src.w -= src.x;
        dest.x = 0;
    }
    if (dest.y < 0) {
        src.y = 0 - dest.y;
        src.h -= src.y;
        dest.y = 0;
    }
    if (dest.x + src.w > screen_width * 16) {
        src.w = screen_width * 16 - dest.x;
    }
    if (dest.y + src.h > screen_height * 16) {
        src.h = screen_height * 16 - dest.y;
    }

    SDL_BlitSurface(image, &src, Window::screen, &dest);

    spr->visible = true;
    spr->flash = false;

}

SDL_Surface *sprite_from_cache(TITUS_level *level, TITUS_sprite *spr) {
    TITUS_spritecache *cache = level->spritecache;
    TITUS_spritedata *spritedata = level->spritedata[spr->number];
    TITUS_spritebuffer *spritebuffer;
    uint8 index;
    int16 i;

    if (spr->flipped) {index = 1;} else {index = 0;};

    if (spr->flash) {
        for (i = cache->count - cache->tmpcount; i < cache->count; i++) {
            spritebuffer = cache->spritebuffer[i];
            if (spritebuffer != NULL) {
                if ((spritebuffer->spritedata == spritedata) &&
                  (spritebuffer->index == index + 2)) {
                    return spritebuffer->data; //Already in buffer
                }
            }
        }
        //Not found, load into buffer
        cache->cycle2++;
        if (cache->cycle2 >= cache->count) { //The last 3 buffer surfaces is temporary (reserved for flash)
            cache->cycle2 = cache->count - cache->tmpcount;
        }
        spritebuffer = cache->spritebuffer[cache->cycle2];
        SDL_FreeSurface(spritebuffer->data); //Free old surface
        spritebuffer->data = copysurface(spritedata->data, spr->flipped, spr->flash);
        spritebuffer->spritedata = spritedata;
        spritebuffer->index = index + 2;
        return spritebuffer->data;
    } else {
        if (spritedata->spritebuffer[index] == NULL) {
            cache->cycle++;
            if (cache->cycle + cache->tmpcount >= cache->count) { //The last 3 buffer surfaces is temporary (reserved for flash)
                cache->cycle = 0;
            }
            spritebuffer = cache->spritebuffer[cache->cycle];
            if (spritebuffer->spritedata != NULL) {
                spritebuffer->spritedata->spritebuffer[spritebuffer->index] = NULL; //Remove old link
            }
            SDL_FreeSurface(spritebuffer->data); //Free old surface
            spritebuffer->data = copysurface(spritedata->data, spr->flipped, spr->flash);
            spritebuffer->spritedata = spritedata;
            spritebuffer->index = index;
            spritedata->spritebuffer[index] = spritebuffer;
        }
        return spritedata->spritebuffer[index]->data;
    }
}

void NO_FAST_CPU(bool slow) {
    int tick, duration, delay, tick2;
    tick = SDL_GetTicks();
    if (slow) {
        delay = 29; //28.53612, fps: 70.09Hz/2
    } else {
        delay = 10;
    }
    LOOPTIME = (tick - LAST_CLOCK);
    delay = delay - (tick - LAST_CLOCK) - LAST_CLOCK_CORR;
    LAST_CLOCK_CORR = 0;
    if (delay > 40) {
        delay = 1;
    } else if (delay < 0) {
        LAST_CLOCK_CORR = (0 - delay) / 2; // To reduce LAST_CLOCK_CORR
        delay = 0;
    }

    tick2 = SDL_GetTicks();
    duration = abs(tick - tick2);
    while (duration < delay) {
        titus_sleep();
        //SDL_Delay(1);
        tick2 = SDL_GetTicks();
        duration = abs(tick - tick2);
    }

    tick2 = SDL_GetTicks();
    if ((tick2 / 1000) != (LAST_CLOCK / 1000)) {
        FPS_LAST = FPS;
        FPS = 0;
    }
    FPS++;


    LAST_CLOCK_CORR += tick2 - tick - delay;
        if (LAST_CLOCK_CORR > 25) {
            LAST_CLOCK_CORR = 25;
        }

    LAST_CLOCK = tick2;
}

void flip_screen(bool slow) {
    Window::render();
    NO_FAST_CPU(slow);
}

int viewstatus(TITUS_level *level, bool countbonus){
    int retval, i, j;
    char tmpchars[10];
    Window::clear();

    if (game == GameType::Titus) {
        SDL_Print_Text("LEVEL", 13 * 8, 12 * 5);
        SDL_Print_Text("EXTRA BONUS", 10 * 8, 10 * 12);
        SDL_Print_Text("LIVES", 10 * 8, 11 * 12);
    } else if (game == GameType::Moktar) {
        SDL_Print_Text("ETAPE", 13 * 8, 12 * 5);
        SDL_Print_Text("EXTRA BONUS", 10 * 8, 10 * 12);
        SDL_Print_Text("VIE", 10 * 8, 11 * 12);
    }


    sprintf(tmpchars, "%d", level->levelnumber + 1);
    SDL_Print_Text(tmpchars, 25 * 8 - strlen(tmpchars) * 8, 12 * 5);

    SDL_Print_Text(leveltitle[level->levelnumber], 0, 12 * 5 + 16);
    sprintf(tmpchars, "%d", level->extrabonus);
    SDL_Print_Text(tmpchars, 28 * 8 - strlen(tmpchars) * 8, 10 * 12);

    sprintf(tmpchars, "%d", level->lives);
    SDL_Print_Text(tmpchars, 28 * 8 - strlen(tmpchars) * 8, 11 * 12);

    Window::render();

    if (countbonus && (level->extrabonus >= 10)) {
        retval = waitforbutton();
        if (retval < 0) {
            return retval;
        }
        while (level->extrabonus >= 10) {
            for (i = 0; i < 10; i++) {
                level->extrabonus--;
                sprintf(tmpchars, "%2d", level->extrabonus);
                SDL_Print_Text(tmpchars, 28 * 8 - strlen(tmpchars) * 8, 10 * 12);
                Window::render();
                for (j = 0; j < 15; j++) {
                    NO_FAST_CPU(false);
                }
            }
            level->lives++;
            sprintf(tmpchars, "%d", level->lives);
            SDL_Print_Text(tmpchars, 28 * 8 - strlen(tmpchars) * 8, 11 * 12);
            Window::render();
            for (j = 0; j < 10; j++) {
                NO_FAST_CPU(false);
            }
        }
    }

    retval = waitforbutton();
    if (retval < 0)
        return retval;

    Window::clear();
    Window::render();

    return (0);
}

void INIT_SCREENM(TITUS_level *level) {
    CLOSE_SCREEN();
    BITMAP_X = 0;
    BITMAP_Y = 0;
    do {
        scroll(level);
    } while (YSCROLL_CENTER || XSCROLL_CENTER);
    OPEN_SCREEN(level);
}

void draw_health_bars(TITUS_level *level) {
    subto0(&(BAR_FLAG));
    if(BAR_FLAG <= 0) {
        return;
    }
    uint8 offset = 96;
    uint8 i;
    SDL_Rect dest;
    for (i = 0; i < level->player.hp; i++) { //Draw big bars (4px*16px, spacing 4px)
        dest.x = offset;
        dest.y = 9;
        dest.w = 4;
        dest.h = 16;
        SDL_FillRect(Window::screen, &dest, SDL_MapRGB(Window::screen->format, 255, 255, 255));
        offset += 8;
    }
    for (i = 0; i < MAXIMUM_ENERGY - level->player.hp; i++) { //Draw small bars (4px*4px, spacing 4px)
        dest.x = offset;
        dest.y = 15;
        dest.w = 4;
        dest.h = 4;
        SDL_FillRect(Window::screen, &dest, SDL_MapRGB(Window::screen->format, 255, 255, 255));
        offset += 8;
    }
}

void fadeout() {
    SDL_Surface *image;
    SDL_Event event;
    unsigned int fade_time = 1000;
    unsigned int tick_start = 0;
    unsigned int image_alpha = 0;
    SDL_Rect src, dest;

    src.x = 0;
    src.y = 0;
    src.w = Window::screen->w;
    src.h = Window::screen->h;

    dest.x = 0;
    dest.y = 0;
    dest.w = Window::screen->w;
    dest.h = Window::screen->h;

    image = SDL_ConvertSurface(Window::screen, Window::screen->format, SDL_SWSURFACE);

    SDL_BlitSurface(Window::screen, &src, image, &dest);
    tick_start = SDL_GetTicks();
    while (image_alpha < 255) //Fade to black
    {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_FreeSurface(image);
                // FIXME: handle this better
                return;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    SDL_FreeSurface(image);
                    // FIXME: handle this better
                    return;
                }
                if (event.key.keysym.scancode == KEY_MUSIC) {
                    AUDIOMODE++;
                    if (AUDIOMODE > 1) {
                        AUDIOMODE = 0;
                    }
                    if (AUDIOMODE == 1) {
                        startmusic();
                    }
                } else if (event.key.keysym.scancode == KEY_FULLSCREEN) {
                    Window::toggle_fullscreen();
                }
            }
        }

        image_alpha = (SDL_GetTicks() - tick_start) * 256 / fade_time;

        if (image_alpha > 255)
            image_alpha = 255;

        SDL_SetSurfaceAlphaMod(image, 255 - image_alpha);
        SDL_SetSurfaceBlendMode(image, SDL_BLENDMODE_BLEND);
        Window::clear();
        SDL_BlitSurface(image, &src, Window::screen, &dest);
        Window::render();

        titus_sleep();
    }
    SDL_FreeSurface(image);

}

int view_password(TITUS_level *level, uint8 level_index) {
    //Display the password !
    char tmpchars[10];
    int retval;

    CLOSE_SCREEN();
    Window::clear();
    Window::render();

    if (game == GameType::Titus) {
        SDL_Print_Text("LEVEL", 13 * 8, 13 * 8);
    } else if (game == GameType::Moktar) {
        SDL_Print_Text("ETAPE", 13 * 8, 13 * 8);
    }
    sprintf(tmpchars, "%d", level_index + 1);
    SDL_Print_Text(tmpchars, 25 * 8 - strlen(tmpchars) * 8, 13 * 8);

    SDL_Print_Text("CODE", 14 * 8, 10 * 8);
    SDL_Print_Text(levelcode[level_index], 20 * 8, 10 * 8);

    Window::render();
    retval = waitforbutton();
    if (retval < 0)
        return retval;

    //Window::paint();
    OPEN_SCREEN(level);
    return (0);
}

int loadpixelformat(SDL_PixelFormat **pixelformat){
    int i;

    *pixelformat = (SDL_PixelFormat *)SDL_malloc(sizeof(SDL_PixelFormat));
    if (*pixelformat == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize palette!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }

    (*pixelformat)->palette = (SDL_Palette *)SDL_malloc(sizeof(SDL_Palette));
    if ((*pixelformat)->palette == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize palette!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }

    (*pixelformat)->palette->ncolors = 16;

    (*pixelformat)->palette->colors = (SDL_Color *)SDL_malloc(sizeof(SDL_Color) * (*pixelformat)->palette->ncolors);
    if ((*pixelformat)->palette->colors == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize palette!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }

    for (i = 0; i < (*pixelformat)->palette->ncolors; i++) {
        (*pixelformat)->palette->colors[i].r = orig_palette_colour[i].r;
        (*pixelformat)->palette->colors[i].g = orig_palette_colour[i].g;
        (*pixelformat)->palette->colors[i].b = orig_palette_colour[i].b;
    }

    (*pixelformat)->BitsPerPixel = 8;
    (*pixelformat)->BytesPerPixel = 1;

    (*pixelformat)->Rloss = 0;
    (*pixelformat)->Gloss = 0;
    (*pixelformat)->Bloss = 0;
    (*pixelformat)->Aloss = 0;

    (*pixelformat)->Rshift = 0;
    (*pixelformat)->Gshift = 0;
    (*pixelformat)->Bshift = 0;
    (*pixelformat)->Ashift = 0;

    (*pixelformat)->Rmask = 0;
    (*pixelformat)->Gmask = 0;
    (*pixelformat)->Bmask = 0;
    (*pixelformat)->Amask = 0;

    //(*pixelformat)->colorkey = 0;
    //(*pixelformat)->alpha = 255;

    return (0);
}

int loadpixelformat_font(SDL_PixelFormat **pixelformat){
    int i;

    *pixelformat = (SDL_PixelFormat *)SDL_malloc(sizeof(SDL_PixelFormat));
    if (*pixelformat == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize palette!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }

    (*pixelformat)->palette = (SDL_Palette *)SDL_malloc(sizeof(SDL_Palette));
    if ((*pixelformat)->palette == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize palette!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }

    (*pixelformat)->palette->ncolors = 16;

    (*pixelformat)->palette->colors = (SDL_Color *)SDL_malloc(sizeof(SDL_Color) * (*pixelformat)->palette->ncolors);
    if ((*pixelformat)->palette->colors == NULL) {
        sprintf(lasterror, "Error: Not enough memory to initialize palette!\n");
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }

    for (i = 0; i < (*pixelformat)->palette->ncolors; i++) {
        (*pixelformat)->palette->colors[i].r = orig_palette_font_colour[i].r;
        (*pixelformat)->palette->colors[i].g = orig_palette_font_colour[i].g;
        (*pixelformat)->palette->colors[i].b = orig_palette_font_colour[i].b;
    }

    (*pixelformat)->BitsPerPixel = 8;
    (*pixelformat)->BytesPerPixel = 1;

    (*pixelformat)->Rloss = 0;
    (*pixelformat)->Gloss = 0;
    (*pixelformat)->Bloss = 0;
    (*pixelformat)->Aloss = 0;

    (*pixelformat)->Rshift = 0;
    (*pixelformat)->Gshift = 0;
    (*pixelformat)->Bshift = 0;
    (*pixelformat)->Ashift = 0;

    (*pixelformat)->Rmask = 0;
    (*pixelformat)->Gmask = 0;
    (*pixelformat)->Bmask = 0;
    (*pixelformat)->Amask = 0;

    //(*pixelformat)->colorkey = 0;
    //(*pixelformat)->alpha = 255;

    return (0);
}

int freepixelformat(SDL_PixelFormat **pixelformat){
    free ((*pixelformat)->palette->colors);
    free ((*pixelformat)->palette);
    free (*pixelformat);
    return (0);
}
