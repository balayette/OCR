#include "../../headers/imgprocessing/rxy.h"
#include "../../headers/misc/bool_matrix.h"
#include <stdbool.h>
#include <stdlib.h>

t_bintree *create_bintree(t_bool_matrix *value, t_bintree *left,
                          t_bintree *right) {
    t_bintree *ret = malloc(sizeof(t_bintree));
    ret->children = malloc(2 * sizeof(t_bintree *));
    ret->value = value;
    if (left)
        ret->children[0] = left;
    else
        ret->children[0] = NULL;
    if (right)
        ret->children[1] = right;
    else
        ret->children[1] = NULL;
    return ret;
}

void free_bintree(t_bintree *b) {
    if (b->children) {
        if (b->children[0])
            free_bintree(b->children[0]);
        if (b->children[1])
            free_bintree(b->children[1]);
    }
    M_bool_FREE(b->value);
    free(b);
}

void print_bool_matrix(t_bool_matrix *m) {
    printf("---\n");
    printf("Lines x Cols : %d x %d\n", m->lines, m->cols);
    for (int y = 0; y < m->lines; y++) {
        printf("| ");
        for (int x = 0; x < m->cols; x++) {
            printf("%d | ", M_bool_GET(m, x, y));
        }
        printf("\n");
    }
}

void depth_first_print(t_bintree *b, T_PRINT_POSITION p) {
    if (!b | !b->children)
        return;
    if (p == PREORDER)
        print_bool_matrix(b->value);
    depth_first_print(b->children[0], p);
    if (p == INORDER)
        print_bool_matrix(b->value);
    depth_first_print(b->children[1], p);
    if (p == POSTORDER)
        print_bool_matrix(b->value);
}

int count(t_bintree *b) {
    if (!b)
        return 0;
    if (!b->children)
        return 1;
    return 1 + count(b->children[0]) + count(b->children[1]);
}

// Remember : 1 = black, 0 = white

// if a cut has been found : set top and bottom and return 1
// if no cut was found : return 0
int do_hcut(t_bool_matrix *img, int size, t_bool_matrix **top,
            t_bool_matrix **bottom) {
    printf("do_hcut | size : %d | top : %p | bottom : %p\n", size, (void *)top,
           (void *)bottom);
    int distance_since_last = 0;
    bool black_on_picture = false;
    for (int y = 0; y < img->lines; y++) {
        bool has_found = false;
        printf("Line : %d\n", y);
        for (int x = 0; x < img->cols; x++) {
            // We can break out of this loop as soon as we find a black pixel
            bool val = M_bool_GET(img, x, y);
            if (val) {
                printf("Found a black pixel, breaking out\n");
                black_on_picture = true;
                has_found = true;
                break;
            }
            // Otherwise, we just keep looking
        }
        if (!has_found && black_on_picture) {
            // We haven't found a black pixel, we increase the distance
            distance_since_last++;
            if (distance_since_last > size) {
                // We have found our split position
                // We are splitting horitontaly, which means that both
                // matrices will keep the same number of cols, but will
                // not have the same number of lines
                // Top matrix gets lines [0, y - size]
                // Bottom matrix gets lines [y + 1, img->lines]
                int toplcount = y - size;
                int botlcount = img->lines - (y + 1);
                if (botlcount <= 0) {
                    // We have found a useless cut.
                    printf("Useless cut\n");
                    return 0;
                }
                printf("Going to cut\n");
                printf("Top : L x C : %d x %d\n", toplcount, img->cols);
                printf("Bottom : L x C : %d x %d\n", botlcount, img->cols);
                *top = CREATE_bool_MATRIX(toplcount, img->cols);
                *bottom = CREATE_bool_MATRIX(botlcount, img->cols);
                for (int col = 0; col < img->cols; col++) {

                    // Filling the top matrix
                    for (int topy = 0; topy < toplcount; topy++) {
                        M_bool_SET(*top, col, topy, M_bool_GET(img, col, topy));
                    }

                    // Filling the bottom matrix
                    for (int boty = y + 1; boty < img->lines; boty++) {
                        M_bool_SET(*bottom, col, boty - (y + 1),
                                   M_bool_GET(img, col, boty));
                    }
                }
                printf("Filled both matrices\n");
                return 1;
            }
        } else {
            // We have found a black pixel, so we can reset the distance
            distance_since_last = 0;
        }
    }
    // We have gone through the whole matrix without returning, so there are no
    // splits.
    if (black_on_picture)
        return 0;
    if (!black_on_picture) {
        // There are no black pixels in the matrix.
        // Thus this block is useless and should be removed. To be implemented
        // later
        return 0;
    }
    return 0;
}

int do_vcut(t_bool_matrix *img, int size, t_bool_matrix **left, t_bool_matrix **right) {
    (void)img;
    (void)size;
    (void)left;
    (void)right;
    return 0;
}

// Returns 2 bintrees if the cut happened
// Returns NULL otherwise
// TODO : Clean this mess of a function
t_bintree **_ryx(t_bool_matrix *img, int size, bool hcut) {
    printf("_ryx | size : %d | hcut : %d\n", size, hcut);
    t_bool_matrix *top;
    t_bool_matrix *bottom;
    t_bintree **ret = malloc(2 * sizeof(t_bintree *));
    if (hcut) {
        if (do_hcut(img, size, &top, &bottom)) {
            // We have found two splits.
            printf("Got two horizontal splits\n");
            t_bintree *left = create_bintree(top, NULL, NULL);
            t_bintree *right = create_bintree(bottom, NULL, NULL);
            left->children = _ryx(left->value, size, !hcut);
            right->children = _ryx(right->value, size, !hcut);
            ret[0] = left;
            ret[1] = right;
            return ret;
        } else {
            printf("Didn't get two horizontal splits\n");
            printf("Trying to cut vertically\n");
            // We couldn't cut horizontally.
            // That doesn't mean that we can't cut vertically.
            if (do_vcut(img, size, &top, &bottom))
            {
                // We could cut vertically.
                // Return the vertical cut.
                printf("Got two vertical splits\n");
                t_bintree *left = create_bintree(top, NULL, NULL);
                t_bintree *right = create_bintree(bottom, NULL, NULL);
                left->children = _ryx(left->value, size, hcut);
                right->children = _ryx(right->value, size, hcut);
                ret[0] = left;
                ret[1] = right;
                return ret;
            }
        }
    }
    return NULL;
}

t_bintree *rxy(t_bool_matrix *img, int size) {
    printf("Starting RXY\n");
    t_bintree *ret = create_bintree(img, NULL, NULL);
    ret->children = _ryx(img, size, true);
    return ret;
}
