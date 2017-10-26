#ifndef RECXY_H
#define RECXY_H
#include "../misc/bool_matrix.h"

#include <SDL/SDL.h>

typedef struct s_rxy_bintree t_rxy_bintree;
struct s_rxy_bintree {
    int x;
    int y;
    t_bool_matrix *values;
    t_rxy_bintree *left;
    t_rxy_bintree *right;
};

typedef enum e_print_position {
    PREORDER,
    INORDER,
    POSTORDER
} T_PRINT_POSITION;

t_rxy_bintree *create_rxy_bintree(t_bool_matrix *value, int x, int y);

int bcount(t_rxy_bintree *b);

int bcount_leafs(t_rxy_bintree *b);

t_rxy_bintree *recxy(t_bool_matrix *img, bool onlyh);

void depth_first_display(SDL_Surface **screen, Uint32 flags, SDL_PixelFormat *fmt, t_rxy_bintree *b);

void depth_first_display_leaves(SDL_Surface **screen, Uint32 flags, SDL_PixelFormat *fmt, t_rxy_bintree *b);

void draw_boxes_leaves(SDL_Surface *img, t_rxy_bintree *b, int red, int green, int blue);

void _recxy_only_v(t_rxy_bintree *parent, int cut);

void apply_on_leaves(t_rxy_bintree *b, void (*f)(t_rxy_bintree *));
void apply_on_leaves2(t_rxy_bintree *b, t_bool_matrix *(*f)(t_bool_matrix *));
void trim_white_cols(t_rxy_bintree *b);
void trim_white_lines(t_rxy_bintree *b);
#endif
