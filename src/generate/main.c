#include "imgprocessing/pixop.h"
#include <dirent.h>
#include "misc/bool_matrix.h"
#include <stdio.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "imgprocessing/display.h"
#include "imgprocessing/processing.h"
#include "matrix/matrixop.h"
#include <string.h>

static const char TOKENS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static const int TOKENS_LEN = 62;

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
    printf("         %s\n", path);
    save_bool_matrix(path, trimc);
    M_bool_FREE(trimc);
    SDL_FreeSurface(surf);
}

void generate(char *font, char *path, int index){
    SDL_Surface *letter = NULL;
    TTF_Font *f = TTF_OpenFont(font, 30);
    for(int i = 0; i < TOKENS_LEN; i++){
        do_letter(letter, f, path, index, TOKENS[i]);
    }
    TTF_CloseFont(f);
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("./generate FONTS_PATH OUTPUT_PATH\n");
        exit(1);
    }
    init_sdl();
    TTF_Init();
    char *output_path = calloc(200, sizeof(char));
    char *fonts_path = calloc(200, sizeof(char));
    strcat(output_path, argv[2]);
    strcat(fonts_path, argv[1]);
    int lenoutput = strlen(output_path);
    int lenfonts = strlen(fonts_path);
    if(output_path[lenoutput - 1] != '/'){
        output_path[lenoutput++] = '/';
    }
    if(fonts_path[lenfonts - 1] != '/'){
        fonts_path[lenfonts++] = '/';
    }
    printf("Fonts path : %s\n", fonts_path);
    printf("Output path : %s\n", output_path);

    DIR *d;
    struct dirent *dir;
    d = opendir(fonts_path);
    if(!d)
    {
        printf("Couldn't open the fonts path\n");
        exit(1);
    }

    char *indiv_font_path = calloc(200, sizeof(char));
    char *indiv_output_path = calloc(200, sizeof(char));
    memcpy(indiv_font_path, fonts_path, lenfonts);
    memcpy(indiv_output_path, output_path, lenoutput);
    while((dir = readdir(d)) != NULL){
        if(dir->d_type == DT_REG){
            strcpy(indiv_font_path + lenfonts, dir->d_name);

            strcpy(indiv_output_path + lenoutput, dir->d_name);
            strcpy(indiv_output_path + lenoutput + strlen(dir->d_name) - 4, ".*");
            int index = strlen(indiv_output_path) - 1;

            printf("Found font %s\n", indiv_font_path);
            printf("    Outputing to %s\n", indiv_output_path);
            generate(indiv_font_path, indiv_output_path, index);
        }
    }

    /* DIR *d; */
    /* struct dirent *dir; */
    /* d = opendir("res/fonts"); */
    /* if(!d) */
    /* { */
    /*     printf("Couldn't open the fonts\n"); */
    /*     exit(1); */
    /* } */
    /* while((dir = readdir(d)) != NULL) */
    /*     if(dir->d_type == DT_REG){ */
    /*         printf("%s\n", dir->d_name); */
    /*         generate(dir->d_name, output_path, 9); */
    /*     } */

    /* closedir(d); */

    TTF_Quit();
    SDL_Quit();
    free(output_path);
    free(fonts_path);

    return 0;
}
