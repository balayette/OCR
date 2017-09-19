#ifndef UTILS_H
#define UTILS_H

#include <math.h>

void map(double (*f)(double), double *list, const int count);

double sum_double_arr(double *arr, const int count);

double sigmoid(double a);

double sigmoid_deriv(double a);

void print_double_arr(double *arr, const int count);

#endif
