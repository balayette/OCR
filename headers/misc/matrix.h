#ifndef MATRIX_H
#define MATRIX_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INIT_MATRIX(Type)                                                      \
    t_##Type##_matrix *CREATE_##Type##_MATRIX(int lines, int cols) {           \
        t_##Type##_matrix *ret = malloc(sizeof(t_##Type##_matrix));            \
        ret->values = calloc(cols * lines, sizeof(bool));                      \
        ret->cols = cols;                                                      \
        ret->lines = lines;                                                    \
        return ret;                                                            \
    }                                                                          \
    Type M_##Type##_GET(t_##Type##_matrix *m, int x, int y) {                  \
        return m->values[x + y * m->cols];                                     \
    }                                                                          \
    void M_##Type##_SET(t_##Type##_matrix *m, int x, int y, Type value) {      \
        m->values[x + y * m->cols] = value;                                    \
    }                                                                          \
    void M_##Type##_FREE(t_##Type##_matrix *m) {                               \
        free(m->values);                                                       \
        free(m);                                                               \
    }

#define INIT_MATRIX_HEADER(Type)                                               \
    typedef struct s_##Type##_matrix {                                         \
        Type *values;                                                          \
        int cols;                                                              \
        int lines;                                                             \
    } t_##Type##_matrix;                                                       \
    t_##Type##_matrix *CREATE_##Type##_MATRIX(int lines, int cols);            \
    Type M_##Type##_GET(t_##Type##_matrix *m, int x, int y);                   \
    void M_##Type##_FREE(t_##Type##_matrix *m);                                \
    void M_##Type##_SET(t_##Type##_matrix *m, int x, int y, Type value);

#endif
