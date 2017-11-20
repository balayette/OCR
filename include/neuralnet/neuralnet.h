/**
 * \file neuralnet.h
 * \brief Neural network header
 */
#ifndef NEURAL_NET_H
#define NEURAL_NET_H

#include "layer.h"

/*

A neural network is made up of (hidden_layer_count + 2) layers :
  hidden_layer_count hidden layers + 1 input layer + 1 output layer.

*/
/**
 * \struct neural_net
 * \brief Neural network struct
 */
struct neural_net {
    int input_count;
    int hidden_layer_count;
    int neuron_count;
    int output_count;
    struct layer **layers;
    double bias;
};

/**
 * \brief Create a neural network
 * \param input_count Number of inputs
 * \param hidden_layer_count Number of hidden layers
 * \param neuron_count Number of neurons in the hidden layers
 * \param output_count Number of outputs
 * \return A pointer to a struct neural_net
 */
struct neural_net *create_nn(const int input_count, const int hidden_layer_count,
                        const int neuron_count, const int output_count);

/**
 * \brief Print a neural network to stdout
 * \param nn Pointer to a struct neural_net
 */
void print_nn(struct neural_net *nn);

/**
 * \brief Applies the forward propagation to a neural network
 * \param nn Pointer to a struct neural_net
 * \param input Array of double. Contains the input
 */
void forward_prop(struct neural_net *nn, double *input);

/**
 * \brief Applies the back propagation to a neural network
 * \param nn Pointer to a struct neural_net
 * \param expected Array of double. Contains the expected output
 * \param input Array of double. Contains the previous input.
 */
void back_prop(struct neural_net *nn, double *expected, double *input);

/**
 * \brief Free all the resources used by a neural network
 * \param nn Pointer to a struct neural_net
 */
void free_nn(struct neural_net *nn);

#endif
