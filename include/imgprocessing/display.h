/** 
 * \file display.h
 * \brief Display surfaces
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL/SDL.h>

/**
 * \bief Init the SDL
 */
void init_sdl(void);

/**
 * \brief Load an SDL surface from a file
 * \param path Path to the image
 * \return Pointer to SDL_Surface
 */
SDL_Surface *load_image(char *path);

/**
 * \brief Display an image to the screen
 * \param screen Pointer to a pointer to SDL_Surface, that holds the display
 * \param img Pointer to the SDL_Surface to display
 */
void display(SDL_Surface **screen, SDL_Surface *img);


/**
 * \brief Display an image to the screen and wait for input
 * \param screen Pointer to pointer to SDL_Surface that holds the display
 * \parmam img Pointer to SDL_Surface to display
 */
void display_and_wait(SDL_Surface **screen, SDL_Surface *img);

#endif
