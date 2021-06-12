#include "window.h"
#include "settings.h"
#include "tituserror.h"

namespace {
    const char* getGameTitle() {
        switch(game) {
            case GameType::Titus:
                return "OpenTitus";
            case GameType::Moktar:
                return "OpenMoktar";
            default:
                return "Something else...";
        }
    }
}

namespace Window {

bool fullscreen = false;
bool display_tilescreen = false;

SDL_Surface *screen;
SDL_Surface *tilescreen;
SDL_Window *window;
SDL_Renderer *renderer;

void toggle_fullscreen() {
    if(!fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        fullscreen = true;
    }
    else {
        SDL_SetWindowFullscreen(window, 0);
        fullscreen = false;
    }
}

void toggle_buffers() {
    display_tilescreen = !display_tilescreen;
}

int init() {
    Uint32 windowflags = 0;
    int w;
    int h;
    switch (videomode) {
        default:
        case 0: //window mode
            w = 960;
            h = 600;
            windowflags = SDL_WINDOW_RESIZABLE;
            fullscreen = false;
            break;
        case 1: // fullscreen
            w = 0;
            h = 0;
            windowflags = SDL_WINDOW_FULLSCREEN_DESKTOP;
            fullscreen = true;
            break;
    }

    window = SDL_CreateWindow(
        getGameTitle(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        w,
        h,
        windowflags
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Unable to set video mode: %s\n", SDL_GetError());
        return TITUS_ERROR_SDL_ERROR;
    }

    // screen = SDL_GetWindowSurface(window);
    screen = SDL_CreateRGBSurfaceWithFormat(0, 320, 200, 32, SDL_GetWindowPixelFormat(window));

    SDL_RenderSetLogicalSize(renderer, 320, 200);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    return 0;
}

void paint() {
    SDL_Texture *frame;
    if(display_tilescreen && tilescreen) {
        frame = SDL_CreateTextureFromSurface(renderer, tilescreen);
    }
    else if (screen) {
        frame = SDL_CreateTextureFromSurface(renderer, screen);
    }
    else {
        return;
    }
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, frame, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(frame);
}

}
