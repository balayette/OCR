#include "misc/bool_matrix.h"
#include "misc/matrix.h"
#include <stdbool.h>

INIT_MATRIX(bool)

void disp_bool_matrix(t_bool_matrix *mat){
    for(int y = 0; y < mat->lines; y++){
        for(int x = 0; x < mat->cols; x++){
            printf("| %d ", M_bool_GET(mat, x, y));
        }
        printf("|\n");
    }
}

void save_bool_matrix(char *path, t_bool_matrix *mat) {
    FILE *f = fopen(path, "wb+");
    fwrite(mat->values, sizeof(bool), mat->lines * mat->cols, f);
    fclose(f);
}
