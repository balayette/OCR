#include "../headers/neuralnet.h"
#include "../headers/layer.h"
#include "../headers/utils.h"
#include <stdio.h>
#include <stdlib.h>

t_neural_net *create_nn(const int input_count, const int hidden_layer_count,
                        const int neuron_count, const int output_count) {
    t_neural_net *nn = malloc(sizeof(t_neural_net));
    nn->input_count = input_count;
    nn->hidden_layer_count = hidden_layer_count;
    nn->neuron_count = neuron_count;
    nn->output_count = output_count;
    t_layer **layers = malloc(sizeof(t_layer *) * (hidden_layer_count + 2));
    layers[0] = create_layer(0, input_count, false, NULL);
    int prev = input_count;
    for (int i = 0; i < hidden_layer_count; i++) {
        layers[i + 1] = create_layer(prev, neuron_count, true, NULL);
        prev = neuron_count;
    }
    layers[hidden_layer_count + 1] =
        create_layer(prev, output_count, true, NULL);
    nn->layers = layers;
    return nn;
}

void print_nn(const t_neural_net *nn) {
    for (int i = 0; i <= nn->hidden_layer_count; i++) {
        printf("Neuron layer %d\n", i);
        for (int y = 0; y < nn->layers[i + 1]->neuron_count; y++) {
            printf("    Weights of neuron %d. Value : %f", y, nn->layers[i + 1]->values[y]);
            printf("        ");
            print_double_arr(nn->layers[i + 1]->weights[y],
                             nn->layers[i + 1]->prev_layer_size);
        }
    }
}

void forward_prop(t_neural_net *nn, double *input) {
    double *prev_output = input;
    printf("Starting forward propagation\n");
    for(int i = 0; i <= nn->hidden_layer_count; i++){
        process_input(nn->layers[i + 1], prev_output);
        prev_output = nn->layers[i + 1]->values;
    }
}

void back_prop(t_neural_net *nn, double *expected){
    // TODO : Back prop
    (void)nn;
    (void)expected;
}
