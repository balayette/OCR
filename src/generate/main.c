#include "imgprocessing/pixop.h"
#include <time.h>
#include <math.h>
#include <dirent.h>
#include "misc/bool_matrix.h"
#include <stdlib.h>
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

void noisify(t_bool_matrix *mat){
    for(int i = 0; i < mat->cols * mat->lines; i++){
        if((float)rand() / (float)RAND_MAX > 0.98f){
            mat->values[i] = !mat->values[i];
        }
    }
}

SDL_Surface *new_surface(int h, int w) {
    return SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
}

int count_m(t_bool_matrix *m){
    int count = 0;
    for(int y = 0; y < m->lines; y++){
        for(int x = 0; x < m->cols; x++){
            if(M_bool_GET(m, x, y))
                count++;
        }
    }
    return count;
}

int count_s(SDL_Surface *s){
    Uint8 r, g, b;
    int count = 0;
    for(int y = 0; y < s->h; y++){
        for(int x = 0; x < s->w; x++){
            Uint32 p = getpixel(s, x, y);
            SDL_GetRGB(p, s->format, &r, &g, &b);
            if(r == 0 && g == 0 && b == 0)
                count++;
        }
    }
    return count;
}

void do_letter(TTF_Font *font, char *path, int index, int nindex, char l){
    SDL_Surface *surf = NULL;
    SDL_Color black = {0, 0, 0, 255};
    char str[2] = {' ', '\0'};
    str[0] = l;
    surf = TTF_RenderText_Solid(font, str, black);
    t_bool_matrix *mat = surface_to_matrix(surf);

    /* SDL_Surface *screen = NULL; */
    SDL_Surface *s = new_surface(mat->lines, mat->cols);
    matrix_to_surface(s, mat);
    /* printf("Mat count : %d\n", count_m(mat)); */
    /* printf("Surf count : %d\n", count_s(surf)); */
    /* display_and_wait(&screen, surf); */
    /* display_and_wait(&screen, s); */

    /* pprint_bool_matrix(mat); */
    t_bool_matrix *trimed = trim_all(mat);
    /* printf("\n"); */
    /* pprint_bool_matrix(trimed); */

    if(trimed){
        path[index] = l;
        /* printf("         %s\n", path); */
        t_bool_matrix *scaled = NULL;
        if(trimed->lines != 25 || trimed->cols != 25){
            scaled = scale(trimed, 25, 25);
            M_bool_FREE(trimed);
            trimed = scaled;
        }
        path[nindex] = '0';
        save_bool_matrix(path, trimed);
        printf("Before noisify\n");
        pprint_bool_matrix(trimed);
        noisify(trimed);
        printf("Noisified\n");
        path[nindex] = '1';
        pprint_bool_matrix(trimed);
        save_bool_matrix(path, trimed);
        M_bool_FREE(scaled);
    }
    SDL_FreeSurface(surf);
}

void generate(char *font, char *path, int sizeindex, int letterindex){
    int c = 0;
    for(int i = 150; i > 10; i -= 10, c++){
        TTF_Font *f = TTF_OpenFont(font, i);
        path[sizeindex] = c + 'a';
        for(int i = 0; i < TOKENS_LEN; i++){
            do_letter(f, path, letterindex, sizeindex + 1, TOKENS[i]);
        }
        TTF_CloseFont(f);
    }
}

int main(int argc, char *argv[]){
    srand(time(NULL));
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
            strcpy(indiv_output_path + lenoutput + strlen(dir->d_name) - 4, ".size.*");
            int sizeindex = strlen(indiv_output_path) - 6;
            int letterindex = strlen(indiv_output_path) - 1;

            printf("Found font %s\n", indiv_font_path);
            printf("    Outputing to %s\n", indiv_output_path);
            generate(indiv_font_path, indiv_output_path, sizeindex, letterindex);
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
