#ifndef BOOL_MATRIX_H
#define BOOL_MATRIX_H

#include "matrix.h"
#include <stdbool.h>
#include <stdio.h>

INIT_MATRIX_HEADER(bool)

void disp_bool_matrix(t_bool_matrix *mat);

void save_bool_matrix(char *path, t_bool_matrix *mat);

t_bool_matrix *load_bool_matrix(char *path);

#endif
