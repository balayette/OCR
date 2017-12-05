#include "neuralnet/layer.h"
#include "misc/utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void free_layer(struct layer *layer) {
    if (layer->weights) {
        for (int i = 0; i < layer->neuron_count; i++) {
            free(layer->weights[i]);
        }
    }
    free(layer->weights);
    free(layer->bias_weights);
    free(layer->hidden_values);
    free(layer->values);
    free(layer->deltas);
    free(layer);
}

void init_weights(struct layer *layer) {
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

struct layer *create_layer(const int prev_layer_size, const int neuron_count,
                           const bool random_weights, double **weights,
                           const bool output_layer, double bias) {
    struct layer *layer = malloc(sizeof(struct layer));
    layer->prev_layer_size = prev_layer_size;
    layer->neuron_count = neuron_count;
    layer->output_layer = output_layer;
    layer->values = calloc(neuron_count, sizeof(double));
    layer->hidden_values = calloc(neuron_count, sizeof(double));
    layer->deltas = calloc(neuron_count, sizeof(double));
    layer->bias_weights = calloc(neuron_count, sizeof(double));
    layer->bias = bias;
    layer->weights = NULL;
    for (int i = 0; i < neuron_count; i++)
        layer->bias_weights[i] = 1.0;
    if (random_weights && prev_layer_size != 0)
        init_weights(layer);
    else
        layer->weights = weights;
    return layer;
}

void process_last_input(struct layer *layer, double *input) {
    // This time, use the softmax function
    double *input_sum = calloc(layer->neuron_count, sizeof(double));
    for (int i = 0; i < layer->neuron_count; i++) {
        for (int y = 0; y < layer->prev_layer_size; y++) {
            input_sum[i] += input[y] * layer->weights[i][y] +
                            layer->bias * layer->bias_weights[i];
        }
    }
    double *activations = malloc(sizeof(double) * layer->neuron_count);
    softmax(input_sum, activations, layer->neuron_count);
    free(layer->values);
    free(layer->hidden_values);
    layer->values = activations;
    layer->hidden_values = input_sum;
}

void process_input(struct layer *layer, double *input) {
    double *input_sum = calloc(layer->neuron_count, sizeof(double));
    for (int i = 0; i < layer->neuron_count; i++) {
        for (int y = 0; y < layer->prev_layer_size; y++) {
            input_sum[i] += input[y] * layer->weights[i][y] +
                            layer->bias * layer->bias_weights[i];
        }
    }
    double *sigmoid_sum = map(sigmoid, input_sum, layer->neuron_count);
    free(layer->values);
    free(layer->hidden_values);
    layer->values = sigmoid_sum;
    layer->hidden_values = input_sum;
}
