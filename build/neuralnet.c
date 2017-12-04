#include "neuralnet/neuralnet.h"
#include "misc/bool_matrix.h"
#include "misc/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stddef.h>

static const char TOKENS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
static const int TOKENS_LEN = 62;
static const int LETTER_OFFSET = 0;
static const int NUMBER_OFFSET = 52;
static const int CAPS_OFFSET = 26;

void mat_to_double(t_bool_matrix *mat, double *output, int width, int height) {
    int h_padding = width - mat->cols;
    int left_padding = h_padding / 2;
    int right_padding = h_padding - left_padding;

    int v_padding = height - mat->lines;
    int top_padding = v_padding / 2;
    int bottom_padding = v_padding - top_padding;

    assert(mat->lines + top_padding + bottom_padding == height);
    assert(mat->cols + left_padding + right_padding == width);

    int index = 0;
    for(int top = 0; top < top_padding; top++){
        output[index] = 0.0;
        index++;
    }
    for(int line = 0; line < mat->lines; line++){
        for(int left = 0; left < left_padding; left++){
            output[index] = 0.0;
            index++;
        }
        for(int col = 0; col < mat->cols; col++){
            output[index] = M_bool_GET(mat, col, line) ? 1.0 : 0.0;
            index++;
        }
        for(int right = 0; right < right_padding; right++){
            output[index] = 0.0;
            index++;
        }
    }
    for(int bot = 0; bot < bottom_padding; bot++){
        output[index] = 0.0;
        index++;
    }
}

// Find a way to remove the memset (set to 0 in the main loop)
void get_output(double *arr, char letter, int size) {
    memset(arr, 0, sizeof(double) * size);
    if(letter <= '9')
        arr[letter - 48 + NUMBER_OFFSET] = 1.0;
    else if(letter <= 'z')
        arr[letter - 97 + LETTER_OFFSET] = 1.0;
    else if(letter <= 'Z')
        arr[letter - 65 + CAPS_OFFSET] = 1.0;
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
    return TOKENS[index];
}

void load_dataset(char *path, int index, t_bool_matrix **arr){
    for(int i = 0; i < TOKENS_LEN; i++){
        path[index] = TOKENS[i];
        t_bool_matrix *m = load_bool_matrix(path);
        if(!m)
            continue;
        arr[i] = m;
    }
}

void free_dataset(t_bool_matrix **arr){
    for(int i = 0; i < TOKENS_LEN; i++)
        if(arr[i])
            M_bool_FREE(arr[i]);
    free(arr);
}

/* int load_dataset(char *path, int index, t_bool_matrix **arr) { */
/*     int i = 0; */
/*     for (char l = 'A'; l <= 'Z'; l++) { */
/*         path[index] = l; */
/*         t_bool_matrix *m = load_bool_matrix(path); */
/*         if (m) */
/*             arr[i] = m; */
/*         else */
/*             arr[i] = NULL; */
/*         i++; */
/*     } */
/*     return i; */
/* } */

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

void generate_cache(t_bool_matrix **dataset, double **cache) {
    for (int i = 0; i < TOKENS_LEN; i++) {
        if (dataset[i])
            mat_to_double(dataset[i], cache[i], 25, 25);
    }
}

void free_cache(double **cache){
    for(int i = 0; i < TOKENS_LEN; i++)
        if(cache[i])
            free(cache[i]);
    free(cache);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc < 2) {
        printf("Count missing\n");
        return 1;
    }
    int reps = atoi(argv[1]);
    struct neural_net *nn = create_nn(625, 1, 15, TOKENS_LEN);
    char *path = calloc(15, sizeof(char));
    path = strcat(path, "res/data/#.bin");
    double *output = malloc(TOKENS_LEN * sizeof(double));
    t_bool_matrix **dataset = calloc(TOKENS_LEN, sizeof(t_bool_matrix *));
    double **cache = calloc(TOKENS_LEN, sizeof(double *));
    for (int i = 0; i < TOKENS_LEN; i++)
        cache[i] = calloc(625, sizeof(double));

    load_dataset(path, 9, dataset);

    generate_cache(dataset, cache);

    /* printf("Dataset length : %d\n", TOKENS_LEN); */

    for(int BIGREP = 1; BIGREP <= 1000; BIGREP++){
        printf("%d / %d\n", BIGREP * (reps / 1000), reps);
        for(int rep = 1; rep <= reps / 1000; rep++){
            for(int i = 0; i < TOKENS_LEN; i++){
                int k = rand() % TOKENS_LEN;
                if(!cache[k])
                    continue;
                forward_prop(nn, cache[k]);
                get_output(output, TOKENS[k], TOKENS_LEN);
                back_prop(nn, output, cache[k]);
            }
        }
    }

    for(int i = 0; i < TOKENS_LEN; i++){
        if(!cache[i])
            continue;
        forward_prop(nn, cache[i]);
        printf("Expected %c | Got %c\n", TOKENS[i], get_letter(nn->layers[nn->hidden_layer_count + 1]->values, TOKENS_LEN));
    }
    /* for (int rep = 1; rep <= reps; rep++) { */
    /*     int i = 0; */
    /*     for (char l = 'A'; l <= 'Z'; l++, i = (i + 1) % TOKENS_LEN) { */
    /*         if (!cache[i]) */
    /*             continue; */
    /*         forward_prop(nn, cache[i]); */
    /*         get_output(output, l, 26); */
    /*         back_prop(nn, output, cache[i]); */
    /*     } */
    /* } */
    /* for (int rep = 1; rep <= 1; rep++) { */
    /*     int i = 0; */
    /*     for (char l = 'A'; l <= 'Z'; l++, i = (i + 1) % TOKENS_LEN) { */
    /*         if (!cache[i]) { */
    /*             continue; */
    /*         } */
    /*         forward_prop(nn, cache[i]); */
    /*         get_output(output, l, 26); */
    /*         char letter = */
    /*             get_letter(nn->layers[nn->hidden_layer_count + 1]->values, 26); */
    /*         printf("Expected : %c | Got : %c\n", l, letter); */
    /*     } */
    /* } */

    return 0;
}
