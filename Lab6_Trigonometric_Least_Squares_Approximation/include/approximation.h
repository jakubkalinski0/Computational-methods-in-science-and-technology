/**
 * @file approximation.h
 * @brief Header file for trigonometric approximation using direct summation formulas.
 *
 * Defines function prototypes for calculating the coefficients of a trigonometric sum
 * using direct formulas derived from discrete Fourier analysis principles, and for
 * evaluating the resulting sum. This approach assumes reasonably uniform sampling.
 */
#ifndef APPROXIMATION_H
#define APPROXIMATION_H

#include "common.h" // Provides omega, MAX_HARMONIC etc.

/**
 * @brief Calculates trigonometric coefficients using direct summation formulas.
 *
 * Computes coefficients $a_k$ and $b_k$ for the trigonometric sum:
 * $T_m(x) = a_0/2 + \sum_{k=1}^{m} [a_k \cos(k \omega x) + b_k \sin(k \omega x)]$
 * using the formulas which approximate Fourier integrals via summation:
 * $a_k = (2/n) \sum_{i=0}^{n-1} y_i \cos(k \omega x_i)$ for $k = 0, \dots, m$
 * $b_k = (2/n) \sum_{i=0}^{n-1} y_i \sin(k \omega x_i)$ for $k = 1, \dots, m$
 *
 * This method's validity as a least-squares solution relies on the (approximate)
 * orthogonality properties typically achieved with uniform sampling. It avoids
 * solving the potentially ill-conditioned normal equations but enforces the
 * condition $m < n/2$ for stability, derived from sampling theory.
 * The fundamental frequency $\omega$ is adapted to the interval $[a, b]$.
 *
 * @param points_x Array of x-coordinates of the data points (size `n`).
 * @param points_y Array of y-coordinates of the data points (size `n`).
 * @param n The number of data points. Must be > 0.
 * @param max_harmonic_m The maximum harmonic order (m). Must satisfy `m >= 0` and the
 *                       crucial condition `m < n/2` must hold for the formulas to be valid
 *                       and stable according to sampling theory.
 * @param coeffs_out Output array where the calculated coefficients will be stored.
 *                   It must have a size of at least `1 + 2*max_harmonic_m`.
 *                   The coefficients are stored in the order: $[a_0, a_1, b_1, a_2, b_2, \dots, a_m, b_m]$.
 *                   The caller must ensure sufficient size.
 * @return 0 on success.
 * @return -1 on failure, which occurs if input constraints are violated (e.g., `m < 0`, `n <= 0`, or `m >= n/2`).
 *         Error messages are printed to stderr in case of failure.
 */
int calculateTrigonometricCoeffsDirect(double points_x[], double points_y[], int n, int max_harmonic_m, double coeffs_out[]);

/**
 * @brief Evaluates a trigonometric sum $T_m(x)$ at a given point x.
 *
 * Calculates the value of the trigonometric sum defined by its coefficients:
 * $T_m(x) = \text{coeffs}[0]/2 + \sum_{k=1}^{m} [\text{coeffs}[2k-1] \cos(k \omega x) + \text{coeffs}[2k] \sin(k \omega x)]$
 * where `m` is the maximum harmonic order provided. The fundamental frequency $\omega$
 * is taken from global constants.
 *
 * @param x The point (independent variable value) at which to evaluate the sum.
 * @param coeffs Array of trigonometric coefficients $[a_0, a_1, b_1, a_2, b_2, \dots, a_m, b_m]$.
 *               The size of the `coeffs` array must be at least `1 + 2*max_harmonic_m`.
 * @param max_harmonic_m The maximum harmonic order (m) used in the sum. Must be non-negative (`>= 0`).
 * @return The computed value of the trigonometric sum $T_m(x)$. Returns NAN if `max_harmonic_m < 0`.
 */
double evaluateTrigonometricSum(double x, double coeffs[], int max_harmonic_m);

#endif // APPROXIMATION_H