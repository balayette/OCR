#include "../headers/layer.h"
#include "../headers/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void free_layer(t_layer *layer) {
    if (layer->weights) {
        for (int i = 0; i < layer->neuron_count; i++) {
            printf("Freeing weight %d\n", i);
            free(layer->weights[i]);
        }
    }
    free(layer->weights);
    free(layer->hidden_values);
    free(layer->values);
    free(layer);
}

void init_weights(t_layer *layer) {
    double **arr = malloc(sizeof(double *) * layer->neuron_count);
    for (int i = 0; i < layer->neuron_count; i++) {
        arr[i] = malloc(sizeof(double) * layer->neuron_count *
                        layer->prev_layer_size);
        for (int y = 0; y < layer->prev_layer_size; y++) {
            arr[i][y] = rand() / (double)RAND_MAX;
        }
    }
    layer->weights = arr;
}

t_layer *create_layer(const int prev_layer_size, const int neuron_count,
                      const bool random_weights, double **weights,
                      const bool output_layer) {
    t_layer *layer = malloc(sizeof(t_layer));
    layer->prev_layer_size = prev_layer_size;
    layer->neuron_count = neuron_count;
    layer->output_layer = output_layer;
    layer->values = calloc(neuron_count, sizeof(double));
    layer->hidden_values = calloc(neuron_count, sizeof(double));
    if (random_weights)
        init_weights(layer);
    else
        layer->weights = weights;
    return layer;
}

void process_input(t_layer *layer, double *input) {
    double *input_sum = calloc(layer->neuron_count, sizeof(double));
    printf("\nProcessing the input in the layer\n");
    for (int i = 0; i < layer->neuron_count; i++) {
        for (int y = 0; y < layer->prev_layer_size; y++) {
            input_sum[i] += input[y] * layer->weights[i][y];
        }
    }
    printf("Sum of the inputs : ");
    print_double_arr(input_sum,layer->neuron_count);
    double *sigmoid_sum = map(sigmoid, input_sum, layer->neuron_count);
    printf("S(sum) : ");
    print_double_arr(sigmoid_sum, layer->neuron_count);
    free(layer->values);
    free(layer->hidden_values);
    layer->values = sigmoid_sum;
    layer->hidden_values = input_sum;
}
