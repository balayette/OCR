#include "../../headers/imgprocessing/drawing.h"
#include "../../headers/imgprocessing/display.h"
#include "../../headers/imgprocessing/pixop.h"
#include "../../headers/imgprocessing/processing.h"
#include "../../headers/imgprocessing/recxy.h"
#include "../../headers/misc/bool_matrix.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdbool.h>

/* // Print nodes blue */
/* // Print leafs red */
/* void draw_bin_tree(SDL_Surface *img, t_bintree *b){ */
/*     if(!b) */
/*         return; */

/*     if(!b->children || (!b->children[0] && !b->children[1])){ */
/*         // leaf */
/*         draw_rect_outline(img, b->x, b->y, b->value->lines, b->value->cols, SDL_MapRGB(img->format, 255, 0, 0)); */
/*         return; */
/*     } */
/*     if(b->children[0] || b->children[1]){ */
/*         draw_rect_outline(img, b->x, b->y, b->value->lines, b->value->cols, SDL_MapRGB(img->format, 0, 0, 255)); */
/*         draw_bin_tree(img, b->children[0]); */
/*         draw_bin_tree(img, b->children[1]); */
/*     } */
/* } */

/* void disp_bin_tree_leafs(SDL_Surface **screen, SDL_Surface *img, t_bintree *b) { */
/*     if (!b) */
/*         return; */
/*     if(!b->children){ */
/*         t_bool_matrix *value = b->value; */
/*         SDL_Surface *s = SDL_CreateRGBSurface( */
/*             img->flags, value->cols, value->lines, img->format->BitsPerPixel, */
/*             img->format->Rmask, img->format->Gmask, img->format->Bmask, */
/*             img->format->Ashift); */
/*         matrix_to_surface(s, value); */
/*         display_and_wait(screen, s); */
/*     } */
/*     else if(b->children[0] || b->children[1]){ */
/*         if(b->children[0]) */
/*             disp_bin_tree_leafs(screen, img, b->children[0]); */
/*         if(b->children[1]) */
/*             disp_bin_tree_leafs(screen, img, b->children[1]); */
/*     } */
/*     else{ */
/*         t_bool_matrix *value = b->value; */
/*         SDL_Surface *s = SDL_CreateRGBSurface( */
/*             img->flags, value->cols, value->lines, img->format->BitsPerPixel, */
/*             img->format->Rmask, img->format->Gmask, img->format->Bmask, */
/*             img->format->Ashift); */
/*         matrix_to_surface(s, value); */
/*         display_and_wait(screen, s); */
/*     } */
/* } */

/* void disp_bin_tree(SDL_Surface **screen, SDL_Surface *img, t_bintree *b) { */
/*     if (!b) */
/*         return; */
/*     t_bool_matrix *value = b->value; */
/*     SDL_Surface *s = SDL_CreateRGBSurface( */
/*         img->flags, value->cols, value->lines, img->format->BitsPerPixel, */
/*         img->format->Rmask, img->format->Gmask, img->format->Bmask, */
/*         img->format->Ashift); */
/*     matrix_to_surface(s, value); */
/*     display_and_wait(screen, s); */
/*     SDL_FreeSurface(s); */
/*     if(!b->children) */
/*         return; */
/*     disp_bin_tree(screen, img, b->children[0]); */
/*     disp_bin_tree(screen, img, b->children[1]); */
/* } */

int main(int argc, char *argv[]) {
    (void)argc;
    SDL_Surface *screen = NULL;
    SDL_Surface *img = load_image(argv[1]);
    int t = otsu(img);
    binarize(img, t);

    t_bool_matrix *matrix = surface_to_matrix(img);

    t_rxy_bintree *woot = recxy(matrix);
    depth_first_display_leaves(&screen, img->flags, img->format, woot);
    printf("Size : %d\n", bcount(woot));
    printf("Size : %d\n", bcount_leafs(woot));
    /* t_bintree *woot = rxy(matrix, 14, 500); */

    /* /\* depth_first_print(woot, INORDER); *\/ */
    /* printf("Size : %d\n", count(woot->children[1])); */

    /* disp_bin_tree(&screen, img, woot->children[1]); */
    /* free_bintree(woot); */
    /* display_and_wait(&screen, img); */

    SDL_FreeSurface(img);
    SDL_FreeSurface(screen);
    SDL_Quit();
    return 0;
}
