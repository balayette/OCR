#ifndef NEURAL_NET_H
#define NEURAL_NET_H

#include "layer.h"



/*

A neural network is made up of (hidden_layer_count + 2) layers :
  hidden_layer_count hidden layers + 1 input layer + 1 output layer.

*/
typedef struct s_neural_net {
    int input_count;
    int hidden_layer_count;
    int neuron_count;
    int output_count;
    t_layer **layers;
    double bias;
} t_neural_net;

t_neural_net *create_nn(const int input_count, const int hidden_layer_count,
                        const int neuron_count, const int output_count);

void print_nn(const t_neural_net *nn);

void forward_prop(t_neural_net *nn, double *input);

void back_prop(t_neural_net *nn, double *expected);

void free_nn(t_neural_net *nn);

#endif
