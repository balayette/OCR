#ifndef LAYER_H
#define LAYER_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct s_layer {
    int neuron_count;
    int prev_layer_size;
    bool output_layer;
    double **weights;
    double *hidden_values;
    double *values;
} t_layer;

t_layer *create_layer(const int prev_layer_size, const int neuron_count,
                      const bool random_weights, double **weights,
                      const bool output_layer);

void process_input(t_layer *layer, double *input);

void free_layer(t_layer *layer);

#endif
