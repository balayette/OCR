#include <stdbool.h>
#include <SDL/SDL.h>
#include "imgprocessing/pixop.h"
#include "imgprocessing/box.h"
#include "imgprocessing/drawing.h"

bool check_black_percent(SDL_Surface *img, t_box *box) {
    /*
      This function should take a percentage as input. Will be done later
    */
    int startx = box->x;
    int starty = box->y;
    int endx = box->x + (box->width);
    int endy = box->y + (box->height);
    Uint8 r, g, b;
    int count = 0;
    for (int y = starty; y < endy; y++) {
        for (int x = startx; x < endx; x++) {
            Uint32 pix = getpixel(img, x, y);
            SDL_GetRGB(pix, img->format, &r, &g, &b);
            if (r == 0)
                count++;
        }
    }
    return count != 0;
}

void draw_box(SDL_Surface *img, t_box *box) {
    Uint32 c = SDL_MapRGB(img->format, 255, 0, 0);
    draw_rect_outline(img, box->x, box->y, box->height, box->width, c);
}

t_box *new_box(int x, int y, int height, int width) {
    t_box *box = malloc(sizeof(t_box));
    box->x = x;
    box->y = y;
    box->height = height;
    box->is_top_of_line = false;
    box->width = width;
    box->line_span = 0;
    return box;
}
