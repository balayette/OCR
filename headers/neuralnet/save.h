#ifndef SAVE_NN_H
#define SAVE_NN_H

#include "neuralnet.h"
#include <stdio.h>

void save_nn(t_neural_net *nn);

void load_nn(FILE* file);

void binput(size_t const size, void const* const ptr, FILE* file);

#endif
