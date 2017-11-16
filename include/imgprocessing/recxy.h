/**
 * \file recxy.h
 * \brief Recursive X-Y cuts algorithm
 */
#ifndef RECXY_H
#define RECXY_H
#include "misc/bool_matrix.h"
#include "imgprocessing/rxy_bintree.h"
#include <SDL/SDL.h>

/**
 * \brief Apply the RXY algorithm to a matrix
 * \param Pointer to t_bool_matrix
 * \param onlyh If set to true, will only apply horizontal cuts
 * \return Pointer to struct rxy_bintree
 */
struct rxy_bintree *recxy(t_bool_matrix *img, bool onlyh);

/**
 * \brief Apply recursive Y cuts
 * \param Pointer to struct rxy_bintree that holds the parent
 * \param cut Width of the cut
 */
void _recxy_only_v(struct rxy_bintree *parent, int cut);

/**
 * \brief Apply a function on leaves
 * \param b Pointer to struct rxy_bintree
 * \param f Pointer to void function
 */
void apply_on_leaves(struct rxy_bintree *b, void (*f)(struct rxy_bintree *));

/**
 * \brief Apply a function on leaves and modiify them
 * \param b Pointer to struct rxy_bintree
 * \param f Pointer to a function returing a pointer to t_bool_matrix
 */
void apply_on_leaves2(struct rxy_bintree *b, t_bool_matrix *(*f)(t_bool_matrix *));

/**
 * \brief Trim white cols of the value of the bintree
 * \param b Pointer to struct rxy_bintree
 */
void trim_white_cols(struct rxy_bintree *b);

/**
 * \brief Trim white lines of the value of the bintree
 * \param b Pointer to struct rxy_bintree
 */
void trim_white_lines(struct rxy_bintree *b);

#endif
