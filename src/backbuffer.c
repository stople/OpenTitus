#include "backbuffer.h"

SDL_Surface *screen; //Backbuffer
SDL_Surface *tilescreen; //Tile screen

void SDL_Flip(SDL_Surface * screen) {
    SDL_Texture *frame = SDL_CreateTextureFromSurface(renderer, screen);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, frame, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(frame);
}
