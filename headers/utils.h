#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <stdbool.h>

void mapi(double (*f)(double), double *list, const int count);

double *map(double (*f)(double), double *list, const int count);

double sum_double_arr(double *arr, const int count);

double sigmoid(double a);

double sigmoid_deriv(double a);

void print_double_arr(double *arr, const int count);

bool cmp_double_array(const double *a1, const int size1, const double *a2, const int size2);

double *div_scalar_by_array(double scalar, double *list, int count);

double *multiply_arrays(double *a1, double *a2, int count);

double threshold(double value);

#endif
