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

/* fonts.c
 * Font functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "sqz.h"
#include "settings.h"
#include "fonts.h"
#include "backbuffer.h"
#include "tituserror.h"
#include "keyboard.h"
#include "draw.h"
#include <time.h>
#include "sprites.h"

TITUS_font *font; //Malloced
SDL_Surface *font_undefined; //Pointer

SDL_Surface * SDL_LoadChar(unsigned char * fontdata, int offset, SDL_PixelFormat * pixelformat);
int freesubfont(TITUS_font *f_sub);

int loadfonts(void) {
    int i, retval;
    unsigned int offset = 0;
    SDL_Surface *surface = NULL;
    char *tmpchar;
    SDL_PixelFormat *pixelformat;
    uint8 *fontdata;

    retval = unSQZ(fontfile, &fontdata);

    if (retval < 0) {
        lasterrornr = retval;
        free (fontdata);
        return (0);
    }

    
    retval = loadpixelformat_font(&(pixelformat));
    if (retval < 0) {
        return retval;
    }


    //First byte
    font = (TITUS_font *)SDL_malloc(sizeof(TITUS_font) * 256);
    if (font == NULL) {
        sprintf(lasterror, "Error: Not enough memory to load fonts!\n");
        freepixelformat(&(pixelformat));
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    for (i = 0; i < 256; i++) {
        font->type[i] = 0;
        font->sub[i] = NULL;
    }
    
/*    
    //Second byte
    font->sub[0] = (TITUS_font *)SDL_malloc(sizeof(TITUS_font) * 256);
    if (font->sub[0] == NULL) {
        sprintf(lasterror, "Error: Not enough memory to load fonts!\n");
        freepixelformat(&(pixelformat));
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    font->type[0] = 1; //Malloced sub
    f_sub = (TITUS_font *)font->sub[0];
    for (i = 0; i < 256; i++) {
        f_sub->type[i] = 0;
        f_sub->sub[i] = NULL;
    }
    
    //Third byte
    f_sub->sub[0] = (TITUS_font *)SDL_malloc(sizeof(TITUS_font) * 256);
    if (f_sub->sub[0] == NULL) {
        sprintf(lasterror, "Error: Not enough memory to load fonts!\n");
        freepixelformat(&(pixelformat));
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    f_sub->type[0] = 1; //Malloced sub
    f_sub = (TITUS_font *)f_sub->sub[0];
    for (i = 0; i < 256; i++) {
        f_sub->type[i] = 0;
        f_sub->sub[i] = NULL;
    }
    
    //Fourth byte
    f_sub->sub[0] = (TITUS_font *)SDL_malloc(sizeof(TITUS_font) * 256);
    if (f_sub->sub[0] == NULL) {
        sprintf(lasterror, "Error: Not enough memory to load fonts!\n");
        freepixelformat(&(pixelformat));
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    f_sub->type[0] = 1; //Malloced sub
    f_sub = (TITUS_font *)f_sub->sub[0];
    for (i = 0; i < 256; i++) {
        f_sub->type[i] = 0;
        f_sub->sub[i] = NULL;
    }
*/

    //Font data
    for (i = 0; i < 10; i++) { //0-9
        if ((font->sub[i + CHAR_0] = SDL_LoadChar(fontdata, i, pixelformat)) == NULL) {
            freepixelformat(&(pixelformat));
            return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
        }
        font->type[i + CHAR_0] = 2; //Malloced surface
    }
    
    if ((font->sub[CHAR_EXCLAMATION] = SDL_LoadChar(fontdata, 10, pixelformat)) == NULL) { //"!"
        freepixelformat(&(pixelformat));
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    font->type[CHAR_EXCLAMATION] = 2; //Malloced surface

    if ((font->sub[CHAR_QUESTION] = SDL_LoadChar(fontdata, 11, pixelformat)) == NULL) { //"?"
        freepixelformat(&(pixelformat));
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    font->type[CHAR_QUESTION] = 2; //Malloced surface
    font_undefined = (SDL_Surface *)font->sub[CHAR_QUESTION];

    if ((font->sub[CHAR_DOT] = SDL_LoadChar(fontdata, 12, pixelformat)) == NULL) { //"."
        freepixelformat(&(pixelformat));
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    font->type[CHAR_DOT] = 2; //Malloced surface

    if ((font->sub[CHAR_DOLLAR] = SDL_LoadChar(fontdata, 13, pixelformat)) == NULL) { //"$"
        freepixelformat(&(pixelformat));
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    font->type[CHAR_DOLLAR] = 2; //Malloced surface

    if ((font->sub[CHAR_UNDERSCORE] = SDL_LoadChar(fontdata, 14, pixelformat)) == NULL) { //"_"
        freepixelformat(&(pixelformat));
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    font->type[CHAR_UNDERSCORE] = 2; //Malloced surface

    for (i = 0; i < 26; i++) { //A-Z
        if ((font->sub[i + CHAR_A] = SDL_LoadChar(fontdata, i + 15, pixelformat)) == NULL) {
            freepixelformat(&(pixelformat));
            return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
        }
        font->type[i + CHAR_A] = 2; //Malloced surface
    }

    for (i = 0; i < 26; i++) { //a-z
        font->sub[i + CHAR_a] = font->sub[i + CHAR_A]; //Use the same surface as the uppercase letter
        font->type[i + CHAR_a] = 3; //Surface pointer
    }
    
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, 12, 8, 0, 0, 0, 0); //" "
    if (surface == NULL) {
        sprintf(lasterror, "Error: Not enough memory to load fonts!\n");
        freepixelformat(&(pixelformat));
        return (TITUS_ERROR_NOT_ENOUGH_MEMORY);
    }
    copypixelformat(surface->format, pixelformat);
    tmpchar = (char *)surface->pixels;
    for (i = 0; i < 96; i++) {
        *tmpchar = 0x01;
        tmpchar++;
    }
    if ((font->sub[CHAR_SPACE] = SDL_DisplayFormat(surface)) == NULL) {
        sprintf(lasterror, "Error: Not enough memory to load fonts!\n");
        freepixelformat(&(pixelformat));
        return (0);
    }
    SDL_FreeSurface(surface);
    font->type[CHAR_SPACE] = 2; //Malloced surface

    //The first byte cannot begin with 10xxxxxx
    for (i = 0x80; i < 0xC0; i++) {
        font->type[i] = 4; //Invalid UTF-8
    }


    freepixelformat(&(pixelformat));

    return (0);
}

SDL_Surface * SDL_LoadChar(unsigned char * fontdata, int offset, SDL_PixelFormat * pixelformat){
    SDL_Surface *surface = NULL;
    SDL_Surface *surface2 = NULL;
    char *tmpchar;
    int i, j;
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, 12, 8, 0, 0, 0, 0);
    if (surface == NULL) {
        sprintf(lasterror, "Error: Not enough memory to load fonts!\n");
        return (NULL);
    }
    
    copypixelformat(surface->format, pixelformat);

    tmpchar = (char *)surface->pixels;
    for (i = offset * 48; i < offset * 48 + 12; i++) {//12
        for (j = 7; j >= 0; j--) {
            *tmpchar = (fontdata[i] >> j) & 0x01;
            *tmpchar += (fontdata[i + 12] >> j << 1) & 0x02;
            *tmpchar += (fontdata[i + 12 * 2] >> j << 2) & 0x04;
            *tmpchar += (fontdata[i + 12 * 3] >> j << 3) & 0x08;
            tmpchar++;
        }
    }
    surface2 = SDL_DisplayFormat(surface);
    if (surface2 == NULL) {
        sprintf(lasterror, "Error: Not enough memory to load fonts!\n");
        return (NULL);
    }
    SDL_FreeSurface(surface);
    return(surface2);
}


int freefonts(void) {
    freesubfont(font);
    return 0;
}

int freesubfont(TITUS_font *f_sub) {
    int i;
    for (i = 0; i < 256; i++) {
        if (f_sub->type[i] == 1) { //Malloced sub
            freesubfont(f_sub->sub[i]);
        } else if (f_sub->type[i] == 2) { //Malloced surface
            SDL_FreeSurface((SDL_Surface *)f_sub->sub[i]);
        }
    }
    free (f_sub);
    return 0;
}

/*
SDL_Surface * SDL_Text(char *text, bool inv_back){
    SDL_Surface *surface = NULL;
    SDL_Surface *charsurface = NULL;
    SDL_Palette *palette;
    char *tmpchar;
    //int fontsize = 1968; //41 characters 8*12, 16 color
    unsigned char *fontdata;
    int retval;
    int currentchar;
    int offset;
    unsigned int i;
    int j;
    SDL_Rect src, dest;

    retval = unSQZ(fontfile, &fontdata);

    if (retval < 0) {
        lasterrornr = retval;
        free (fontdata);
        return (NULL);
    }

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 8 * strlen(text), 12, 8, 0, 0, 0, 0);
    charsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, 12, 8, 0, 0, 0, 0);
    palette = (charsurface->format)->palette;
    if (palette) {
        palette->colors[0].r = 0 * 4; // Unused
        palette->colors[0].g = 63 * 4;
        palette->colors[0].b = 63 * 4;
        palette->colors[0].unused = 0 * 4; 

        palette->colors[1].r = 0 * 4; // Black
        palette->colors[1].g = 0 * 4;
        palette->colors[1].b = 0 * 4;
        palette->colors[1].unused = 0 * 4; 

        palette->colors[2].r = 63 * 4; // Unused
        palette->colors[2].g = 0 * 4;
        palette->colors[2].b = 0 * 4;
        palette->colors[2].unused = 0 * 4; 

        palette->colors[3].r = 0 * 4; // Unused
        palette->colors[3].g = 63 * 4;
        palette->colors[3].b = 0 * 4;
        palette->colors[3].unused = 0 * 4; 

        palette->colors[4].r = 0 * 4; // Unused
        palette->colors[4].g = 0 * 4;
        palette->colors[4].b = 63 * 4;
        palette->colors[4].unused = 0 * 4; 

        palette->colors[5].r = 60 * 4; // Brightest yellow
        palette->colors[5].g = 60 * 4;
        palette->colors[5].b = 28 * 4;
        palette->colors[5].unused = 0 * 4; 

        palette->colors[6].r = 60 * 4;
        palette->colors[6].g = 60 * 4;
        palette->colors[6].b = 12 * 4;
        palette->colors[6].unused = 0 * 4; 

        palette->colors[7].r = 60 * 4;
        palette->colors[7].g = 56 * 4;
        palette->colors[7].b = 0 * 4;
        palette->colors[7].unused = 0 * 4; 

        palette->colors[8].r = 56 * 4;
        palette->colors[8].g = 48 * 4;
        palette->colors[8].b = 0 * 4;
        palette->colors[8].unused = 0 * 4; 

        palette->colors[9].r = 52 * 4;
        palette->colors[9].g = 44 * 4;
        palette->colors[9].b = 0 * 4;
        palette->colors[9].unused = 0 * 4; 

        palette->colors[10].r = 52 * 4;
        palette->colors[10].g = 36 * 4;
        palette->colors[10].b = 0 * 4;
        palette->colors[10].unused = 0 * 4; 

        palette->colors[11].r = 44 * 4;
        palette->colors[11].g = 36 * 4;
        palette->colors[11].b = 0 * 4;
        palette->colors[11].unused = 0 * 4; 

        palette->colors[12].r = 36 * 4;
        palette->colors[12].g = 28 * 4;
        palette->colors[12].b = 0 * 4;
        palette->colors[12].unused = 0 * 4; 

        palette->colors[13].r = 32 * 4;
        palette->colors[13].g = 24 * 4;
        palette->colors[13].b = 0 * 4;
        palette->colors[13].unused = 0 * 4; 

        palette->colors[14].r = 24 * 4;
        palette->colors[14].g = 20 * 4;
        palette->colors[14].b = 0 * 4;
        palette->colors[14].unused = 0 * 4; 

        palette->colors[15].r = 20 * 4; // Darkest yellow
        palette->colors[15].g = 16 * 4;
        palette->colors[15].b = 0 * 4;
        palette->colors[15].unused = 0 * 4; 

        palette->ncolors = 16;
    }	

    if (inv_back) //Invisible (transparent) background
        SDL_SetColorKey(surface, SDL_SRCCOLORKEY|SDL_RLEACCEL, 1);

    SDL_SetColors(surface, palette->colors, 0, 16);

    src.x = 0;
    src.y = 0;
    src.w = charsurface->w;
    src.h = charsurface->h;

    dest.x = 0;
    dest.y = 0;
    dest.w = charsurface->w;
    dest.h = charsurface->h;

    tmpchar = (char *)charsurface->pixels;

    for (currentchar = 0; currentchar < strlen(text); currentchar++) {
        if (((text[currentchar] & 0xFF) >= CHAR_a) && ((text[currentchar] & 0xFF) <= CHAR_z)) // "a-z"
            offset = (text[currentchar] & 0xFF) - CHAR_a + 15;
        else if (((text[currentchar] & 0xFF) >= CHAR_A) && ((text[currentchar] & 0xFF) <= CHAR_Z)) // "A-Z"
            offset = (text[currentchar] & 0xFF) - CHAR_A + 15;
        else if (((text[currentchar] & 0xFF) >= CHAR_0) && ((text[currentchar] & 0xFF) <= CHAR_9)) // "0-9"
            offset = (text[currentchar] & 0xFF) - CHAR_0;
        else if ((text[currentchar] & 0xFF) == CHAR_EXCLAMATION) // "!"
            offset = 10;
        else if ((text[currentchar] & 0xFF) == CHAR_QUESTION) // "?"
            offset = 11;
        else if ((text[currentchar] & 0xFF) == CHAR_DOT) // "."
            offset = 12;
        else if ((text[currentchar] & 0xFF) == CHAR_DOLLAR) // "$"
            offset = 13;
        else if ((text[currentchar] & 0xFF) == CHAR_UNDERSCORE) // "_"
            offset = 14;
        else {
            for (i = 0; i < 96; i++) {
                *tmpchar = 0x01;
                tmpchar++;
            }
            tmpchar -= 96;
            SDL_BlitSurface(charsurface, &src, surface, &dest);
            dest.x += 8;
            continue;
        }

        for (i = offset * 48; i < offset * 48 + 12; i++) {//12
            for (j = 7; j >= 0; j--) {
                *tmpchar = (fontdata[i] >> j) & 0x01;
                *tmpchar += (fontdata[i + 12] >> j << 1) & 0x02;
                *tmpchar += (fontdata[i + 12 * 2] >> j << 2) & 0x04;
                *tmpchar += (fontdata[i + 12 * 3] >> j << 3) & 0x08;
                tmpchar++;
            }
        }

        tmpchar -= 96;

        SDL_BlitSurface(charsurface, &src, surface, &dest);
        dest.x += 8;

    }

done:
    free (fontdata);
    SDL_FreeSurface(charsurface);
    return(surface);
}

int SDL_Print_Text(char *text, int x, int y, bool inv_back){
    SDL_Rect src, dest;
    SDL_Surface *surface = SDL_Text(text, inv_back);

    if (surface == NULL)
        return (lasterrornr);

    SDL_Surface *image = SDL_DisplayFormat(surface);

    src.x = 0;
    src.y = 0;
    src.w = image->w;
    src.h = image->h;
	
    dest.x = x;
    dest.y = y;
    dest.w = image->w;
    dest.h = image->h;

    SDL_BlitSurface(image, &src, screen, &dest);
    SDL_FreeSurface(image);
    SDL_FreeSurface(surface);
    return (0);
}
*/

int SDL_Print_Text(uint8 *text, int x, int y){
    TITUS_font *f_sub;
    uint8 i, j;
    SDL_Rect src, dest;
    SDL_Surface *image;
    
    src.x = 0;
    src.y = 0;
    dest.x = x;
    dest.y = y;

    for (i = 0; i < strlen(text); i++) {
        j = i;
        f_sub = font;
        if ((text[i] & 0xC0) == 0x80) { //The first character cannot start with 10xxxxxx
            sprintf(lasterror, "Error: Invalid UTF-8!\n");
            return (TITUS_ERROR_INVALID_UTF8);
        }
        while (f_sub->type[text[i]] == 1) { //Sub
            if (i == strlen(text) - 1) {
                sprintf(lasterror, "Error: Invalid UTF-8!\n");
                return (TITUS_ERROR_INVALID_UTF8);
            }
            i++;
            if ((text[i] & 0xC0) != 0x80) { //The following characters must begin with 10xxxxxx
                sprintf(lasterror, "Error: Invalid UTF-8!\n");
                return (TITUS_ERROR_INVALID_UTF8);
            }
            f_sub = (TITUS_font *)f_sub->sub[text[i]];
        }
        
        switch (f_sub->type[text[i]]) {
        case 0: //Undefined character
            image = font_undefined;
            src.w = image->w;
            src.h = image->h;
            SDL_BlitSurface(image, &src, screen, &dest);
            dest.x += 8;
            if (text[j] > 0xBF) { //If first letter is larger than 10111111: this is multibyte. Make sure "i" is in the last byte
                do {
                    if (j == strlen(text) - 1) {
                        sprintf(lasterror, "Error: Invalid UTF-8!\n");
                        return (TITUS_ERROR_INVALID_UTF8);
                    }
                    j++;
                } while ((text[j] & 0xC0) == 0x80); //Loop while continuing bytes begin with 10xxxxxx
                j--;
                if (j < i) { //Error in the data structure
                    sprintf(lasterror, "Error: Invalid font data!\n");
                    return (TITUS_ERROR_INVALID_FILE);
                }
                i = j;
            }
            break;
        case 2: //Malloced surface
        case 3: //Surface pointer
            image = (SDL_Surface *)f_sub->sub[text[i]];
            src.w = image->w;
            src.h = image->h;
            SDL_BlitSurface(image, &src, screen, &dest);
            dest.x += 8;
            break;
        case 4: //Invalid UTF-8
            sprintf(lasterror, "Error: Invalid UTF-8!\n");
            return (TITUS_ERROR_INVALID_UTF8);
        }
    }
    return (0);
}

int viewintrotext(){
    int retval;
    char tmpstring[41];
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    SDL_Print_Text("     YEAAA . . .", 0, 5 * 12);
    sprintf(tmpstring, "YOU ARE STILL PLAYING MOKTAR IN %d !!", timeinfo->tm_year + 1900);
    SDL_Print_Text(tmpstring, 0, 6 * 12);
    SDL_Print_Text(" PROGRAMMED IN 1991 ON AT .286 12MHZ.", 0, 12 * 12);
    SDL_Print_Text("   . . . ENJOY MOKTAR ADVENTURE !!", 0, 13 * 12);

    SDL_Flip(screen);

    retval = waitforbutton();
    if (retval < 0)
        return retval;

    SDL_Print_Text("     YEAAA . . .", 0, 5 * 12);
    sprintf(tmpstring, "YOU ARE STILL PLAYING MOKTAR IN %d !!", timeinfo->tm_year + 1900);
    SDL_Print_Text(tmpstring, 0, 6 * 12);
    SDL_Print_Text("REPROGRAMMED IN 2011 ON X86_64 2.40 GHZ.", 0, 12 * 12);
    SDL_Print_Text("   . . . ENJOY MOKTAR ADVENTURE !!", 0, 13 * 12);

    SDL_Flip(screen);

    retval = waitforbutton();
    if (retval < 0)
        return retval;

    return (0);
}
