#include "../headers/neuralnet.h"
#include "../headers/utils.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    srand(0x1);
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
    free_nn(nn);
    return 0;
}
