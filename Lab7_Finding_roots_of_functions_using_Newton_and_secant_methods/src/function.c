/**
* @file function.c
 * @brief Implementation of the mathematical function f(x) = x^n + x^m and its derivative f'(x),
 *        and definition of related global constants.
 */
#include "../include/function.h"
#include <math.h> // For pow()

// Define Global Constants (declared 'extern' in common.h)
// Assignment: n=14, m=13, interval [-1.4, 0.6]
const double N_param = 14.0; /**< Parameter 'n' in f(x) = x^n + x^m. */
const double M_param = 13.0; /**< Parameter 'm' in f(x) = x^n + x^m. */
const double a = -1.4;       /**< Start of the interval [a, b]. */
const double b = 0.6;        /**< End of the interval [a, b]. */

/**
 * @brief Computes the value of the primary function f(x) = x^n + x^m for root finding.
 */
double f(double x) {
    // For integer exponents n=14, m=13, pow() handles negative bases correctly.
    return pow(x, N_param) + pow(x, M_param);
}

/**
 * @brief Computes the first derivative f'(x) = n*x^(n-1) + m*x^(m-1).
 * (Simplified version for n=14, m=13, ZERO_TOLERANCE removed)
 */
double df(double x) {
    // Removed conditional checks as they are always false for n=14, m=13
    // and pow(0.0, positive_exponent) correctly returns 0.0.

    double term1 = N_param * pow(x, N_param - 1.0);
    double term2 = M_param * pow(x, M_param - 1.0);

    return term1 + term2;
}