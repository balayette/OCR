#ifndef PROCESSING_H
#define PROCESSING_H

#include <SDL/SDL.h>

void gray_level(SDL_Surface *img);

void inverse(SDL_Surface *img);

void binarize(SDL_Surface *img, int threshold);

int otsu(SDL_Surface *img);

void contrast(SDL_Surface *img, int value);

SDL_Surface *blur(SDL_Surface *input, int radius);

void divide_and_conquer(SDL_Surface *img);
SDL_Surface *rlsa(SDL_Surface *img, int c, SDL_Surface **h, SDL_Surface **v);
#endif
