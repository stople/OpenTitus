#include "window.h"
#include "settings.h"
#include "tituserror.h"

namespace Window {

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

Uint32 black = 0;
}

bool fullscreen = false;

SDL_Surface *screen;
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
    black = SDL_MapRGB(screen->format, 0, 0, 0);

    SDL_RenderSetLogicalSize(renderer, 320, 200);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    return 0;
}

void clear(const SDL_Rect * rect) {
    SDL_FillRect(screen, rect, black);
}

void render() {
    if(!screen) {
        return;
    }
    SDL_Texture *frame = SDL_CreateTextureFromSurface(renderer, screen);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, frame, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(frame);
}

}
