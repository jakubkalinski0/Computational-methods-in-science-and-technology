/**
 * @file interpolation.h
 * @brief Header file for spline interpolation algorithms.
 *
 * Defines function prototypes for cubic and quadratic spline interpolation.
 */
#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "common.h" // Includes BoundaryConditionType

/**
 * @brief Performs cubic spline interpolation.
 *
 * Calculates the coefficients for the cubic spline interpolating the given data points (nodes, values)
 * using the specified boundary conditions. Then evaluates the spline at point 'x'.
 *
 * @param x The point at which to evaluate the interpolation.
 * @param nodes Array of x-coordinates of the interpolation nodes (size n, n >= 2). Must be sorted.
 * @param values Array of y-coordinates (function values) at the nodes (size n).
 * @param n The number of interpolation nodes (must be >= 2).
 * @param bc_type The type of boundary condition to use (BOUNDARY_NATURAL or BOUNDARY_CLAMPED).
 * @param deriv_a Optional: The derivative value f'(a) needed for BOUNDARY_CLAMPED. Ignored otherwise.
 * @param deriv_b Optional: The derivative value f'(b) needed for BOUNDARY_CLAMPED. Ignored otherwise.
 * @return The interpolated value at point 'x'. Returns NAN on error (e.g., memory allocation failure, invalid input, n < 2).
 */
double cubicSplineInterpolation(double x, double nodes[], double values[], int n,
                                BoundaryConditionType bc_type, double deriv_a, double deriv_b);

/**
 * @brief Performs quadratic spline interpolation.
 *
 * Calculates the coefficients for the quadratic spline interpolating the given data points (nodes, values)
 * using the specified boundary condition at the start point. Then evaluates the spline at point 'x'.
 *
 * @param x The point at which to evaluate the interpolation.
 * @param nodes Array of x-coordinates of the interpolation nodes (size n, n >= 2). Must be sorted.
 * @param values Array of y-coordinates (function values) at the nodes (size n).
 * @param n The number of interpolation nodes (must be >= 2).
 * @param bc_type The type of boundary condition to use at the start node 'a' (e.g., BOUNDARY_CLAMPED or BOUNDARY_ZERO_SLOPE_START).
 * @param deriv_a Optional: The derivative value f'(a) needed for BOUNDARY_CLAMPED. Ignored otherwise.
 * @return The interpolated value at point 'x'. Returns NAN on error (e.g., memory allocation failure, invalid input, n < 2).
 */
double quadraticSplineInterpolation(double x, double nodes[], double values[], int n,
                                    BoundaryConditionType bc_type, double deriv_a);


#endif // INTERPOLATION_H