#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../headers/utils.h"

// In place map
void map(double (*f)(double), double *list, const int count) {
    for (int i = 0; i < count; i++)
        list[i] = f(list[i]);
}

double sum_double_arr(double *arr, const int count) {
    double ret = 0;
    for (int i = 0; i < count; i++) {
        ret += arr[i];
    }
    return ret;
}

void print_double_arr(double *arr, const int count) {
    if(count == 1){
        printf("[%f]\n", arr[0]);
        return;
    }
    if(count == 0){
        printf("[]\n");
        return;
    }
    printf("[%f, ", arr[0]);
    for (int i = 1; i < count - 1; i++) {
        printf("%f, ", arr[i]);
    }
    printf("%f]\n", arr[count - 1]);
}
double sigmoid(double a) { return (1 / (1 + exp(-a))); }

double sigmoid_deriv(double a) { return a * (1 - a); }
