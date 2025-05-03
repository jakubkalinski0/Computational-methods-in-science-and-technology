/**
 * @file function.c
 * @brief Implementation of the mathematical function f(x) and its derivative f'(x),
 *        and definition of related global constants.
 *
 * This file defines the specific function being analyzed in the project,
 * f(x) = sin(k * x / PI) * exp(-m * x / PI),
 * its first derivative df(x), and the associated constants k, m, PI, a, b.
 * These constants and functions provide the mathematical context for the
 * approximation tasks performed elsewhere in the project.
 */
#include "../include/function.h"
#include <math.h> // For sin(), exp(), cos()

// Define Global Constants (declared 'extern' in common.h)
const double k = 4.0;  /**< Parameter 'k' affecting the frequency of the sine term in f(x). */
const double m = 0.4;  /**< Parameter 'm' affecting the decay rate of the exponential term in f(x). */
const double PI = 3.14159265358979323846; /**< Mathematical constant Pi, used for scaling within f(x). */
const double a = -2.0 * PI * PI; /**< Start of the interval [a, b] (~ -19.74) for analysis and plotting. */
const double b = PI * PI;        /**< End of the interval [a, b] (~ 9.87) for analysis and plotting. */

/**
 * @brief Computes the value of the primary function f(x) for the analysis.
 *
 * Calculates f(x) = sin(k * x / PI) * exp(-m * x / PI) using the globally
 * defined constants k, m, and PI.
 *
 * @param x The input value (independent variable).
 * @return The computed value of f(x).
 */
double f(double x) {
    // Calculate the argument for sin and exp scaled by PI
    double scaled_x_k = k * x / PI;
    double scaled_x_m = -m * x / PI; // Note the negative sign for the exponent

    // Calculate the sine term
    double sin_term = sin(scaled_x_k);
    // Calculate the exponential term
    double exp_term = exp(scaled_x_m);

    // Return the product of the two terms
    return sin_term * exp_term;
}

/**
 * @brief Computes the first derivative of the function f(x) with respect to x.
 *
 * Calculates f'(x) using the product rule for differentiation on
 * f(x) = sin(k*x/PI) * exp(-m*x/PI).
 * The resulting formula is:
 * f'(x) = (d/dx [sin(kx/PI)]) * exp(-mx/PI) + sin(kx/PI) * (d/dx [exp(-mx/PI)])
 *       = (k/PI * cos(kx/PI)) * exp(-mx/PI) + sin(kx/PI) * (-m/PI * exp(-mx/PI))
 *       = (exp(-mx/PI) / PI) * [ k*cos(kx/PI) - m*sin(kx/PI) ]
 * This function uses global constants k, m, and PI.
 *
 * @param x The input value (independent variable) at which to evaluate the derivative.
 * @return The computed value of the first derivative f'(x).
 */
double df(double x) {
    // Pre-calculate terms for efficiency and clarity
    double k_div_pi = k / PI;
    double m_div_pi = m / PI;
    double kx_div_pi = k_div_pi * x; // Argument for trig functions
    double neg_mx_div_pi = -m_div_pi * x; // Argument for exp function

    double cos_term = cos(kx_div_pi);
    double sin_term = sin(kx_div_pi);
    double exp_term = exp(neg_mx_div_pi);

    // Calculate the two parts resulting from the product rule
    double term1 = k_div_pi * cos_term * exp_term; // Derivative of sin part * exp part
    double term2 = sin_term * (-m_div_pi * exp_term); // sin part * Derivative of exp part

    // Return the sum, which is the total derivative f'(x)
    return term1 + term2;

    // Alternative compact calculation (matches the simplified formula):
    // return (exp_term / PI) * (k * cos_term - m * sin_term);
}