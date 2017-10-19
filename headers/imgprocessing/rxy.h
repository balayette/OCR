#ifndef RXY_H
#define RXY_H

#include "../misc/bool_matrix.h"

typedef enum e_print_position {
    PREORDER,
    INORDER,
    POSTORDER
} T_PRINT_POSITION;

typedef struct s_bintree t_bintree;
struct s_bintree {
    t_bool_matrix *value;
    t_bintree **children;
    int size;
};

t_bintree *create_bintree(t_bool_matrix *value, t_bintree *left, t_bintree *right);

void depth_first_print(t_bintree *b, T_PRINT_POSITION p);

int count(t_bintree *b);

t_bintree *rxy(t_bool_matrix *img, int size);

void free_bintree(t_bintree *b);

#endif
