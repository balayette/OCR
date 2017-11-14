#ifndef BOX_H
#define BOX_H


#include <stdbool.h>
#include <SDL/SDL.h>

typedef struct s_box {
    int x;
    int y;
    int height;
    int width;
    bool is_top_of_line;
    int line_span;
} t_box;


bool check_black_percent(SDL_Surface *img, t_box *box);


void draw_box(SDL_Surface *img, t_box *box);


t_box *new_box(int x, int y, int height, int width);

#endif
