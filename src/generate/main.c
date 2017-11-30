#include "imgprocessing/pixop.h"
#include "misc/bool_matrix.h"
#include <stdio.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "imgprocessing/display.h"
#include "imgprocessing/processing.h"
#include "matrix/matrixop.h"
#include <string.h>

SDL_Surface *new_surface(int h, int w) {
    return SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
}

void do_letter(SDL_Surface *surf, TTF_Font *font, char *path, int index, char l){
    SDL_Color black = {0, 0, 0, 0};
    SDL_Color white = {255, 255, 255, 0};
    char str[2] = {' ', '\0'};
    str[0] = l;
    surf = TTF_RenderText_Shaded(font, str, black, white);
    t_bool_matrix *mat = surface_to_matrix(surf);
    t_bool_matrix *triml = trim_lines(mat);
    if(!triml)
        return;
    t_bool_matrix *trimc = trim_cols(triml);
    if(!trimc)
        return;
    M_bool_FREE(mat);
    M_bool_FREE(triml);
    path[index] = l;
    printf("%d - %d\n", trimc->lines, trimc->cols);
    save_bool_matrix(path, trimc);
    M_bool_FREE(trimc);
    SDL_FreeSurface(surf);
}

void generate(char *font, char *path, int index){
    SDL_Surface *letter = NULL;
    TTF_Font *f = TTF_OpenFont(font, 30);
    for(char l = 'A'; l <= 'Z'; l++){
        do_letter(letter, f, path, index, l);
        do_letter(letter, f, path, index, l + 26 + 6);
    }
    TTF_CloseFont(f);
}

int main(){
    init_sdl();
    TTF_Init();
    char *path = calloc(15, sizeof(char));
    strcat(path, "res/data/#.bin");
    generate("res/fonts/arial.ttf", path, 9);

    TTF_Quit();
    SDL_Quit();
    free(path);

    return 0;
}
