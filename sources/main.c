#include "../headers/neuralnet.h"
#include "../headers/utils.h"
#include <stdio.h>
#include <stdlib.h>
/* #define NEURON_COUNT 3 */
/* #define INPUT_COUNT 2 */
/* #define WEIGHT_COUNT (NEURON_COUNT * (INPUT_COUNT + 1)) */

/* double *init_weights() { */
/*   double *arr = malloc(sizeof(double) * WEIGHT_COUNT); */
/*   for (int i = 0; i < WEIGHT_COUNT; i++) { */
/*     arr[i] = rand() / (double)RAND_MAX; */
/*   } */
/*   return arr; */
/* } */

/* void process_input(int *input, double *weights, const int expected) { */
/*   printf("Applying one input to the neural net\n"); */
/*   double *values = calloc(NEURON_COUNT, sizeof(double)); */
/*   for (int i = 0; i < NEURON_COUNT; i++) { */
/*     for (int y = 0; y < INPUT_COUNT; y++) { */
/*       values[i] += input[y] * weights[y * NEURON_COUNT + i]; */
/*     } */
/*   } */
/*   // We have raw weighted values. */
/*   // Let us apply sigmoid(X) */
/*   printf("Before S(X) : "); */
/*   print_double_arr(values, NEURON_COUNT); */
/*   mapi(sigmoid, values, NEURON_COUNT); */
/*   printf("After S(X) : "); */
/*   print_double_arr(values, NEURON_COUNT); */
/*   double sum = sum_double_arr(values, NEURON_COUNT); */
/*   printf("Hidden layer sum : %f\n", sum); */
/*   double sumsig = sigmoid(sum); */
/*   printf("Hidden layer sum after S(X) : %f\n", sumsig); */
/*   double diff = expected - sumsig; */
/*   printf("Expected : %d | Got : %f | Delta : %f\n", expected, sumsig, diff);
 */
/*   double deltaout = sigmoid_deriv(sum) * diff; */
/*   printf("delta output sum = %f\n", deltaout); */
/*   double *deltaws = malloc(sizeof(double) * NEURON_COUNT); */
/*   for (int i = 0; i < NEURON_COUNT; i++) { */
/*     deltaws[i] = deltaout / values[i]; */
/*   } */
/*   print_double_arr(deltaws, NEURON_COUNT); */
/* } */

int main() {
    /* printf("Starting\n"); */
    /* printf("Neuron count : %d\nInput Count : %d\nWeight count : %d\n", */
    /*        NEURON_COUNT, INPUT_COUNT, WEIGHT_COUNT); */
    srand(0x1);
    /* int inputs[4][2] = {{0, 0}, {1, 1}, {1, 0}, {0, 1}}; */
    /* int outputs[4] = {0, 0, 1, 1}; */
    /* double *weights = init_weights(); */
    /* printf("Random starting weights : "); */
    /* print_double_arr(weights, WEIGHT_COUNT); */
    /* process_input(inputs[2], weights, outputs[2]); */
    /* return 0; */

    t_neural_net *nn = create_nn(2, 1, 3, 1);
    double output[2] = {1};
    double input[2] = {0, 1};
    for (int i = 0; i < 1; i++) {
        print_nn(nn);
        printf("---------------------\n");
        forward_prop(nn, input);
        printf("---------------------\n");
        print_nn(nn);
        printf("-----\n");
        back_prop(nn, output);
        printf("-----\n");
        print_nn(nn);
    }
    return 0;
}
