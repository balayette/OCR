#include "neuralnet/neuralnet.h"
#include "neuralnet/layer.h"
#include "misc/utils.h"
#include <stdio.h>
#include <stdlib.h>

struct neural_net *create_nn(const int input_count, const int hidden_layer_count,
                        const int neuron_count, const int output_count) {
    struct neural_net *nn = malloc(sizeof(struct neural_net));
    nn->input_count = input_count;
    nn->hidden_layer_count = hidden_layer_count;
    nn->neuron_count = neuron_count;
    nn->output_count = output_count;
    nn->bias = 1;
    struct layer **layers = malloc(sizeof(struct layer *) * (hidden_layer_count + 2));
    layers[0] = create_layer(0, input_count, false, NULL, false, nn->bias);
    int prev = input_count;
    for (int i = 0; i < hidden_layer_count; i++) {
        layers[i + 1] = create_layer(prev, neuron_count, true, NULL, false, nn->bias);
        prev = neuron_count;
    }
    layers[hidden_layer_count + 1] =
        create_layer(prev, output_count, true, NULL, true, nn->bias);
    nn->layers = layers;
    return nn;
}

void free_nn(struct neural_net *nn) {
    for (int i = 0; i < nn->hidden_layer_count + 2; i++) {
        printf("Freeing the %dth layer\n", i);
        free_layer(nn->layers[i]);
    }
    free(nn->layers);
    free(nn);
}

void print_nn(struct neural_net *nn) {
    for (int i = 0; i <= nn->hidden_layer_count; i++) {
        printf("Neuron layer %d\n", i);
        for (int y = 0; y < nn->layers[i + 1]->neuron_count; y++) {
            printf("    Weights of neuron %d. Value : %f. Hidden value : %f", y,
                   nn->layers[i + 1]->values[y],
                   nn->layers[i + 1]->hidden_values[y]);
            printf("        ");
            print_double_arr(nn->layers[i + 1]->weights[y],
                             nn->layers[i + 1]->prev_layer_size);
        }
    }
}

void forward_prop(struct neural_net *nn, double *input) {
    double *prev_output = input;
    /* printf("Starting forward propagation\n"); */
    for (int i = 0; i <= nn->hidden_layer_count; i++) {
        process_input(nn->layers[i + 1], prev_output);
        prev_output = nn->layers[i + 1]->values;
    }
}

double error(double *expected, double *result) { return *expected - *result; }

void back_prop(struct neural_net *nn, double *expected, double *input) {
    // We assume that the neural network only has 1 hidden layer
    // (For now)
    struct layer *output_layer = nn->layers[nn->hidden_layer_count + 1];
    for (int i = 0; i < nn->output_count; i++) {
        // We are iterating over the nodes of the output layer

        double err = error(&expected[i], &output_layer->values[i]);

        // The delta of a node is equal to the derivative of the activation
        // function of the output of the node * the error of the node

        output_layer->deltas[i] = sigmoid_deriv(output_layer->values[i]) * err;
    }

    struct layer *hidden_layer = nn->layers[1];
    for (int i = 0; i < hidden_layer->neuron_count; i++) {
        // We are iterating over the nodes of the hidden layers

        // The delta of a hidden node is equal to the derivative of the
        // activation function of the output of the node * the sum of
        // delta * weight from the neuron to the output neuron, for all
        // neurons of the output layer

        double deriv = sigmoid_deriv(hidden_layer->values[i]);
        double sum = 0;
        for (int k = 0; k < output_layer->neuron_count; k++) {
            // weights[k][i] is the weigth from the hidden neuron i to the
            // output neuron k
            sum += output_layer->deltas[k] * output_layer->weights[k][i];
        }
        hidden_layer->deltas[i] = deriv * sum;
    }

    // Finally, we can update the weights
    // New weights are computed as follows :
    // - Weights that link to an output neuron = weight + delta * output of the
    // hidden neuron on the other side of the link.
    // - Weights that link to a hidden neuron = weight + delta * input of the
    // neuron

    for (int neuron = 0; neuron < output_layer->neuron_count; neuron++) {
        // iterating over the weights of the output layer
        for (int prev_n = 0; prev_n < hidden_layer->neuron_count; prev_n++) {
            output_layer->weights[neuron][prev_n] +=
                output_layer->deltas[neuron] * hidden_layer->values[prev_n];
        }
        output_layer->bias_weights[neuron] += output_layer->deltas[neuron];
    }

    struct layer *input_layer = nn->layers[0];

    for (int neuron = 0; neuron < hidden_layer->neuron_count; neuron++) {
        // iterating over the weights of the hidden layer
        for (int prev_n = 0; prev_n < input_layer->neuron_count; prev_n++) {
            hidden_layer->weights[neuron][prev_n] +=
                hidden_layer->deltas[neuron] *
                input[prev_n];
        }
        hidden_layer->bias_weights[neuron] += hidden_layer->deltas[neuron];
    }
}
