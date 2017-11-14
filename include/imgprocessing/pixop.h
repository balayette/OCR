#ifndef PIXOP_H
#define PIXOP_H

#include <SDL/SDL.h>

Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y);
void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

#endif
