#include "imgprocessing/rxy.h"
#include "misc/bool_matrix.h"
#include <stdbool.h>
#include <stdlib.h>

t_bintree *create_bintree(t_bool_matrix *value, int x, int y, t_bintree *left,
                          t_bintree *right) {
    t_bintree *ret = malloc(sizeof(t_bintree));
    ret->children = malloc(2 * sizeof(t_bintree *));
    ret->value = value;
    ret->x = x;
    ret->y = y;
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

bool is_full_white(t_bool_matrix *m, int x, int y, int w, int h){
    for(int line = y; y < y + h; line++){
        for(int col = x; x < x + w; col++){
            if(M_bool_GET(m, col, line))
            {
                printf("FOUND BLACK\n");
                return false;
            }
        }
    }
    printf("FULL WHITE\n");
    return true;
}

// Remember : 1 = black, 0 = white

// if a cut has been found : set top and bottom and return 1
// if no cut was found : return 0
int do_hcut(t_bool_matrix *img, int size, t_bool_matrix **top,
            t_bool_matrix **bottom) {
    printf("do_hcut | size : %d\n", size);
    int distance_since_last = 0;
    bool black_on_picture = false;
    for (int y = 0; y < img->lines; y++) {
        bool has_found = false;
        /* printf("Line : %d\n", y); */
        for (int x = 0; x < img->cols; x++) {
            // We can break out of this loop as soon as we find a black pixel
            bool val = M_bool_GET(img, x, y);
            if (val) {
                /* printf("Found a black pixel, breaking out\n"); */
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
                /* if (botlcount <= 0) { */
                /*     // We have found a useless cut. */
                /*     printf("Useless cut\n"); */
                /*     return 0; */
                /* } */
                /* printf("Going to cut\n"); */
                /* printf("Top : L x C : %d x %d\n", toplcount, img->cols); */
                /* printf("Bottom : L x C : %d x %d\n", botlcount, img->cols);
                 */
                if(is_full_white(img, 0, 0, img->cols, toplcount) || is_full_white(img, 0, y + 1, img->cols, botlcount))
                    return 0;
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
                /* printf("Filled both matrices\n"); */
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

int do_vcut(t_bool_matrix *img, int size, t_bool_matrix **left,
            t_bool_matrix **right) {
    printf("do_vcut | size : %d\n", size);
    int distance_since_last = 0;
    bool black_on_picture = false;
    for (int x = 0; x < img->cols; x++) {
        bool has_found = false;
        /* printf("Col : %d\n", x); */
        for (int y = 0; y < img->lines; y++) {
            bool val = M_bool_GET(img, x, y);
            if (val) {
                /* printf("Found a black pixel, breaking\n"); */
                black_on_picture = true;
                has_found = true;
                break;
            }
        }
        if (!has_found && black_on_picture) {
            distance_since_last++;
            if (distance_since_last > size) {
                // We are splitting vertically, which means that both
                // matrices will keep the same number of lines, but will
                // have a different number of cols.
                // left matrix gets cols [0, x - size]
                // right matrix gets cols [x + 1, img->cols]
                int leftccount = x - size;
                int rightccount = img->cols - (x + 1);
                if (rightccount <= 0) {
                    // Useless split
                    printf("Useless v split\n");
                    return 0;
                }
                /* printf("Going to cut\n"); */
                /* printf("Left : L x C : %d x %d\n", img->lines, leftccount);
                 */
                /* printf("Right : L x C : %d x %d\n", img->lines, rightccount);
                 */
                *left = CREATE_bool_MATRIX(img->lines, leftccount);
                *right = CREATE_bool_MATRIX(img->lines, rightccount);
                for (int line = 0; line < img->lines; line++) {
                    // Filling the left matrix
                    for (int leftx = 0; leftx < leftccount; leftx++) {
                        M_bool_SET(*left, leftx, line,
                                   M_bool_GET(img, leftx, line));
                    }

                    // Filling the right matrix
                    for (int rightx = x + 1; rightx < img->cols; rightx++) {
                        M_bool_SET(*right, rightx - (x + 1), line,
                                   M_bool_GET(img, rightx, line));
                    }
                }
                /* printf("Filled both matrices\n"); */
                return 1;
            }
        } else {
            distance_since_last = 0;
        }
    }
    if (black_on_picture)
        return 0;
    if (!black_on_picture)
        return 0;
    return 0;
}

// Returns 2 bintrees if the cut happened
// Returns NULL otherwise
// TODO : Clean this mess of a function
t_bintree **_ryx(t_bool_matrix *img, t_bintree *parent, int hsize, int vsize,
                 bool hcut) {
    printf("_ryx | hsize : %d | vsize : %d | hcut : %d\n", hsize, vsize, hcut);
    t_bintree **ret = malloc(2 * sizeof(t_bintree *));
    if (hcut) {
        t_bool_matrix *top;
        t_bool_matrix *bottom;
        if (do_hcut(img, hsize, &top, &bottom)) {
            // We have found two splits.
            printf("Got two horizontal splits\n");
            t_bintree *left =
                create_bintree(top, parent->x, parent->y, NULL, NULL);
            int righty = parent->y + top->lines + 1 + hsize;
            t_bintree *right =
                create_bintree(bottom, parent->x, righty, NULL, NULL);
            left->children = _ryx(left->value, left, hsize, vsize, !hcut);
            right->children = _ryx(right->value, right, hsize, vsize, !hcut);
            ret[0] = left;
            ret[1] = right;
            return ret;
        } else {
            printf("Didn't get two horizontal splits\n");
            printf("Trying to cut vertically\n");
            // We couldn't cut horizontally.
            // That doesn't mean that we can't cut vertically.
            if (do_vcut(img, vsize, &top, &bottom)) {
                // We could cut vertically.
                // Return the vertical cut.
                printf("Got two vertical splits\n");
                t_bintree *left = create_bintree(top, parent->x, parent->y, NULL, NULL);
                int rightx = parent->x + top->cols + 1 + vsize;
                t_bintree *right = create_bintree(bottom, rightx, parent->y, NULL, NULL);
                left->children = _ryx(left->value, left, hsize, vsize, hcut);
                right->children = _ryx(right->value, right, hsize, vsize, hcut);
                ret[0] = left;
                ret[1] = right;
                return ret;
            }
            printf("Didn't get two vertical splits, ending the recursion\n");
        }
    } else {
        t_bool_matrix *leftm;
        t_bool_matrix *rightm;
        if (do_vcut(img, vsize, &leftm, &rightm)) {
            printf("Got two vertical splits v\n");
            t_bintree *left = create_bintree(leftm, parent->x, parent->y, NULL, NULL);
            int rightx = parent->x + leftm->cols + 1 + vsize;
            t_bintree *right = create_bintree(rightm, rightx, parent->y, NULL, NULL);
            printf("Created v bintrees\n");
            left->children = _ryx(left->value, left, hsize, vsize, !hcut);
            right->children = _ryx(right->value, right, hsize, vsize, !hcut);
            ret[0] = left;
            ret[1] = right;
            return ret;
        } else {
            printf("Didn't get two vertical splits\n");
            printf("Trying to cut horizontally\n");
            if (do_hcut(img, hsize, &leftm, &rightm)) {
                printf("Got two horizontal splits\n");
                t_bintree *left = create_bintree(leftm, parent->x, parent->y, NULL, NULL);
                int righty = parent->y + leftm->lines + 1 + hsize;
                t_bintree *right = create_bintree(rightm, righty, parent->y, NULL, NULL);
                left->children = _ryx(left->value, left, hsize, vsize, hcut);
                right->children = _ryx(right->value, right, hsize, vsize, hcut);
                ret[0] = left;
                ret[1] = right;
                return ret;
            }
            printf("Didn't get two horitontal splits, ending the recursion\n");
        }
    }
    return NULL;
}

t_bintree *rxy(t_bool_matrix *img, int hsize, int vsize) {
    printf("Starting RXY\n");
    t_bintree *ret = create_bintree(img, 0, 0, NULL, NULL);
    ret->children = _ryx(img, ret, hsize, vsize, true);
    return ret;
}
