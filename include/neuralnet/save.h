#ifndef SAVE_NN_H
#define SAVE_NN_H

#include "neuralnet.h"
#include <stdio.h>

void save_nn(struct neural_net *nn, char *path);

struct neural_net *load_nn(char *path);

#endif
