#include "imgprocessing/display.h"
#include "imgprocessing/drawing.h"
#include "imgprocessing/processing.h"
#include "imgprocessing/recxy.h"
#include "imgprocessing/rxy_bintree.h"
#include "misc/bool_matrix.h"
#include <SDL/SDL.h>
#include <stdlib.h>


bool is_white_col(t_bool_matrix *mat, int x) {
    for (int i = 0; i < mat->lines; i++)
        if (M_bool_GET(mat, x, i))
            return false;
    return true;
}

bool is_white_line(t_bool_matrix *mat, int y) {
    for (int i = 0; i < mat->cols; i++)
        if (M_bool_GET(mat, i, y))
            return false;
    return true;
}

bool is_white(t_bool_matrix *mat, int x, int y, int h, int w) {
    int xend = x + w;
    int yend = y + h;
    for (int i = x; i < xend; i++) {
        for (int j = 0; j < yend; j++) {
            if (M_bool_GET(mat, i, j))
                return false;
        }
    }
    return true;
}

int find_v_cut(t_bool_matrix *mat, int span) {
    bool black = false;
    for (int x = 0; x < mat->cols; x++) {
        if (!is_white_col(mat, x)) {
            black = true;
            continue;
        }
        int count = 1;
        while (count < span && x + count < mat->cols) {
            if (is_white_col(mat, x + count))
                count++;
            else
                break;
        }
        if (!(count < span) && black)
            return x;
    }
    return -1;
}

int find_h_cut(t_bool_matrix *mat, int span) {
    bool black = false;
    for (int y = 0; y < mat->lines; y++) {
        if (!is_white_line(mat, y)) {
            black = true;
            continue;
        }
        int count = 1;
        while (count < span && y + count < mat->lines) {
            if (is_white_line(mat, y + count))
                count++;
            else
                break;
        }
        if (!(count < span) && black)
            return y;
    }
    return -1;
}

// y included
t_bool_matrix *before_h(t_bool_matrix *mat, int y) {
    t_bool_matrix *ret = CREATE_bool_MATRIX(y + 1, mat->cols);
    for (int x = 0; x < mat->cols; x++) {
        for (int j = 0; j <= y; j++) {
            M_bool_SET(ret, x, j, M_bool_GET(mat, x, j));
        }
    }
    return ret;
}

// y included
t_bool_matrix *after_h(t_bool_matrix *mat, int y) {
    t_bool_matrix *ret = CREATE_bool_MATRIX(mat->lines - y, mat->cols);
    for (int x = 0; x < mat->cols; x++) {
        for (int j = y; j < mat->lines; j++) {
            M_bool_SET(ret, x, j - y, M_bool_GET(mat, x, j));
        }
    }
    return ret;
}

// x included
t_bool_matrix *before_v(t_bool_matrix *mat, int x) {
    t_bool_matrix *ret = CREATE_bool_MATRIX(mat->lines, x + 1);
    for (int i = 0; i <= x; i++) {
        for (int y = 0; y < mat->lines; y++) {
            M_bool_SET(ret, i, y, M_bool_GET(mat, i, y));
        }
    }
    return ret;
}

// x included
t_bool_matrix *after_v(t_bool_matrix *mat, int x) {
    t_bool_matrix *ret = CREATE_bool_MATRIX(mat->lines, mat->cols - x);
    for (int i = x; i < mat->cols; i++) {
        for (int y = 0; y < mat->lines; y++) {
            M_bool_SET(ret, i - x, y, M_bool_GET(mat, i, y));
        }
    }
    return ret;
}

void _recxy(struct rxy_bintree *parent, bool h, int cut, bool endnext) {
    printf("recxy | h : %d\n", h);
    printf("Parent : L x C : %d x %d\n", parent->values->lines,
           parent->values->cols);
    if (parent->values->lines < 20 || parent->values->cols < 20) {
        printf("The parent is small enough\n");
        return;
    }
    if (h) {
        printf("Horizontal\n");
        int c = find_h_cut(parent->values, cut);
        if (c == -1) {
            printf("Couldn't find a split\n");
            if (endnext) {
                printf("Ending the recursion\n");
                return;
            }
            endnext = true;
        }
        printf("C : %d\n", c);
        if (is_white(parent->values, 0, 0, c, parent->values->cols) ||
            is_white(parent->values, 0, c + 1, parent->values->lines - c - 1,
                     parent->values->cols)) {
            printf("One of the cuts is white, ignoring it...\n");
            endnext = true;
        } else {
            printf("Found 2 good H splits\n");
            t_bool_matrix *before = before_h(parent->values, c - 1);
            t_bool_matrix *after = after_h(parent->values, c + cut);
            struct rxy_bintree *left_child =
                create_rxy_bintree(before, parent->x, parent->y);
            struct rxy_bintree *right_child =
                create_rxy_bintree(after, parent->x, c + cut);
            parent->left = left_child;
            parent->right = right_child;
            endnext = false;
        }
    } else {
        printf("Vertical\n");
        int c = find_v_cut(parent->values, cut);
        if (c == -1) {
            printf("Couldn't find a split\n");
            if (endnext) {
                printf("Ending the recursion\n");
                return;
            }
            endnext = true;
        }
        printf("C : %d\n", c);
        if (is_white(parent->values, 0, 0, parent->values->lines, c) ||
            is_white(parent->values, c, 0, parent->values->lines,
                     parent->values->cols - c - 1)) {
            printf("One of the cuts is white, ignoring it...\n");
            endnext = true;
        } else {
            printf("Found 2 good V splits\n");
            t_bool_matrix *before = before_v(parent->values, c - 1);
            t_bool_matrix *after = after_v(parent->values, c + cut);
            struct rxy_bintree *left_child =
                create_rxy_bintree(before, parent->x, parent->y);
            struct rxy_bintree *right_child =
                create_rxy_bintree(after, c + cut, parent->y);
            parent->left = left_child;
            parent->right = right_child;
            endnext = false;
        }
    }
    if (!endnext) {
        _recxy(parent->left, !h, cut, endnext);
        _recxy(parent->right, !h, cut, endnext);
    } else {
        _recxy(parent, !h, cut, endnext);
    }
}

void _recxy_only_h(struct rxy_bintree *parent, int cut) {
    printf("Parent : L x C : %d x %d\n", parent->values->lines,
           parent->values->cols);
    if (parent->values->lines < 20 || parent->values->cols < 20) {
        printf("The parent is small enough\n");
        return;
    }
    int c = find_h_cut(parent->values, cut);
    if (c == -1) {
        printf("Couldn't find a split\n");
        printf("Ending the recursion\n");
        return;
    }
    printf("C : %d\n", c);
    if (is_white(parent->values, 0, 0, c, parent->values->cols) ||
        is_white(parent->values, 0, c + 1, parent->values->lines - c - 1,
                 parent->values->cols)) {
        printf("One of the cuts is white, ignoring it...\n");
        return;
    } else {
        printf("Found 2 good H splits\n");
        t_bool_matrix *before = before_h(parent->values, c - 1);
        t_bool_matrix *after = after_h(parent->values, c + cut);
        struct rxy_bintree *left_child =
            create_rxy_bintree(before, parent->x, parent->y);
        struct rxy_bintree *right_child =
            create_rxy_bintree(after, parent->x, parent->y + c + cut);
        parent->left = left_child;
        parent->right = right_child;
    }
    _recxy_only_h(parent->left, cut);
    _recxy_only_h(parent->right, cut);
}

t_bool_matrix *_trim_cols_before(t_bool_matrix *mat){
    if(!mat)
        return NULL;
    int x = 0;
    for(; x < mat->cols; x++){
        if(!is_white_col(mat, x))
            break;
    }
    if(x == 0 || x == mat->cols - 1){
        return NULL;
    }
    t_bool_matrix *ret = CREATE_bool_MATRIX(mat->lines, mat->cols - x);
    for(int y = 0; y < mat->lines; y++){
        for(int i = x; i < mat->cols; i++){
            M_bool_SET(ret, i - x, y, M_bool_GET(mat, i, y));
        }
    }
    return ret;
}

t_bool_matrix *_trim_lines_before(t_bool_matrix *mat){
    if(!mat){
        return NULL;
    }
    int y = 0;
    for(; y < mat->lines; y++){
        if(!is_white_line(mat, y))
            break;
    }
    if(y == 0 || y == mat->lines - 1){
        return NULL;
    }
    t_bool_matrix *ret = CREATE_bool_MATRIX(mat->lines - y , mat->cols);
    for(int x = 0; x < mat->cols; x++){
        for(int j = y; j < mat->lines; j++){
            M_bool_SET(ret, x, j - y, M_bool_GET(mat, x, j));
        }
    }
    return ret;
}

void trim_white_cols(struct rxy_bintree *b){
    t_bool_matrix *new = _trim_cols_before(b->values);
    if(new){
        b->x += (b->values->cols - new->cols);
        b->values = new;
    }
}

void trim_white_lines(struct rxy_bintree *b){
    t_bool_matrix *new = _trim_lines_before(b->values);
    if(new){
        b->y += (b->values->lines - new->lines);
        b->values = new;
    }
}

void _recxy_only_v(struct rxy_bintree *parent, int cut) {
    if (parent->values->cols < 20) {
        /* printf("Small enough\n"); */
        return;
    }
    int c = find_v_cut(parent->values, cut);
    if (c == -1) {
        /* printf("Couldn't find a cut\n"); */
        return;
    }
    /* printf("C : %d\n", c); */
    t_bool_matrix *before = before_v(parent->values, c - 1);
    t_bool_matrix *after = after_v(parent->values, c + cut);
    struct rxy_bintree *left = create_rxy_bintree(before, parent->x, parent->y);
    struct rxy_bintree *right =
        create_rxy_bintree(after, parent->x + c + 1, parent->y);
    parent->left = left;
    parent->right = right;
    _recxy_only_v(parent->left, cut);
    _recxy_only_v(parent->right, cut);
}

struct rxy_bintree *recxy(t_bool_matrix *img, bool onlyh) {
    struct rxy_bintree *ret = create_rxy_bintree(img, 0, 0);
    if (!onlyh) {
        _recxy(ret, true, 1, false);
    } else {
        _recxy_only_h(ret, 1);
    }
    return ret;
}

void draw_boxes_leaves(SDL_Surface *img, struct rxy_bintree *b, int red, int green, int blue) {
    if (!b)
        return;
    if (!b->left && !b->right) {
        draw_rect_outline(img, b->x, b->y, b->values->lines, b->values->cols,
                          SDL_MapRGB(img->format, red, green, blue));
        return;
    }
    draw_boxes_leaves(img, b->left, red, green, blue);
    draw_boxes_leaves(img, b->right, red, green, blue);
}
