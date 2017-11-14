#ifndef DRAWING_H
#define DRAWING_H

#include <SDL/SDL.h>


void draw_rect_fill(SDL_Surface *img, int x, int y, int h, int w);


void draw_line(SDL_Surface *img, Uint32 color, int xstart, int xend, int y);


void draw_rect_outline(SDL_Surface *img, int x, int y, int h, int w, Uint32 c);

#endif
