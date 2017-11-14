#ifndef RECXY_H
#define RECXY_H
#include "misc/bool_matrix.h"
#include "imgprocessing/rxy_bintree.h"
#include <SDL/SDL.h>

struct rxy_bintree *recxy(t_bool_matrix *img, bool onlyh);

void _recxy_only_v(struct rxy_bintree *parent, int cut);

void apply_on_leaves(struct rxy_bintree *b, void (*f)(struct rxy_bintree *));

void apply_on_leaves2(struct rxy_bintree *b, t_bool_matrix *(*f)(t_bool_matrix *));

void trim_white_cols(struct rxy_bintree *b);

void trim_white_lines(struct rxy_bintree *b);

#endif
