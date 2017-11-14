#ifndef PROCESSING_H
#define PROCESSING_H

#include "../misc/bool_matrix.h"
#include <SDL/SDL.h>

void gray_level(SDL_Surface *img);

void inverse(SDL_Surface *img);

void binarize(SDL_Surface *img, int threshold);

int otsu(SDL_Surface *img);

void contrast(SDL_Surface *img, int value);

SDL_Surface *blur(SDL_Surface *input, int radius);

void divide_and_conquer(SDL_Surface *img);

SDL_Surface *rlsa(SDL_Surface *img, int c, SDL_Surface **h, SDL_Surface **v);

t_bool_matrix *surface_to_matrix(SDL_Surface *img);

void matrix_to_surface(SDL_Surface *surface, t_bool_matrix *m);

t_bool_matrix *hrlsa_bm(t_bool_matrix *img, int c);

t_bool_matrix *vrlsa_bm(t_bool_matrix *img, int c);

t_bool_matrix *recombine_bm(t_bool_matrix *a, t_bool_matrix *b);

SDL_Surface *copy_surface(SDL_Surface *img);
t_bool_matrix *rlsa_bm(t_bool_matrix *img, int c, t_bool_matrix **h, t_bool_matrix **v);
#endif
