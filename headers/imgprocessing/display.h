#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL/SDL.h>

void init_sdl(void);

SDL_Surface *load_image(char *path);

void display(SDL_Surface **screen, SDL_Surface *img);

void display_and_wait(SDL_Surface **screen, SDL_Surface *img);

#endif
