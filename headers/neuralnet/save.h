#ifndef SAVE_NN_H
#define SAVE_NN_H

#include "neuralnet.h"
#include <stdio.h>

void save_nn(t_neural_net *nn, char *path);

t_neural_net * load_nn(char *path);

#endif
