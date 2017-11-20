/**
 * \file save.h
 * \brief Saving and loading a neural network
 */
#ifndef SAVE_NN_H
#define SAVE_NN_H

#include "neuralnet.h"
#include <stdio.h>

/*
  Format of the save file :

  input_count | output_count | hidden_layer_count | neuron_count
  int            int            int                 int

  for i in range(hidden_layer_count):

  k = hidden_layer[i]->neuron_count
  k * bias_weight   |     k * hidden_layer[i]->prev_layer_size weight    |
  k * double          k * hidden_layer[i]->prev_layer_size * double

  endfor

  k = output_count
  k * bias_weight     |     k * output_layer->prev_layer_size weight      |
  k * double            k * output_layer->prev_layer_size * double

*/

/**
 * \brief Save the neural network to a file
 * \param nn Pointer to a neural network
 * \param path Path to the save file
 */
void save_nn(struct neural_net *nn, char *path);

/**
 * \brief Load a neural network from a file
 * \param path Path to the file
 * \return A pointer to a struct neural_net
 */
struct neural_net *load_nn(char *path);

#endif
