/**
 * \brief Util functions
 * \file utils.h
 */
#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <stdbool.h>

/**
 * \brief Map in place on a double array
 * \param f Pointer to function returning double
 * \param list Double array
 * \param count Size of the array
 */
void mapi(double (*f)(double), double *list, const int count);

/**
 * \brief Map
 * \param f Pointer to function returning double
 * \param list Double array
 * \param count Size of the array
 */
double *map(double (*f)(double), double *list, const int count);

/**
 * \brief Sum an array of double
 * \param arr Double array
 * \param count Size of the array
 * \return Sum of the values of the array
 */
double sum_double_arr(double *arr, const int count);

/**
 * \brief Sigmoid function
 * \param a Input
 * \return Sigmoid(a)
 */
double sigmoid(double a);

/**
 * \brief Derivative of the sigmoid function
 * \param a Input
 * \return Sigmoid'(a)
 */
double sigmoid_deriv(double a);

void softmax(double *weighted_inputs, double *arr, int size);
void softmax2(double *weighted_inputs, double w_sum, double *arr, int size);

double softmax_deriv(double a);

/**
 * \brief Print an array of double
 * \param arr Double array
 * \param size Size of the array
 */
void print_double_arr(double *arr, const int count);

/**
 * \brief Compare two arrays of double
 * \param a1 Double array
 * \param size1 Size of a1
 * \param a2 Double array
 * \param size2 size of a2
 * \return true if all the values and sizes are the same.
 */
bool cmp_double_array(const double *a1, const int size1, const double *a2, const int size2);

/**
 * \brief Divide all values of an array by a scalar
 * \param scalar Scalar
 * \param list Double array
 * \param count Size of the array
 * \return Array divided by the scalar
 */
double *div_scalar_by_array(double scalar, double *list, int count);

/**
 * \brief Multiply the values of two arrays
 * \param a1 Double array
 * \param a2 Double array
 * \param count Size of the array
 * \return Double array
 */
double *multiply_arrays(double *a1, double *a2, int count);

double threshold(double value);

#endif
