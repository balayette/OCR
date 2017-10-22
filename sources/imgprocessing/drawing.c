#include <SDL/SDL.h>
#include "../../headers/imgprocessing/pixop.h"

void draw_rect_fill(SDL_Surface *img, int x, int y, int h, int w) {
    SDL_Rect *r = malloc(sizeof(SDL_Rect));
    r->x = x;
    r->y = y;
    r->h = h;
    r->w = w;
    SDL_FillRect(img, r, SDL_MapRGB(img->format, 255, 0, 0));
    free(r);
}

void draw_line(SDL_Surface *img, Uint32 color, int xstart, int xend, int y) {
    for (int i = xstart; i < xend && i < img->w && y < img->h; i++)
        putpixel(img, i, y, color);
}
void draw_rect_outline(SDL_Surface *img, int x, int y, int h, int w, Uint32 c) {
    draw_line(img, c, x, x + w, y);
    draw_line(img, c, x, x + w, y + h);
    for (int i = 0; i < h; i++) {
        putpixel(img, x, y + i, c);
        putpixel(img, x + w, y + i, c);
    }
}
