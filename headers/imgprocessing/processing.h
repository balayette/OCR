#ifndef PROCESSING_H
#define PROCESSING_H

#include <SDL/SDL.h>

void gray_level(SDL_Surface *img);

void inverse(SDL_Surface *img);

void binarize(SDL_Surface *img, int threshold);

int otsu(SDL_Surface *img);

#endif
