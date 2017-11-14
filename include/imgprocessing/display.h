#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL/SDL.h>

void init_sdl(void);

SDL_Surface *load_image(char *path);

// Displays an image img to the pointer to the surface screen
// display(&screen, img);
void display(SDL_Surface **screen, SDL_Surface *img);

// Displays an image to the screen, and waits for a keypress
void display_and_wait(SDL_Surface **screen, SDL_Surface *img);

#endif
