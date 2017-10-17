#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <err.h>

void init_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1, "Couldn't initialize SDL: %s.\n", SDL_GetError());
    SDL_WM_SetCaption("Image Processing", NULL);
}

void print_mouse_pos(void) {
    int a = 0;
    int b = 0;
    SDL_GetMouseState(&a, &b);
    printf("Position x : %d y : %d\n", a, b);
}

void wait_for_keypressed(void) {
    SDL_Event event;
    for (;;) {
        SDL_PollEvent(&event);
        switch (event.type) {
        case SDL_MOUSEBUTTONUP:
            print_mouse_pos();
            return;
        case SDL_KEYDOWN:
            return;
        default:
            break;
        }
    }
}

SDL_Surface *load_image(char *path) {
    SDL_Surface *img;
    img = IMG_Load(path);
    if (!img)
        errx(3, "can't load %s: %s", path, IMG_GetError());
    return img;
}

void display(SDL_Surface **screen, SDL_Surface *img) {
    if (!(*screen))
        *screen =
            SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE | SDL_ANYFORMAT);
    if (img->w != (*screen)->w || img->h != (*screen)->h)
        *screen =
            SDL_SetVideoMode(img->w, img->h, 0, SDL_SWSURFACE | SDL_ANYFORMAT);
    SDL_BlitSurface(img, NULL, *screen, NULL);
    SDL_Flip(*screen);
}

void display_and_wait(SDL_Surface **screen, SDL_Surface *img) {
    display(screen, img);
    wait_for_keypressed();
}
