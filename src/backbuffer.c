#include "backbuffer.h"

SDL_Surface *screen; //Backbuffer
SDL_Surface *tilescreen; //Tile screen

void SDL_Flip(SDL_Surface * screen) {
    SDL_UpdateWindowSurface(window);
}
