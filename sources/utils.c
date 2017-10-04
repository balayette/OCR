#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../headers/utils.h"

// In place map
void mapi(double (*f)(double), double *list, const int count) {
    for (int i = 0; i < count; i++)
        list[i] = f(list[i]);
}

double *map(double (*f)(double), double *input, const int count){
    double *output = malloc(sizeof(double) * count);
    for(int i = 0; i < count; i++){
        output[i] = f(input[i]);
    }
    return output;
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

bool cmp_double_array(const double *a1, const int size1, const double *a2, const int size2){
    if(size1 != size2)
        return false;
    bool ret = true;
    for(int i = 0; i < size1 && ret; i++)
        ret = a1[i] == a2[i];
    return ret;
}

double *div_scalar_by_array(double scalar, double *list, int count){
    double *ret = malloc(sizeof(double) * count);
    for(int i = 0; i < count; i++){
        ret[i] = scalar / list[i];
    }
    return ret;
}

double *multiply_arrays(double *a1, double *a2, int count){
    double *ret = malloc(sizeof(double) * count);
    for(int i = 0; i < count; i++)
        ret[i] = a1[i] * a2[i];
    return ret;
}
double threshold(double value) { return 0.5 > value ? 0 : 1; }
