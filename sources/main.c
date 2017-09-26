#include "../headers/neuralnet.h"
#include "../headers/utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char* argv[]) {
    if(argc != 2){
        printf("Wrong arguments.");
        exit(0);
    }
    int rep = atoi(argv[1]);
    srand(0x1);
    t_neural_net *nn = create_nn(2, 1, 3, 1);
    double output[2] = {0};
    double input[2] = {1, 1};
    for (int i = 1; i <= rep; i++) {
        forward_prop(nn, input);
        back_prop(nn, output);
        printf("Rep %d / %d | OUTPUT : ", i, rep);
        print_double_arr(nn->layers[nn->hidden_layer_count + 1]->values, nn->output_count);

    }
    free_nn(nn);
    return 0;
}
