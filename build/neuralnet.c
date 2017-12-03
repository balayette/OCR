#include "neuralnet/neuralnet.h"
#include "misc/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "misc/bool_matrix.h"
#include <string.h>

void mat_to_double(t_bool_matrix *mat, double *output, int size){
    int index = 0;
    (void)size;
    for(int l = 0; l < mat->lines; l++){
        int c;
        for(c = 0; c < mat->cols; c++){
            /* printf("Setting %d %d | %d\n", l, c, index); */
            output[index] = M_bool_GET(mat, c, l) ? 1.0 : 0.0;
            index++;
        }
        for(; c < 25; c++){
            output[index] = 0.0;
            index++;
        }
    }
}

void get_output(double *arr, char letter, int size){
    memset(arr, size, sizeof(double));
    arr[letter - 97] = 1.0;
}

char get_letter(double *arr, int size){
    int index = 0;
    int max = *arr;
    for(int i = 1; i < size; i++){
        if(arr[i] > max) {
            max = arr[i];
            index = i;
        }
    }
    return (char)(index + 97);
}

int main(int argc, char *argv[])
{
    (void)argc;
    struct neural_net *nn = create_nn(625, 1, 15, 26);
    char *path = calloc(15, sizeof(char));
    strcat(path, "res/data/#.bin");
    double *input = calloc(625, sizeof(double));
    double *output = malloc(26 * sizeof(double));

    for(int i = 0; i < atoi(argv[1]); i++){
        printf("Training %d\n", i);
        for(char l = 'a'; l <= 'z'; l++){
            path[9] = l;

            t_bool_matrix *m = load_bool_matrix(path);
            if(!m)
                continue;

            get_output(output, l, 26);

            mat_to_double(m, input, 625);

            forward_prop(nn, input);
            back_prop(nn, output, input);

            print_nn(nn);

            M_bool_FREE(m);

            memset(input, 625, sizeof(double));
        }
    }
    for(int i = 0; i < 10; i++){
        for(char l = 'a'; l <= 'z'; l++){
            path[9] = l;

            t_bool_matrix *m = load_bool_matrix(path);
            if(!m)
                continue;

            get_output(output, l, 26);

            mat_to_double(m, input, 625);

            forward_prop(nn, input);
            printf("Expected : %c | Got : %c\n", l, get_letter(nn->layers[nn->hidden_layer_count + 1]->values, 26));

            M_bool_FREE(m);

            memset(input, 625, sizeof(double));
        }
    }


    free_nn(nn);
}
