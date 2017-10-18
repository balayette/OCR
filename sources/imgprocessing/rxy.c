#include "../../headers/imgprocessing/rxy.h"
#include "../../headers/misc/bool_matrix.h"
#include <stdbool.h>

t_bintree *create_bintree(t_bool_matrix *value, t_bintree *left,
                          t_bintree *right) {
    t_bintree *ret = malloc(sizeof(t_bintree));
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
    if(!b->children)
        return 1;
    return 1 + count(b->children[0]) + count(b->children[1]);
}

// Remember : 1 = black, 0 = white

t_bool_matrix **do_hcut(t_bool_matrix *img, int size) {
    printf("Doing hcut\n");
    bool found_black = false;
    int no_black_series = 0;
    for (int x = 0; x < img->cols; x++) {
        int col_black_count = 0;
        for (int y = 0; y < img->lines; y++) {
            int val = M_bool_GET(img, x, y);
            /* printf("Value : %d\n", val); */
            if (val) {
                // Found a black pixel
                if (!found_black) {
                    // First black pixel we find
                    found_black = true;
                    col_black_count++;
                } else {
                    // We have already found a black pixel
                    col_black_count++;
                }
            }
        }
        // At the end, col_black_count = 0 => There's no black pixel on the col
        // We add to no_black_series
        if (col_black_count == 0) {
            printf("Found a full white col\n");
            no_black_series++;
        } else {
            // There are black pixels, we are not going to cut here
            no_black_series = 0;
        }
        if (found_black && no_black_series >= size) {
            // We have found a series of size white cols, we cut here.
            // The first matrix will have cols [0, x - size]
            // The second matrix will have cols [x + 1, img->cols - 1]
            t_bool_matrix *left = CREATE_bool_MATRIX(img->lines, x - size + 1);
            for (int i = 0; i < img->lines; i++) {
                for (int j = 0; j <= (x - size); j++) {
                    M_bool_SET(left, j, i, M_bool_GET(img, j, i));
                }
            }
            t_bool_matrix *right = NULL;
            int rightcols = img->cols - 1 - (x + 1);
            if (rightcols >= 0) {
                right = CREATE_bool_MATRIX(img->lines, rightcols);
                for (int i = 0; i < img->lines; i++) {
                    for (int j = x + 1; j < img->cols - 1; j++) {
                        M_bool_SET(right, j, i, M_bool_GET(img, j, i));
                    }
                }
            }
            printf("Left : [0, %d] | Right : [%d, %d]\n", x - size, x + 1, img->cols - 1);
            t_bool_matrix **ret = malloc(sizeof(t_bool_matrix) * 2);
            ret[0] = left;
            ret[1] = right;
            return ret;
        }
    }
    return NULL;
}

t_bintree **_ryx(t_bool_matrix *img, int size, bool hcut) {
    printf("Size : %d | Hcut : %d\n", size, hcut);
    if (hcut){
        t_bool_matrix **cuts = do_hcut(img, size);
        if(!cuts)
            return NULL;
        printf("%p | %p\n", (void *)cuts[0], (void *)cuts[1]);
        t_bintree **children = malloc(sizeof(t_bintree) * 2);
        children[0] = create_bintree(cuts[0], NULL, NULL);
        children[1] = create_bintree(cuts[1], NULL, NULL);
        children[0]->children = _ryx(children[0]->value, size, false);
        children[1]->children = _ryx(children[1]->value, size, false);
        return children;
    }
    printf("NOT HORIZONTAL\n");
    return NULL;
}

t_bintree *rxy(t_bool_matrix *img, int size) {
    printf("Starting RXY\n");
    t_bintree **children = _ryx(img, size, true);
    t_bintree *ret = create_bintree(img, children[0], children[1]);
    return ret;
}
