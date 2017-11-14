#ifndef RXY_BINTREE_H
#define RXY_BINTREE_H

#include "misc/bool_matrix.h"
#include <SDL/SDL.h>

struct rxy_bintree {
    int x;
    int y;
    t_bool_matrix *values;
    struct rxy_bintree *left;
    struct rxy_bintree *right;
};

enum PRINT_POSITION { PREORDER, INORDER, POSTORDER };

struct rxy_bintree *create_rxy_bintree(t_bool_matrix *value, int x, int y);

int bcount(struct rxy_bintree *b);

int bcount_leaves(struct rxy_bintree *b);

struct rxy_bintree *recxy(t_bool_matrix *img, bool onlyh);

void df_display(SDL_Surface **screen, Uint32 flags, SDL_PixelFormat *fmt,
                struct rxy_bintree *b);

void df_display_leaves(SDL_Surface **screen, Uint32 flags, SDL_PixelFormat *fmt,
                       struct rxy_bintree *b);

void draw_boxes_leaves(SDL_Surface *img, struct rxy_bintree *b, int red,
                       int green, int blue);

void apply_on_leaves(struct rxy_bintree *b, void (*f)(struct rxy_bintree *));

void apply_on_leaves2(struct rxy_bintree *b,
                      t_bool_matrix *(*f)(t_bool_matrix *));

#endif
