#include "neuralnet/save.h"
#include "misc/utils.h"
#include "neuralnet/layer.h"
#include "neuralnet/neuralnet.h"
#include <stdio.h>
#include <stdlib.h>

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

void save_nn(struct neural_net *nn, char *path) {
    FILE *file = fopen(path, "wb");
    if (!file) {
        printf("Couldn't open the file\n");
        exit(1);
    }

    fwrite(&nn->input_count, sizeof(int), 1, file);
    fwrite(&nn->output_count, sizeof(int), 1, file);
    fwrite(&nn->hidden_layer_count, sizeof(int), 1, file);
    fwrite(&nn->neuron_count, sizeof(int), 1, file);

    for (int i = 0; i < nn->hidden_layer_count; i++) {
        fwrite(nn->layers[i + 1]->bias_weights, sizeof(double),
               nn->neuron_count, file);

        // Change this when we move the weights to use a matrix
        // Will simplify the code a lot, we'll have to use the same piece of
        // code as for bias_weights

        for (int k = 0; k < nn->neuron_count; k++) {
            fwrite(nn->layers[i + 1]->weights[k], sizeof(double),
                   nn->layers[i + 1]->prev_layer_size, file);
        }
    }

    struct layer *output = nn->layers[nn->hidden_layer_count + 1];
    fwrite(output->bias_weights, sizeof(double),
           nn->output_count, file);
    for(int i = 0; i < output->neuron_count; i++){
        fwrite(output->weights[i], sizeof(double), output->prev_layer_size, file);
    }
    fclose(file);
}

struct neural_net *load_nn(char *path){
    FILE *file = fopen(path, "rb");
    int input_count, output_count, hidden_layer_count, neuron_count;

    if(!fread(&input_count, sizeof(int), 1, file))
    {
        printf("Couldn't read the input count of the neural network from the file\n");
        exit(1);
    }
    if(!fread(&output_count, sizeof(int), 1, file)){
        printf("Couldn't read the output count of the neural network from the file\n");
        exit(1);
    }
    if(!fread(&hidden_layer_count, sizeof(int), 1, file)){
        printf("Couldn't read the hidden layer count of the neural network from the file\n");
        exit(1);
    }
    if(!fread(&neuron_count, sizeof(int), 1, file)){
        printf("Couldn't read the neuron count of the neural network from the file\n");
        exit(1);
    }

    struct neural_net *nn = create_nn(input_count, hidden_layer_count, neuron_count, output_count);

    for(int i = 0; i < hidden_layer_count; i++){
        if(!fread(nn->layers[i + 1]->bias_weights, sizeof(double), nn->layers[i + 1]->neuron_count, file)){
            printf("Couldn't read the bias weights of the hidden layer %d\n", i);
            exit(1);
        }
        for(int k = 0; k < nn->layers[i + 1]->neuron_count; k++){
            if(!fread(nn->layers[i + 1]->weights[k], sizeof(double), nn->layers[i + 1]->prev_layer_size, file)){
                printf("Couldn't read the weights for neuron %d of layer %d\n", k, i);
                exit(1);
            }
        }
    }

    struct layer *output = nn->layers[nn->hidden_layer_count + 1];
    if(!fread(output->bias_weights, sizeof(double), output->neuron_count, file)){
        printf("Couldn't read the bias weights of the output layer\n");
        exit(1);
    }
    for(int k = 0; k < output->neuron_count; k++){
        if(!fread(output->weights[k], sizeof(double), output->prev_layer_size, file)){
            printf("Couldn't read the weights of neuron %d of output layer\n", k);
            exit(1);
        }
    }
    fclose(file);
    return nn;
}
