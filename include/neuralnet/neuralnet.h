#ifndef NEURAL_NET_H
#define NEURAL_NET_H

#include "layer.h"



/*

A neural network is made up of (hidden_layer_count + 2) layers :
  hidden_layer_count hidden layers + 1 input layer + 1 output layer.

*/
struct neural_net {
    int input_count;
    int hidden_layer_count;
    int neuron_count;
    int output_count;
    struct layer **layers;
    double bias;
};

struct neural_net *create_nn(const int input_count, const int hidden_layer_count,
                        const int neuron_count, const int output_count);

void print_nn(struct neural_net *nn);

void forward_prop(struct neural_net *nn, double *input);

void back_prop(struct neural_net *nn, double *expected, double *input);

void free_nn(struct neural_net *nn);

#endif
