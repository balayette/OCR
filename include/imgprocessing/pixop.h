/**
 * \file pixop.h
 * \brief Pixel operations
*/
#ifndef PIXOP_H
#define PIXOP_H

#include <SDL/SDL.h>

/**
 * \brief Retrieve a pixel
 * \param surface Pointer to SDL_Surface
 * \param x X coordinate of the pixel
 * \param y Y coordinate of the pixel
 * \return Pixel
 */
Uint32 getpixel(SDL_Surface *surface, unsigned x, unsigned y);

/**
 * \brief Write a pixel
 * \param surface Pointer to SDL_Surface
 * \param x X coordinate of the pixel
 * \param y Y coordinate of the pixel
 * \param pixel Pixel to write
 */
void putpixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

#endif
