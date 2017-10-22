#include "../../headers/imgprocessing/drawing.h"
#include "../../headers/imgprocessing/display.h"
#include "../../headers/imgprocessing/pixop.h"
#include "../../headers/imgprocessing/processing.h"
#include "../../headers/imgprocessing/recxy.h"
#include "../../headers/misc/bool_matrix.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdbool.h>

void aol(t_rxy_bintree *b){
    _recxy_only_v(b, 1);
}

int main(int argc, char *argv[]) {
    (void)argc;
    SDL_Surface *screen = NULL;
    SDL_Surface *img = load_image(argv[1]);
    int t = otsu(img);
    binarize(img, t);


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
