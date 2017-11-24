/**
 * \file matrixop.h
 * \brief Operations on matrices
 */
#ifndef MATRIXOP_H
#define MATRIXOP_H

#include "misc/bool_matrix.h"
#include <stdbool.h>
#include <SDL/SDL.h>

int find_h_cut(t_bool_matrix *mat, int span);

int find_v_cut(t_bool_matrix *mat, int span);

bool is_white(t_bool_matrix *mat, int x, int y, int h, int w);

bool is_white_line(t_bool_matrix *mat, int y);

bool is_white_col(t_bool_matrix *mat, int x);

t_bool_matrix *after_v(t_bool_matrix *mat, int x);

t_bool_matrix *before_v(t_bool_matrix *mat, int x);

t_bool_matrix *after_h(t_bool_matrix *mat, int y);

t_bool_matrix *before_h(t_bool_matrix *mat, int y);

t_bool_matrix *rlsa_bm(t_bool_matrix *img, int c, t_bool_matrix **h,
                       t_bool_matrix **v);

t_bool_matrix *vrlsa_bm(t_bool_matrix *img, int c);

t_bool_matrix *recombine_bm(t_bool_matrix *a, t_bool_matrix *b);

t_bool_matrix *_trim_lines_before(t_bool_matrix *mat);

t_bool_matrix *_trim_lines_after(t_bool_matrix *mat);
t_bool_matrix *trim_lines(t_bool_matrix *mat);

t_bool_matrix *hrlsa_bm(t_bool_matrix *img, int c);

t_bool_matrix *vrlsa_bm(t_bool_matrix *img, int c);

void matrix_to_surface(SDL_Surface *surface, t_bool_matrix *m);

t_bool_matrix *surface_to_matrix(SDL_Surface *img);

t_bool_matrix *_trim_cols_before(t_bool_matrix *mat);

t_bool_matrix *_trim_lines_before(t_bool_matrix *mat);

#endif
