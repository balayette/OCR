#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <err.h>
#include "../../headers/imgprocessing/processing.h"

void print_mouse_pos(){
    int a = 0;
    int b = 0;
    SDL_GetMouseState(&a, &b);
    printf("Position x : %d y : %d\n", a, b);
}

void wait_for_keypressed(void) {
    SDL_Event             event;
    for (;;) {
        SDL_PollEvent( &event );
        switch (event.type) {
        case SDL_MOUSEBUTTONUP:
            print_mouse_pos();
            return;
        case SDL_KEYDOWN:
            return;
        default:break;
        }
    }
}

void init_sdl(void) {
    if( SDL_Init(SDL_INIT_VIDEO)==-1 ) {
        errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
    }
}

SDL_Surface* load_image(char *path) {
    SDL_Surface          *img;
    img = IMG_Load(path);
    if (!img)
        errx(3, "can't load %s: %s", path, IMG_GetError());
    return img;
}

SDL_Surface* display_image(SDL_Surface *img) {
    SDL_Surface          *screen;
    screen = SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE|SDL_ANYFORMAT);
    if ( screen == NULL ) {
        errx(1, "Couldn't set %dx%d video mode: %s\n",
             img->w, img->h, SDL_GetError());
    }

    if(SDL_BlitSurface(img, NULL, screen, NULL) < 0)
        warnx("BlitSurface error: %s\n", SDL_GetError());

    SDL_UpdateRect(screen, 0, 0, img->w, img->h);

    wait_for_keypressed();

    return screen;
}

int main(int argc, char *argv[]){
    (void)argc;
    printf("Loading an image from %s\n", argv[1]);
    SDL_Surface *img = load_image(argv[1]);
    gray_level(img);
    int t = otsu(img);
    binarize(img, t);
    display_image(img);
    free(img);
    SDL_Quit();
    return 0;
}
