#include "../headers/neuralnet.h"
#include "../headers/layer.h"
#include "../headers/utils.h"
#include <stdio.h>
#include <stdlib.h>

t_neural_net *create_nn(const int input_count, const int hidden_layer_count,
                        const int neuron_count, const int output_count) {
    t_neural_net *nn = malloc(sizeof(t_neural_net));
    nn->input_count = input_count;
    nn->hidden_layer_count = hidden_layer_count;
    nn->neuron_count = neuron_count;
    nn->output_count = output_count;
    t_layer **layers = malloc(sizeof(t_layer *) * (hidden_layer_count + 2));
    layers[0] = create_layer(0, input_count, false, NULL, false);
    int prev = input_count;
    for (int i = 0; i < hidden_layer_count; i++) {
        layers[i + 1] = create_layer(prev, neuron_count, true, NULL, false);
        prev = neuron_count;
    }
    layers[hidden_layer_count + 1] =
        create_layer(prev, output_count, true, NULL, true);
    nn->layers = layers;
    return nn;
}

void free_nn(t_neural_net *nn) {
    for (int i = 0; i < nn->hidden_layer_count + 2; i++) {
        /* printf("Freeing the %dth layer\n", i); */
        free_layer(nn->layers[i]);
    }
    free(nn->layers);
    free(nn);
}

void print_nn(const t_neural_net *nn) {
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

void forward_prop(t_neural_net *nn, double *input) {
    double *prev_output = input;
    printf("Starting forward propagation\n");
    for (int i = 0; i <= nn->hidden_layer_count; i++) {
        process_input(nn->layers[i + 1], prev_output);
        prev_output = nn->layers[i + 1]->values;
    }
}

double error(double *expected, double *result) { return *expected - *result; }

void back_prop(t_neural_net *nn, double *expected) {
    // TODO : Back prop with multiple outputs, see
    // https://cs.stackexchange.com/questions/35266/ann-backpropagation-with-multiple-output-neurons
    (void)nn;
    (void)expected;
    printf("Starting back propagation\n");
    /* backprop_output(nn->layers[nn->hidden_layer_count + 1], expected); */
    t_layer *output_layer = nn->layers[nn->hidden_layer_count + 1];
    /* double d_output_sum; */
    /* for (int i = 0; i < output_layer->neuron_count; i++) { */
    /*     printf("Backprop on neuron %d\n", i); */
    /*     double target = expected[i]; */
    /*     double error_margin = target - output_layer->values[i]; */
    /*     printf("Target : %f | Value : %f | Margin : %f\n", target, */
    /*            output_layer->values[i], error_margin); */
    /*     d_output_sum = sigmoid_deriv(output_layer->values[i]) * error_margin;
     */
    /*     printf("dOutputSum : %f\n", d_output_sum); */
    /*     printf("Previous hidden layer values : "); */
    /*     t_layer *prev_layer = nn->layers[nn->hidden_layer_count]; */
    /*     print_double_arr(prev_layer->values, nn->neuron_count); */
    /*     double *d_weights = div_scalar_by_array( */
    /*         d_output_sum, prev_layer->values, nn->neuron_count); */
    /*     printf("dWeights : "); */
    /*     print_double_arr(d_weights, nn->neuron_count); */
    /*     for (int y = 0; y < nn->neuron_count; y++) { */
    /*         output_layer->weights[i][y] = */
    /*             output_layer->weights[i][y] + d_weights[y]; */
    /*     } */
    /*     free(d_weights); */
    /* } */

    /* double *deltas = malloc(output_layer->neuron_count * sizeof(double)); */
    /* for (int i = 0; i < output_layer->neuron_count; i++) { */
    /*     double error = expected[i] - output_layer->values[i]; */
    /*     deltas[i] = */
    /*         output_layer->values[i] * (1 - output_layer->values[i]) * error;
     */
    /*     double s = 0; */
    /*     for(int y = 0; y < output_layer->prev_layer_size; y++){ */
    /*         s += deltas[i] * output_layer->weights[i][y]; */
    /*     } */
    /*     deltas[i] = nn->layers[nn->hidden_layer_count]->values[i] * s; */
    /* } */
    /* for(int x = 0; x < nn->hidden_layer_count; x++){ */
    /*     t_layer *next_layer = nn->layers[x + 1]; */

    for (int i = 0; i < output_layer->neuron_count; i++) {
        double err = error(&expected[i], &output_layer->values[i]);
        printf("Error on output layer[%d]: %f\n", i, err);
        output_layer->deltas[i] = sigmoid_deriv(output_layer->values[i]) * err;
    }

    for (int i = 0; i < nn->hidden_layer_count + 1; i++) {
        t_layer *l = nn->layers[i];
        if (l->output_layer) {
            printf("Got an output layer\n");
            exit(1);
        }
        for (int y = 0; y < l->neuron_count; y++) {
            double s = 0;
            t_layer *next_layer = nn->layers[i + 1];
            for (int x = 0; x < next_layer->neuron_count; x++) {
                s += next_layer->deltas[x] * next_layer->weights[x][y];
            }
            l->deltas[y] = l->values[y] * (1 - l->values[y]) * s;
        }
    }

    for (int i = 0; i < nn->hidden_layer_count + 1; i++) {
        printf("On layer %d\n", i);
        t_layer *next = nn->layers[i + 1];
        t_layer *curr = nn->layers[i];
        if (next->output_layer) {
            for (int y = 0; y < next->neuron_count; y++) {
                for(int x = 0; x < curr->neuron_count; x++){
                    next->weights[y][x] += next->deltas[y] * next->values[y];
                }
            }
        }
        else{
            for (int y = 0; y < next->neuron_count; y++) {
                for(int x = 0; x < curr->neuron_count; x++){
                    next->weights[y][x] += next->deltas[y] * curr->values[y];
                }
            }
        }
    }

    /* for(int i = 0; i < next_layer->neuron_count; i++){ */
    /*     for(int y = 0; y < next_layer->prev_layer_size; y++){ */
    /*         if(next_layer->output_layer){ */
    /*             next_layer[] */
    /*         } */
    /*         else{ */

    /*         } */
    /*     } */
    /* } */
}

/* void backprop_output(t_layer *layer, double *expected) { */
/*     if (!layer->output_layer) { */
/*         printf("Trying to backprop_output on a hidden layer\n"); */
/*         exit(0); */
/*     } */
/*     printf("Starting backprop_output\n"); */
/*     (void)expected; */
/*     for (int i = 0; i < layer->neuron_count; i++) { */
/*         printf("Backprop on neuron %d\n", i); */
/*         double target = expected[i]; */
/*         double error_margin = target - layer->values[i]; */
/*         printf("Target : %f | Value : %f | Margin : %f\n", target, */
/*                layer->values[i], error_margin); */
/*         double d_output_sum = sigmoid_deriv(layer->values[i]) * error_margin;
 */
/*         printf("dOutputSum : %f\n", d_output_sum); */
/*     } */
/* } */
