#include "imgprocessing/rxy_bintree.h"
#include "misc/bool_matrix.h"
#include "imgprocessing/processing.h"
#include "imgprocessing/display.h"

void apply_on_leaves2(struct rxy_bintree *b, t_bool_matrix *(*f)(t_bool_matrix *)){
    if(!b){
        return;
    }
    if(!b->left && !b->right){
        b->values = f(b->values);
    }
    else{
        apply_on_leaves2(b->left, f);
        apply_on_leaves2(b->right, f);
    }
}
void apply_on_leaves(struct rxy_bintree *b, void (*f)(struct rxy_bintree *)) {
    if (!b) {
        return;
    }
    if (!b->left && !b->right) {
        f(b);
    }
    else{
        apply_on_leaves(b->left, f);
        apply_on_leaves(b->right, f);
    }
}

struct rxy_bintree *create_rxy_bintree(t_bool_matrix *value, int x, int y) {
    struct rxy_bintree *ret = malloc(sizeof(struct rxy_bintree));
    ret->x = x;
    ret->y = y;
    ret->values = value;
    ret->left = NULL;
    ret->right = NULL;
    return ret;
}

int bcount_leaves(struct rxy_bintree *b) {
    if (!b)
        return 0;
    if (b->left || b->right)
        return bcount_leaves(b->left) + bcount_leaves(b->right);
    return 1;
}

void df_display(SDL_Surface **screen, Uint32 flags,
                         SDL_PixelFormat *fmt, struct rxy_bintree *b) {
    if (!b)
        return;
    SDL_Surface *s = SDL_CreateRGBSurface(
        flags, b->values->cols, b->values->lines, fmt->BitsPerPixel, fmt->Rmask,
        fmt->Gmask, fmt->Bmask, fmt->Amask);
    matrix_to_surface(s, b->values);
    display_and_wait(screen, s);
    df_display(screen, flags, fmt, b->left);
    df_display(screen, flags, fmt, b->right);
}

void df_display_leaves(SDL_Surface **screen, Uint32 flags,
                                SDL_PixelFormat *fmt, struct rxy_bintree *b) {
    if (!b)
        return;
    if (b->left || b->right) {
        df_display_leaves(screen, flags, fmt, b->left);
        df_display_leaves(screen, flags, fmt, b->right);
        return;
    }
    printf("Printing\n");
    disp_bool_matrix(b->values);
    SDL_Surface *s = SDL_CreateRGBSurface(
        flags, b->values->cols, b->values->lines, fmt->BitsPerPixel, fmt->Rmask,
        fmt->Gmask, fmt->Bmask, fmt->Amask);
    matrix_to_surface(s, b->values);
    display_and_wait(screen, s);
}

int bcount(struct rxy_bintree *b) {
    if (!b)
        return 0;
    return 1 + bcount(b->left) + bcount(b->right);
}
