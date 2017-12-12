#define _GNU_SOURCE
#include "neuralnet/neuralnet.h"
#include "misc/bool_matrix.h"
#include "misc/utils.h"
#include "neuralnet/save.h"
#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define H 35
#define V 35
#define SIZE H*V


static const char TOKENS[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVW"
    "XYZ0123456789";
static const int TOKENS_LEN = 62;
static const int SYMBOL_OFFSET = 0;
static const int LETTER_OFFSET = 0;
static const int CAPS_OFFSET = 26 + 0;
static const int NUMBER_OFFSET = 26 + 26 + 0;

void mat_to_double(t_bool_matrix *mat, double *output) {
    for (int i = 0; i < mat->cols * mat->lines; i++)
        output[i] = mat->values[i] ? 0.0 : 1.0;
}

int get_letter_index(char letter) {
    if(letter <= '.')
        return letter - 44 + SYMBOL_OFFSET;
    else if (letter <= '9')
        return letter - 48 + NUMBER_OFFSET;
    else if (letter <= 'Z')
        return letter - 65 + CAPS_OFFSET;
    else if (letter <= 'z')
        return letter - 97 + LETTER_OFFSET;
    return -1;
}

// The array has to be 0'ed
void get_output(double *arr, char letter) {
    arr[get_letter_index(letter)] = 1.0;
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

// The dataset is a an array of TOKENS_LEN arrays of *t_bool_matrix, each
// containing a variable number of matrices

struct dataset {
    int *sizes;
    t_bool_matrix ***data;
};

struct cache {
    int *sizes;
    double ***data;
    double **output;
};

struct cache *cache_from_dataset(struct dataset *ds) {
    struct cache *c = malloc(sizeof(struct cache));

    c->output = calloc(TOKENS_LEN, sizeof(double *));

    c->sizes = malloc(TOKENS_LEN * sizeof(int));
    memcpy(c->sizes, ds->sizes, sizeof(int) * TOKENS_LEN);

    c->data = calloc(TOKENS_LEN, sizeof(double **));
    for (int i = 0; i < TOKENS_LEN; i++) {
        c->data[i] = calloc(ds->sizes[i], sizeof(double *));
        for (int y = 0; y < ds->sizes[i]; y++) {
            c->data[i][y] = calloc(SIZE, sizeof(double));
            /* pprint_bool_matrix(ds->data[i][y]); */
            mat_to_double(ds->data[i][y], c->data[i][y]);
        }
        c->output[i] = calloc(TOKENS_LEN, sizeof(double));
        get_output(c->output[i], TOKENS[i]);
    }
    return c;
}

void free_cache(struct cache *c) {

    for (int i = 0; i < TOKENS_LEN; i++) {
        for (int y = 0; y < c->sizes[i]; y++) {
            free(c->data[i][y]);
        }
        free(c->data[i]);
        free(c->output[i]);
    }
    free(c->sizes);
    free(c->output);
    free(c->data);
    free(c);
}

struct dataset *load_dataset(char *path) {
    int plen = strlen(path);
    if (path[plen - 1] != '/')
        path[plen++] = '/';

    printf("Loading dataset from %s\n", path);
    struct dataset *ds = malloc(sizeof(struct dataset));
    ds->sizes = calloc(TOKENS_LEN, sizeof(int));
    ds->data = calloc(TOKENS_LEN, sizeof(t_bool_matrix **));
    // FIXME : This has to change if we use more than a 1000 fonts
    for (int i = 0; i < TOKENS_LEN; i++) {
        ds->data[i] = calloc(1000, sizeof(t_bool_matrix *));
    }

    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    if (!d) {
        printf("Couldn't open %s\n", path);
        exit(1);
    }
    char *indiv_path = calloc(1000, sizeof(char));
    strcpy(indiv_path, path);
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG) {
            strcpy(indiv_path + plen, dir->d_name);

            printf("Found matrix %s\n", indiv_path);
            int index = get_letter_index(indiv_path[strlen(indiv_path) - 1]);
            /* printf("    Letter : %c | Index : %d | Current count : %d\n", */
            /*        indiv_path[strlen(indiv_path) - 1], index,
             * ds->sizes[index]); */
            t_bool_matrix *m = load_bool_matrix(indiv_path);
            if (!m) {
                printf("Loading failed\n");
                continue;
            }
            ds->data[index][ds->sizes[index]++] = m;
        }
    }
    closedir(d);
    free(indiv_path);
    printf("\nLoaded the dataset. Final state : \n");
    printf("    |  Letter  |  Count  |\n");
    int sum = 0;
    for (int i = 0; i < TOKENS_LEN; i++) {
        printf("    |    %c     |    %d   |\n", TOKENS[i], ds->sizes[i]);
        sum += ds->sizes[i];
    }
    printf("    |  Total   |   %d  |\n", sum);
    return ds;
}

void free_dataset(struct dataset *ds) {

    for (int i = 0; i < TOKENS_LEN; i++) {
        for (int y = 0; y < ds->sizes[i]; y++) {
            M_bool_FREE(ds->data[i][y]);
        }
        free(ds->data[i]);
    }
    free(ds->data);
    free(ds->sizes);
    free(ds);
}

void print_diff(double *arr1, double *arr2, int size) {
    for (int i = 0; i < size; i++) {
        printf("%f - %f\n", arr1[i], arr2[i]);
    }
}

void generate_cache(t_bool_matrix **dataset, double **cache) {
    for (int i = 0; i < TOKENS_LEN; i++) {
        if (dataset[i]) {
            mat_to_double(dataset[i], cache[i]);
        }
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc < 3) {
        printf("Usage : ./neuralnet DATA_DIR REPS\n");
        exit(1);
    }

    struct neural_net *nn;
    printf("%d\n", argc);
    if (argc == 3) {
        nn = create_nn(SIZE, 1, 55, TOKENS_LEN);
    } else {
        printf("Loading\n");
        nn = load_nn(argv[3]);
    }
    char *path = calloc(1000, sizeof(char));
    strcat(path, argv[1]);
    struct dataset *ds = load_dataset(path);
    struct cache *c = cache_from_dataset(ds);

    int reps = atoi(argv[2]);
    /* struct neural_net *nn = load_nn("save.nn"); */

    int count = 0;
    for (int BIGREP = 1; BIGREP <= 1000; BIGREP++) {
        printf("%d / %d\n", BIGREP * (reps / 1000), reps);
        for (int rep = 1; rep <= reps / 1000; rep++) {
            int k = rand() % TOKENS_LEN;
            int y = rand() % c->sizes[k];
            /* for (int y = 0; y < c->sizes[k]; y++) { */
            forward_prop(nn, c->data[k][y]);
            /* printf("Output : "); */
            /* print_double_arr(nn->layers[nn->hidden_layer_count + 1]->values,
             * TOKENS_LEN); */
            back_prop(nn, c->output[k], c->data[k][y]);
            count++;
            /* } */
        }
    }
    printf("Propagations : %d\n", count);
    save_nn(nn, "save.nn");
    free_nn(nn);

    nn = load_nn("save.nn");
    for (int i = 0; i < TOKENS_LEN; i++) {
        if (!c->sizes[i])
            continue;
        printf("%c ->\n    ", TOKENS[i]);
        for (int k = 0; k < c->sizes[i]; k++) {
            forward_prop(nn, c->data[i][k]);
            printf("%c, ",
                   get_letter(nn->layers[nn->hidden_layer_count + 1]->values,
                              TOKENS_LEN));
        }
        printf("\n");
    }

    free_dataset(ds);
    free_cache(c);
    free_nn(nn);
    free(path);
    return 0;
}
