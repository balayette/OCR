#include "neuralnet/save.h"
#include "misc/utils.h"
#include "neuralnet/layer.h"
#include "neuralnet/neuralnet.h"
#include <stdio.h>
#include <stdlib.h>

/*
  Format of the save file :

  input_count | output_count | hidden_layer_count | neuron_count
      int            int            int                 int

  for i in range(hidden_layer_count):

    k = hidden_layer[i]->neuron_count
    k * bias_weight   |     k * hidden_layer[i]->prev_layer_size weight    |
       k * double          k * hidden_layer[i]->prev_layer_size * double

  endfor

  k = output_count
  k * bias_weight     |     k * output_layer->prev_layer_size weight      |
     k * double            k * output_layer->prev_layer_size * double

 */

/* void save_nn(struct neural_net *nn, char *path) { */
/*     FILE *file = fopen(path, "wb"); */
/*     if (file == NULL) { */
/*         printf("Error while opening the file."); */
/*         return; */
/*     } */

/*     fwrite(&nn->input_count, sizeof(int), 1, file); */
/*     fwrite(&nn->output_count, sizeof(int), 1, file); */
/*     fwrite(&nn->hidden_layer_count, sizeof(int), 1, file); */
/*     fwrite(&nn->neuron_count, sizeof(int), 1, file); */

/*     for (int i = 0; i <= nn->hidden_layer_count; i++) { */

/*         for (int y = 0; y < nn->layers[i + 1]->neuron_count; y++) { */
/*             fwrite(&nn->layers[i + 1]->bias_weights[y], sizeof(double), */
/*                    nn->layers[i + 1]->neuron_count, file); */

/*             for (int j = 0; j < nn->layers[i + 1]->prev_layer_size; j++) { */
/*                 fwrite(&nn->layers[i + 1]->weights[y][j], sizeof(double), */
/*                        nn->layers[i + 1]->neuron_count, file); */
/*             } */
/*         } */
/*     } */
/*     fclose(file); */
/* } */

void save_nn(struct neural_net *nn, char *path) {
    FILE *file = fopen(path, "wb");
    if (!file) {
        printf("Couldn't open the file\n");
        exit(1);
    }

    fwrite(&nn->input_count, sizeof(int), 1, file);
    fwrite(&nn->output_count, sizeof(int), 1, file);
    fwrite(&nn->hidden_layer_count, sizeof(int), 1, file);
    fwrite(&nn->neuron_count, sizeof(int), 1, file);

    for (int i = 0; i < nn->hidden_layer_count; i++) {
        fwrite(nn->layers[i + 1]->bias_weights, sizeof(double),
               nn->neuron_count, file);

        // Change this when we move the weights to use a matrix
        // Will simplify the code a lot, we'll have to use the same piece of
        // code as for bias_weights

        for (int k = 0; k < nn->neuron_count; k++) {
            fwrite(nn->layers[i + 1]->weights[k], sizeof(double),
                   nn->layers[i + 1]->prev_layer_size, file);
        }
    }

    fwrite(nn->layers[nn->hidden_layer_count + 1]->bias_weights, sizeof(double),
           nn->output_count, file);
}

struct neural_net *load_nn(char *path) {

    FILE *file = fopen(path, "rb");

    int r;

    int inputs;
    r = fread(&inputs, sizeof(int), 1, file);
    if (r == 0)
        return NULL;
    int outputs;
    r = fread(&outputs, sizeof(int), 1, file);
    if (r == 0)
        return NULL;
    int hidden;
    r = fread(&hidden, sizeof(int), 1, file);
    if (r == 0)
        return NULL;
    int count;
    r = fread(&count, sizeof(int), 1, file);
    if (r == 0)
        return NULL;
    struct neural_net *nn = create_nn(inputs, hidden, count, outputs);

    for (int i = 0; i <= nn->hidden_layer_count; i++) {
        for (int y = 0; y < nn->layers[i + 1]->neuron_count; y++) {
            r = fread(&nn->layers[i + 1]->bias_weights[y], sizeof(double),
                      nn->layers[i + 1]->neuron_count, file);
            if (r == 0)
                return NULL;
            for (int j = 0; j < nn->layers[i + 1]->prev_layer_size; j++) {
                r = fread(&nn->layers[i + 1]->weights[y][j], sizeof(double),
                          nn->layers[i + 1]->neuron_count, file);
                if (r == 0)
                    return NULL;
            }
        }
    }

    fclose(file);

    return nn;
}
