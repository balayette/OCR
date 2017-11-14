#include "imgprocessing/drawing.h"
#include "imgprocessing/display.h"
#include "imgprocessing/pixop.h"
#include "imgprocessing/processing.h"
#include "imgprocessing/recxy.h"
#include "imgprocessing/rxy_bintree.h"
#include "misc/bool_matrix.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdbool.h>

void aol(struct rxy_bintree *b){
    _recxy_only_v(b, 1);
}

int main(int argc, char *argv[]) {
    if(argc <= 1)
    {
        printf("Need a path\n");
        exit(1);
    }
    bool do_it = false;
    if(argc > 2){
        do_it = true;
    }
    SDL_Surface *screen = NULL;
    SDL_Surface *img = load_image(argv[1]);
    display_and_wait(&screen, img);
    gray_level(img);
    display_and_wait(&screen, img);
    int t = otsu(img);
    binarize(img, t);
    display_and_wait(&screen, img);
    if(do_it){
        SDL_Surface *h;
        SDL_Surface *v;
        SDL_Surface *c = rlsa(img, 5, &h, &v);
        display_and_wait(&screen, h);
        display_and_wait(&screen, v);
        display_and_wait(&screen, c);
    }

    t_bool_matrix *matrix = surface_to_matrix(img);
    struct rxy_bintree *rxy = recxy(matrix, true);
    /* draw_boxes_leaves(img, rxy, 255, 0, 0); */
    /* display_and_wait(&screen, img); */
    apply_on_leaves(rxy, aol);
    apply_on_leaves(rxy, trim_white_cols);
    apply_on_leaves(rxy, trim_white_lines);
    draw_boxes_leaves(img, rxy, 0, 0, 255);
    display_and_wait(&screen, img);

    /* t_rxy_bintree *woot = recxy(matrix, true); */
    /* depth_first_display_leaves(&screen, img->flags, img->format, woot); */
    /* printf("Size : %d\n", bcount(woot)); */
    /* printf("Size : %d\n", bcount_leafs(woot)); */
    /* draw_boxes_leaves(img, woot, 0, 0, 0); */
    /* display_and_wait(&screen, img); */
    /* apply_on_leaves(woot, aol); */
    /* apply_on_leaves(woot, trim_white_cols); */
    /* draw_boxes_leaves(img, woot, 0, 0, 0); */
    /* display_and_wait(&screen, img); */
    /* depth_first_display_leaves(&screen, img->flags, img->format, woot); */
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
