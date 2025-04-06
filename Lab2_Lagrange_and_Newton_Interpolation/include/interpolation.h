/**
* @file interpolation.h
 * @brief Header file for interpolation algorithms.
 *
 * Defines function prototypes for Lagrange and Newton interpolation methods.
 */
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "common.h"

/**
 * @brief Performs Lagrange interpolation.
 *
 * Calculates the interpolated value at a point 'x' using the Lagrange polynomial
 * constructed from the given nodes and their corresponding function values.
 *
 * @param x The point at which to evaluate the interpolation.
 * @param nodes Array of x-coordinates of the interpolation nodes.
 * @param values Array of y-coordinates (function values) at the interpolation nodes.
 * @param n The number of interpolation nodes.
 * @return The interpolated value at point 'x'.
 */
double lagrangeInterpolation(double x, double nodes[], double values[], int n);

/**
 * @brief Performs Newton interpolation using divided differences.
 *
 * Calculates the interpolated value at a point 'x' using the Newton polynomial
 * constructed from the given nodes and their corresponding function values.
 *
 * @param x The point at which to evaluate the interpolation.
 * @param nodes Array of x-coordinates of the interpolation nodes.
 * @param values Array of y-coordinates (function values) at the interpolation nodes.
 * @param n The number of interpolation nodes.
 * @return The interpolated value at point 'x'. Returns NAN if n < 1 or if division by zero occurs.
 */
double newtonInterpolation(double x, double nodes[], double values[], int n);

#endif // INTERPOLATION_H