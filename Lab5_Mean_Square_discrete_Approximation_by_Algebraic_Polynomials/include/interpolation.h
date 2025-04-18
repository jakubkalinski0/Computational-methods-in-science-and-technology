/**
 * @file interpolation.h
 * @brief Header file for interpolation algorithms.
 *
 * Defines function prototype for Hermite interpolation method.
 */
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "common.h"

/**
 * @brief Performs Hermite interpolation using divided differences.
 *
 * Calculates the interpolated value at a point 'x' using the Hermite polynomial.
 * This polynomial matches both the function values and the first derivative values
 * at the given nodes. It uses a generalized divided difference table.
 *
 * @param x The point at which to evaluate the interpolation.
 * @param nodes Array of x-coordinates of the interpolation nodes (n nodes).
 * @param values Array of y-coordinates (function values) at the nodes (n values).
 * @param derivatives Array of first derivative values (f'(x)) at the nodes (n values).
 * @param n The number of distinct interpolation nodes (polynomial degree will be 2n-1).
 * @return The interpolated value at point 'x'. Returns NAN on error (e.g., memory allocation failure, invalid input).
 */
double hermiteInterpolation(double x, double nodes[], double values[], double derivatives[], int n);

#endif // INTERPOLATION_H