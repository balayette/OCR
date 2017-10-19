#include "../../headers/imgprocessing/display.h"
#include "../../headers/imgprocessing/pixop.h"
#include "../../headers/imgprocessing/processing.h"
#include "../../headers/imgprocessing/rxy.h"
#include "../../headers/misc/bool_matrix.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdbool.h>


void disp_bin_tree_leafs(SDL_Surface **screen, SDL_Surface *img, t_bintree *b) {
    if (!b)
        return;
    if(!b->children){
        t_bool_matrix *value = b->value;
        SDL_Surface *s = SDL_CreateRGBSurface(
            img->flags, value->cols, value->lines, img->format->BitsPerPixel,
            img->format->Rmask, img->format->Gmask, img->format->Bmask,
            img->format->Ashift);
        matrix_to_surface(s, value);
        display_and_wait(screen, s);
    }
    else if(b->children[0] || b->children[1]){
        if(b->children[0])
            disp_bin_tree_leafs(screen, img, b->children[0]);
        if(b->children[1])
            disp_bin_tree_leafs(screen, img, b->children[1]);
    }
}

void disp_bin_tree(SDL_Surface **screen, SDL_Surface *img, t_bintree *b) {
    if (!b)
        return;
    t_bool_matrix *value = b->value;
    SDL_Surface *s = SDL_CreateRGBSurface(
        img->flags, value->cols, value->lines, img->format->BitsPerPixel,
        img->format->Rmask, img->format->Gmask, img->format->Bmask,
        img->format->Ashift);
    matrix_to_surface(s, value);
    display_and_wait(screen, s);
    SDL_FreeSurface(s);
    if(!b->children)
        return;
    disp_bin_tree(screen, img, b->children[0]);
    disp_bin_tree(screen, img, b->children[1]);
}

int main(int argc, char *argv[]) {
    (void)argc;
    SDL_Surface *screen = NULL;
    SDL_Surface *img = load_image(argv[1]);
    display_and_wait(&screen, img);
    int t = otsu(img);
    binarize(img, t);

    t_bool_matrix *matrix = surface_to_matrix(img);

    t_bintree *woot = rxy(matrix, 20);
    /* depth_first_print(woot, INORDER); */
    printf("Size : %d\n", count(woot));
    disp_bin_tree_leafs(&screen, img, woot);

    free_bintree(woot);

    SDL_FreeSurface(img);
    SDL_FreeSurface(screen);
    SDL_Quit();
    return 0;
}
