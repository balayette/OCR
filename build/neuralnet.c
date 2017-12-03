#include "neuralnet/neuralnet.h"
#include "misc/bool_matrix.h"
#include "misc/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void mat_to_double(t_bool_matrix *mat, double *output, int width, int height) {
    (void)height;
    (void)width;
    int index = 0;
    int l;
    for (l = 0; l < mat->lines; l++) {
        int c;
        for (c = 0; c < mat->cols; c++, index++) {
            output[index] = M_bool_GET(mat, c, l) ? 1.0 : 0.0;
        }
    }
}

void get_output(double *arr, char letter, int size) {
    memset(arr, 0, sizeof(double) * size);
    arr[letter - 65] = 1.0;
}

char get_letter(double *arr, int size) {
    int index = 0;
    double max = *arr;
    for (int i = 1; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i];
            index = i;
        }
    }
    return (char)(index + 65);
}

int load_dataset(char *path, int index, t_bool_matrix **arr) {
    int i = 0;
    for (char l = 'A'; l <= 'Z'; l++) {
        path[index] = l;
        t_bool_matrix *m = load_bool_matrix(path);
        if (m)
            arr[i] = m;
        else
            arr[i] = NULL;
        i++;
    }
    return i;
}

void print_diff(double *arr1, double *arr2, int size) {
    for (int i = 0; i < size; i++) {
        printf("%f - %f\n", arr1[i], arr2[i]);
    }
}

/* int main(){ */
/*     double input[4][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}}; */
/*     double output[4][1] = {{0}, {1}, {1}, {0}}; */

/*     struct neural_net *nn = create_nn(2, 1, 3, 1); */

/*     for(int i = 0; i < 10; i++){ */
/*         print_nn(nn); */
/*         int k = rand() % 4; */
/*         forward_prop(nn, input[k]); */
/*         back_prop(nn, output[k], input[k]); */
/*         printf("Input : \n"); */
/*         print_double_arr(input[k], 2); */
/*         printf("->  "); */
/*         print_double_arr(nn->layers[nn->hidden_layer_count + 1]->values, 1);
 */
/*         printf("\n"); */
/*     } */

/*     return 0; */
/*  } */

void generate_cache(t_bool_matrix **dataset, double **cache, int size) {
    for (int i = 0; i < size; i++) {
        if (dataset[i])
            mat_to_double(dataset[i], cache[i], 25, 25);
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc < 2) {
        printf("Count missing\n");
        return 1;
    }
    int reps = atoi(argv[1]);
    struct neural_net *nn = create_nn(625, 1, 15, 26);
    char *path = calloc(15, sizeof(char));
    path = strcat(path, "res/data/#.bin");
    double *output = malloc(26 * sizeof(double));
    t_bool_matrix **dataset = calloc(100, sizeof(t_bool_matrix *));
    double **cache = calloc(100, sizeof(double *));
    for (int i = 0; i < 100; i++)
        cache[i] = calloc(625, sizeof(double));

    int datasetlen = load_dataset(path, 9, dataset);

    generate_cache(dataset, cache, 100);

    printf("Dataset length : %d\n", datasetlen);

    for (int rep = 1; rep <= reps; rep++) {
        int i = 0;
        for (char l = 'A'; l <= 'Z'; l++, i = (i + 1) % datasetlen) {
            if (!cache[i])
                continue;
            forward_prop(nn, cache[i]);
            get_output(output, l, 26);
            back_prop(nn, output, cache[i]);
        }
    }
    for (int rep = 1; rep <= 1; rep++) {
        int i = 0;
        for (char l = 'A'; l <= 'Z'; l++, i = (i + 1) % datasetlen) {
            if (!cache[i]) {
                continue;
            }
            forward_prop(nn, cache[i]);
            get_output(output, l, 26);
            char letter =
                get_letter(nn->layers[nn->hidden_layer_count + 1]->values, 26);
            printf("Expected : %c | Got : %c\n", l, letter);
        }
    }

    free(path);
    free(output);
    free_nn(nn);
    for(int i = 0; i < 100; i++)
        if(cache[i])
            free(cache[i]);
    free(cache);
    for(int i = 0; i < 100; i++)
        if(dataset[i])
            M_bool_FREE(dataset[i]);
    free(dataset);
    exit(0);
}
