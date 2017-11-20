/**
 * \file drawing.h
 * \brief Draw shapes
 */
#ifndef DRAWING_H
#define DRAWING_H

#include <SDL/SDL.h>

/**
 * \brief Draw a full rectangle
 * \param img Pointer to the SDL_Surface to modify
 * \param x X coordinate of the top left corner of the rectangle
 * \param y Y coordinate of the top left corner of the rectangle
 * \param h Height of the rectangle
 * \param w Width of the rectangle
 */
void draw_rect_fill(SDL_Surface *img, int x, int y, int h, int w);

/**
 * \brief Draw a horizontal line
 * \param img Pointer to the SDL_Surface to modify
 * \param color Color of the line
 * \param xstart X coordinate of the start of the line
 * \param xend X coordinate of the end of the line
 * \param y Y coordinate of the line
 */
void draw_line(SDL_Surface *img, Uint32 color, int xstart, int xend, int y);

/**
 * \brief Draw the outline of a rectangle
 * \param img Pointer to the SDL_Surface to modify
 * \param x X coordinate of the top left corner of the rectangle
 * \param y Y coordinate of the top left corner of the rectangle
 * \param h Height of the rectangle
 * \param w Width of the rectangle
 * \param c Color of the rectangle
 */
void draw_rect_outline(SDL_Surface *img, int x, int y, int h, int w, Uint32 c);

#endif
