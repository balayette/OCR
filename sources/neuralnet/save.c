#include "../../headers/neuralnet/save.h"
#include "../../headers/misc/utils.h"
#include "../../headers/neuralnet/layer.h"
#include "../../headers/neuralnet/neuralnet.h"
#include <stdio.h>
#include <stdlib.h>

void save_nn(t_neural_net *nn) {
    FILE *file = NULL;

    file = fopen("nn.save", "w+");
    if (file != NULL) {
        binput(sizeof(nn->input_count), &nn->input_count, file);
        binput(sizeof(nn->hidden_layer_count), &nn->hidden_layer_count, file);
        binput(sizeof(nn->output_count), &nn->output_count, file);
        binput(sizeof(nn->layers[nn->hidden_layer_count]->neuron_count),  &nn->layers[nn->hidden_layer_count]->neuron_count, file);
        for (int i = 0; i <= nn->hidden_layer_count; i++) {
            printf("Neuron layer %d\nNb layers : %lu\n", i, sizeof(nn->layers) / sizeof(nn->layers[0]));
            for (int y = 0; y < nn->layers[i + 1]->neuron_count; y++) {
                printf("you lose");
                printf("    Weights of neuron %d.", y);
                printf("        ");
                for (int j = 0; i < nn->layers[i + 1]->prev_layer_size; i++) {
                    binput(sizeof(nn->layers[i + 1]->weights[y][j]),
                           &nn->layers[i + 1]->weights[y][j], file);
                }
                //print_double_arr(nn->layers[i + 1]->weights[y],
                //                 nn->layers[i + 1]->prev_layer_size);
            }
        }
    }

    fclose(file);
}

void binput(size_t const size, void const *const ptr, FILE *file) {
    unsigned char *b = (unsigned char *)ptr;
    unsigned char byte;
    int i, j;

    for (i = size - 1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            fputc(byte, file);
            // printf("%u", byte);
        }
    }
}

void load_nn(FILE *file) {
    int curr = 0;
    char buff[64];
    double buf[4];
    //int neuron_count = 0;

    if (file != NULL) {
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < 64; i++) {
                curr = fgetc(file);
                buff[i] = curr;
            }
            buf[j] = *((double*) buff);
            printf("%fl", buf[j]);
            //neuron_count += buf[j];
        }

        // double load[neuron_count];


        // 1st is input, then hidden layer, then output, then weights
    }
}
