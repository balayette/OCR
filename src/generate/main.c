#include "imgprocessing/pixop.h"
#include "misc/bool_matrix.h"
#include <stdio.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "imgprocessing/display.h"
#include "imgprocessing/processing.h"
#include "matrix/matrixop.h"

SDL_Surface *new_surface(int h, int w) {
    return SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
}

void generate(char *font, char *path, int index){
    SDL_Surface *letter = NULL;
    TTF_Font *f = TTF_OpenFont(font, 20);
    SDL_Color black = {0, 0, 0, 0};
    SDL_Color white = {255, 255, 255, 0};
    char str[2] = {' ', '\0'};
    for(char l = 'A'; l <= 'Z'; l++){
        str[0] = l;
        letter = TTF_RenderText_Shaded(f, str, black, white);
        t_bool_matrix *mat = surface_to_matrix(letter);
        t_bool_matrix *trim = trim_lines(mat);
        M_bool_FREE(mat);
        path[index] = l;
        save_bool_matrix(path, trim);
    }
}

int main(){
    init_sdl();
    TTF_Init();
    char *path = calloc(11, sizeof(char));

    TTF_Quit();
    SDL_Quit();

    return 0;
}
