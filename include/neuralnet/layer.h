/**
 * \file layer.h
 * \brief Layer of the neural network
*/
#ifndef LAYER_H
#define LAYER_H

#include <stdbool.h>
#include <stdlib.h>

/*
  Each layer contains neuron_count neurons.

  Each layer, except the input layer, is preceded by another layer that contains
  prev_layer_size neurons.

  All neurons (except the neurons of the input layer) have to store the weights
  of the neurons of the previous layer that are connected to them.
  Weights are stored in an array of arrays, indexed such that the weights can be
  accessed as follows :
    weights[i][y] is the weight from neuron number y on the previous layer to
    neuron number i on the current layer.

  The neurons don't need to be represented by a struct, they can be as easily
  represented by an array of values.
  The value of the neuron number i on the current layer can be accessed as
  follows : values[i]

  *hidden_values is used for internal computations, and contains the value of
  neuron before the application of the sigmoid function.

*/
/**
 * \brief Struct of layers of a neural network
 */
struct layer {
    int neuron_count;
    int prev_layer_size;
    bool output_layer;
    double **weights;
    double *hidden_values;
    double *values;
    double *deltas;
    double *bias_weights;
    double bias;
};

// Random weights will be used if random_weigths is set to true.
// Otherwise, **weights will be used.
/*
 * \brief Create a layer
 * \param prev_layer_size Number of neurons in the previous layer
 * \param neuron_count Number of neurons in the layer
 * \param random_weights If set to true, will generate random weights. If set to false, will use **weights
 * \param weights Matrix of weights
 * \param output_layer Set to true if the layer is the output layer
 * \param bias Bias to use for the layer
 */
struct layer *create_layer(const int prev_layer_size, const int neuron_count,
                      const bool random_weights, double **weights,
                      const bool output_layer, double bias);

/**
 * \brief Process an input
 * \param layer Pointer to a struct layer
 * \param input Array of double, contains the input
 */
void process_input(struct layer *layer, double *input);

/**
 * \brief Free all the resources used by a layer
 * \param Pointer to struct layer
 */
void free_layer(struct layer *layer);

#endif
