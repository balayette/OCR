#include "../../headers/imgprocessing/processing.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "../../headers/imgprocessing/display.h"


int main(int argc, char *argv[]) {
    (void)argc;
    SDL_Surface *screen = NULL;
    SDL_Surface *img = load_image(argv[1]);
    SDL_Surface *img2 = load_image(argv[2]);

    display_and_wait(&screen, img);
    display_and_wait(&screen, img2);
    display_and_wait(&screen, img);

    SDL_FreeSurface(img);
    SDL_FreeSurface(img2);
    SDL_FreeSurface(screen);
    SDL_Quit();
    return 0;
}
