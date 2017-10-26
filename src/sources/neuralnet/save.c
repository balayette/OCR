#include "../../headers/neuralnet/save.h"
#include "../../headers/misc/utils.h"
#include "../../headers/neuralnet/layer.h"
#include "../../headers/neuralnet/neuralnet.h"
#include <stdio.h>
#include <stdlib.h>

void save_nn(t_neural_net *nn, char *path) {
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        printf("Error while opening the file.");
        return;
    }

    fwrite(&nn->input_count, sizeof(int), 1, file);
    fwrite(&nn->output_count, sizeof(int), 1, file);
    fwrite(&nn->hidden_layer_count, sizeof(int), 1, file);
    fwrite(&nn->neuron_count, sizeof(int), 1, file);

    for (int i = 0; i <= nn->hidden_layer_count; i++) {

        for (int y = 0; y < nn->layers[i + 1]->neuron_count; y++) {
            fwrite(&nn->layers[i + 1]->bias_weights[y], sizeof(double),
                   nn->layers[i + 1]->neuron_count, file);

            for (int j = 0; j < nn->layers[i + 1]->prev_layer_size; j++) {
                fwrite(&nn->layers[i + 1]->weights[y][j], sizeof(double),
                       nn->layers[i + 1]->neuron_count, file);
            }
        }
    }
    fclose(file);
}

t_neural_net *load_nn(char *path) {

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
    t_neural_net *nn = create_nn(inputs, hidden, count, outputs);

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
