/**
 * @file approximation.h
 * @brief Header file for least-squares polynomial approximation functions.
 *
 * Defines function prototypes for calculating the coefficients of a best-fit
 * polynomial using the method of least squares and for evaluating the
 * resulting polynomial at a given point.
 */
#ifndef APPROXIMATION_H
#define APPROXIMATION_H

#include "common.h"

/**
 * @brief Calculates the coefficients of the best-fit polynomial using least squares.
 *
 * Finds the polynomial P(x) = a_0 + a_1*x + ... + a_m*x^m of degree `degree_m`
 * that minimizes the sum of squared errors Sum[(P(x_i) - y_i)^2] for a given
 * set of `n` data points (x_i, y_i). This involves setting up and solving the
 * normal equations (G * a = B), where G is the Gram matrix and 'a' is the vector
 * of unknown coefficients.
 *
 * @param points_x Array of x-coordinates of the data points (size `n`).
 * @param points_y Array of y-coordinates of the data points (size `n`).
 * @param n The number of data points. Must be greater than `degree_m`.
 * @param degree_m The degree of the approximating polynomial. Must be non-negative and less than `n`.
 * @param coeffs_out Output array (size `degree_m + 1`) where the calculated
 *                   coefficients [a_0, a_1, ..., a_m] will be stored.
 *                   The caller must ensure this array has sufficient size.
 * @return 0 on success.
 * @return -1 on failure, which can occur due to:
 *         - Invalid input (e.g., `n <= degree_m`, `degree_m < 0`).
 *         - Memory allocation error during setup of the normal equations.
 *         - Failure to solve the linear system (e.g., singular or ill-conditioned Gram matrix).
 *         Error messages are printed to stderr in case of failure.
 */
int leastSquaresApprox(double points_x[], double points_y[], int n, int degree_m, double coeffs_out[]);

/**
 * @brief Evaluates a polynomial P(x) at a given point x.
 *
 * Calculates the value of the polynomial defined by its coefficients:
 * P(x) = coeffs[0] + coeffs[1]*x + coeffs[2]*x^2 + ... + coeffs[degree]*x^degree.
 * Uses Horner's method for efficient and numerically stable evaluation.
 *
 * @param x The point (independent variable value) at which to evaluate the polynomial.
 * @param coeffs Array of polynomial coefficients [a_0, a_1, ..., a_degree], where
 *               `coeffs[i]` is the coefficient of the x^i term.
 * @param degree The degree of the polynomial. The size of the `coeffs` array must be `degree + 1`.
 * @return The computed value of the polynomial P(x).
 */
double evaluatePolynomial(double x, double coeffs[], int degree);

#endif // APPROXIMATION_H