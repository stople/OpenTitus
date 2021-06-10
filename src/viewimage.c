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

/* viewimage.c
 * Displays an image (menu, logo, intro, photo)
 */

#include <stdio.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "viewimage.h"
#include "sqz.h"
#include "settings.h"
#include "backbuffer.h"
#include "globals.h"
#include "common.h"

#include "config.h"

#include "audio.h"

int viewimage(char * imagefile, int imageformat, int displayformat, int delay) {
    SDL_Surface *surface;
    SDL_Palette *palette;
    char *tmpchar;
    SDL_Surface *image;
    //int imagesize = 66304; //for 256 color
    unsigned char *imagedata;
    int retval;
    int i, j;
    int activedelay = 1;
    int fadeoutskip = 0;
    SDL_Event event;
    unsigned int groupsize;
    //unsigned int fade_time = 2000;
    unsigned int fade_time = 1000;
    unsigned int tick_start = 0;
    unsigned int image_alpha = 0;

    SDL_Rect src, dest;

    retval = unSQZ(imagefile, &imagedata);

    if (retval < 0) {
        free (imagedata);
        return (retval);
    }

    switch (imageformat) {
    case 0: //Planar 16-color grayscale
        surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 200, 8, 0, 0, 0, 0);
        palette = (surface->format)->palette;
        if (palette) {
            for (i = 0; i < 16; i++) {
                palette->colors[i].r = i * 16;
                palette->colors[i].g = i * 16;
                palette->colors[i].b = i * 16;
                palette->colors[i].unused = 0; 
            }
        }

        groupsize = ((320 * 200) >> 3);
        tmpchar = (char *)surface->pixels;
        for (i = 0; i < groupsize; i++) {
            for (j = 7; j >= 0; j--) {
                *tmpchar = (imagedata[i] >> j) & 0x01;
                *tmpchar += (imagedata[i + groupsize] >> j << 1) & 0x02;
                *tmpchar += (imagedata[i + groupsize * 2] >> j << 2) & 0x04;
                *tmpchar += (imagedata[i + groupsize * 3] >> j << 3) & 0x08;
                tmpchar++;
            }
        }

        break;

    case 1: //Planar 16-color

        break;

    case 2: //256 color
        surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 200, 8, 0, 0, 0, 0);
        palette = (surface->format)->palette;
        if (palette) {
            for (i = 0; i < 256; i++) {
                palette->colors[i].r = (imagedata[i * 3] & 0xFF) * 4;
                palette->colors[i].g = (imagedata[i * 3 + 1] & 0xFF) * 4;
                palette->colors[i].b = (imagedata[i * 3 + 2] & 0xFF) * 4;
                palette->colors[i].unused = 0; 
            }
        }

        tmpchar = (char *)surface->pixels;
        for (i = 256 * 3; i < 256 * 3 + 320*200; i++) {
            *tmpchar = imagedata[i];
            tmpchar++;
        }
        break;
    }

    free (imagedata);
    image = SDL_DisplayFormat(surface);

    src.x = 0;
    src.y = 0;
    src.w = image->w;
    src.h = image->h;

    dest.x = 0;
    dest.y = 0;
    dest.w = image->w;
    dest.h = image->h;

    switch (displayformat) {
    case 0: //fade in fade out
        tick_start = SDL_GetTicks();
        while ((image_alpha < 255) && activedelay) //Fade to visible
        {
            if (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    SDL_FreeSurface(image);
                    SDL_FreeSurface(surface);
                    return (-1);
                }

                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        SDL_FreeSurface(image);
                        SDL_FreeSurface(surface);
                        return (-1);
                    }

                    if (event.key.keysym.sym == KEY_RETURN || event.key.keysym.sym == KEY_ENTER || event.key.keysym.sym == KEY_SPACE) {
                        activedelay = 0;
                        fadeoutskip = 255 - image_alpha;
                    }

                    if (event.key.keysym.sym == KEY_MUSIC) {
                        AUDIOMODE++;
                        if (AUDIOMODE > 1) {
                            AUDIOMODE = 0;
                        }
                        if (AUDIOMODE == 1) {
                            startmusic();
                        }
                    }

                }
            }

            image_alpha = (SDL_GetTicks() - tick_start) * 256 / fade_time;

            if (image_alpha > 255)
                image_alpha = 255;

            SDL_SetAlpha(image, SDL_SRCALPHA, image_alpha);
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
            SDL_BlitSurface(image, &src, screen, &dest);
            SDL_Flip(screen);
            titus_sleep();
        }

        while (activedelay) //Visible delay
        {
            if (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    SDL_FreeSurface(image);
                    SDL_FreeSurface(surface);
                    return (-1);
                }

                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        SDL_FreeSurface(image);
                        SDL_FreeSurface(surface);
                        return (-1);
                    }

                    if (event.key.keysym.sym == KEY_RETURN || event.key.keysym.sym == KEY_ENTER || event.key.keysym.sym == KEY_SPACE)
                        activedelay = 0;

                    if (event.key.keysym.sym == KEY_MUSIC) {
                        AUDIOMODE++;
                        if (AUDIOMODE > 1) {
                            AUDIOMODE = 0;
                        }
                        if (AUDIOMODE == 1) {
                            startmusic();
                        }
                    }
                }
            }
            titus_sleep();
            if ((SDL_GetTicks() - tick_start + fade_time) >= delay)
                activedelay = 0;
        }

        image_alpha = 255 - image_alpha;
        tick_start = SDL_GetTicks();
        while (image_alpha < 255) //Fade to black
        {
            if (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    SDL_FreeSurface(image);
                    SDL_FreeSurface(surface);
                    return (-1);
                }

                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        SDL_FreeSurface(image);
                        SDL_FreeSurface(surface);
                        return (-1);
                    }
                    if (event.key.keysym.sym == KEY_MUSIC) {
                        AUDIOMODE++;
                        if (AUDIOMODE > 1) {
                            AUDIOMODE = 0;
                        }
                        if (AUDIOMODE == 1) {
                            startmusic();
                        }
                    }
                }
            }

            image_alpha = (SDL_GetTicks() - tick_start) * 256 / fade_time + fadeoutskip;

            if (image_alpha > 255)
                image_alpha = 255;

            SDL_SetAlpha(image, SDL_SRCALPHA, 255 - image_alpha);
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
            SDL_BlitSurface(image, &src, screen, &dest);
            SDL_Flip(screen);
            titus_sleep();
        }
        break;

    case 1: //visible until keypress, then fade out
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_BlitSurface(image, &src, screen, &dest);
        SDL_Flip(screen);

        retval = waitforbutton();
        if (retval < 0) {
            SDL_FreeSurface(image);
            SDL_FreeSurface(surface);
            return retval;
        }

        tick_start = SDL_GetTicks();
        while (image_alpha < 255) //Fade to black
        {
            if (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    SDL_FreeSurface(image);
                    SDL_FreeSurface(surface);
                    return (-1);
                }

                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        SDL_FreeSurface(image);
                        SDL_FreeSurface(surface);
                        return (-1);
                    }
                    if (event.key.keysym.sym == KEY_MUSIC) {
                        AUDIOMODE++;
                        if (AUDIOMODE > 1) {
                            AUDIOMODE = 0;
                        }
                        if (AUDIOMODE == 1) {
                            startmusic();
                        }
                    }
                }
            }

            image_alpha = (SDL_GetTicks() - tick_start) * 256 / fade_time;

            if (image_alpha > 255)
                image_alpha = 255;

            SDL_SetAlpha(image, SDL_SRCALPHA, 255 - image_alpha);
            SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
            SDL_BlitSurface(image, &src, screen, &dest);
            SDL_Flip(screen);
            titus_sleep();
        }

        break;
    }

    SDL_FreeSurface(image);
    SDL_FreeSurface(surface);


    return 0;


}
