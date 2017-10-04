#include "../headers/neuralnet.h"
#include "../headers/utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        printf("Wrong arguments.");
        exit(0);
    }
    int rep = atoi(argv[1]);
    srand(0xDEADBEEF);

    t_neural_net *nn = create_nn(2, 1, 3, 1);
    double input[4][2] = {{1, 1}, {0, 0}, {1, 0}, {0, 1}};

    double output[4][1] = {{0}, {0}, {1}, {1}};

    /* t_neural_net *nn = create_nn(1, 1, 3, 1); */
    /* double input [2][1] = {{0}, {1}}; */
    /* double output [2][1] = {{1}, {0}}; */
        for (int i = 1; i <= rep; i++) {
            int k = rand() % 4;
            forward_prop(nn, input[k]);
            back_prop(nn, output[k]);
            printf("Rep : %d / %d | INPUT : ", i, rep);
            print_double_arr(input[k], nn->input_count);
            printf("OUTPUT : ");
            print_double_arr(nn->layers[2]->values, nn->output_count);
            print_nn(nn);
        }
    free_nn(nn);
    return 0;
}
