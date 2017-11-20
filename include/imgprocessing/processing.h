/**
 * \file processing.h
 * \brief Image processing algorithms
 */
#ifndef PROCESSING_H
#define PROCESSING_H

#include "../misc/bool_matrix.h"
#include <SDL/SDL.h>

/**
 * \brief Convert an image to grayscale
 * \param img Pointer to SDL_Surface
 */
void gray_level(SDL_Surface *img);

/**
 * \brief Invert the colors of an image
 * \param img Pointer to SDL_Surface
 */
void inverse(SDL_Surface *img);

/**
 * \brief Binarize an image
 * \param img Pointer to SDL_Surface
 * \param threshold Threshold to use for the binarization
 */
void binarize(SDL_Surface *img, int threshold);

/**
 * \brief Apply Otsu's algorithm on a grayscaled image
 * \param img Pointer to SDL_Surface
 */
int otsu(SDL_Surface *img);

/**
 * \brief Change the contrast of an image
 * \param img Pointer to SDL_Surface
 * \param value Contrast delta 
 */
void contrast(SDL_Surface *img, int value);

/**
 * \brief Blur an image
 * \param input Pointer to SDL_Surface
 * \param radius Radius of the blur
 * \return Pointer to a blured SDL_Surface 
 */
SDL_Surface *blur(SDL_Surface *input, int radius);

/**
 * \brief Apply the rlsa algorithm
 * \param img Pointer to the input SDL_Surface
 * \param c Radius
 * \param h Pointer to pointer of SDL_Surface that will be set to the horizontal application of the rlsa algorithm
 * \param v Pointer to pointer of SDL_Surface that will be set to the vertical application of the algorithm
 * \return Pointer to SDL_Surface that holds the result
 */
SDL_Surface *rlsa(SDL_Surface *img, int c, SDL_Surface **h, SDL_Surface **v);

/**
 * \brief Convert an image to a matrix of booleans
 * \param img Pointer to SDL_Surface
 * \return Pointer to t_bool_matrix
 */
t_bool_matrix *surface_to_matrix(SDL_Surface *img);

/**
 * \brief Convert a boolean matrix to an SDL_Surface
 * \param surface Pointer to SDL_Surface
 * \param m Pointer to t_bool_matrix (Expeted to be big enough to hold the SDL_Surface)
 */
void matrix_to_surface(SDL_Surface *surface, t_bool_matrix *m);

/**
 * \brief Apply horizontal RLSA to a matrix
 * \param img Pointer to t_bool_matrix
 * \param c Radius
 * \return Pointer to t_bool_matrix
 */
t_bool_matrix *hrlsa_bm(t_bool_matrix *img, int c);

/**
 * \brief Apply vertical RLSA to a matrix
 * \param img Pointer to t_bool_matrix
 * \param c Radius
 * \return Pointer to t_bool_matrix
 */
t_bool_matrix *vrlsa_bm(t_bool_matrix *img, int c);

/**
 * \brief Recombine two matrices into one
 * \param a Pointer to t_bool_matrix 
 * \param b Pointer to t_bool_matrix
 * \return Pointer to t_bool_matrix
 */
t_bool_matrix *recombine_bm(t_bool_matrix *a, t_bool_matrix *b);

/**
 * \brief Copy an image
 * \param img Pointer to SDL_Surface
 * \return Pointer to SDL_Surface that holds a copy of the input
 */
SDL_Surface *copy_surface(SDL_Surface *img);

/**
 * \brief Apply RLSA to a matrix
 * \param img Pointer to t_bool_matrix
 * \param c Radius
 * \param h Pointer to pointer to t_bool_matrix that will be modified to hold the application of hrlsa_bm
 * \param v Pointer to pointer of t_bool_matrix that will be modified to hold the application of vrlsa_bm
 * \return Pointer to t_bool_matrix
 */
t_bool_matrix *rlsa_bm(t_bool_matrix *img, int c, t_bool_matrix **h, t_bool_matrix **v);

#endif
