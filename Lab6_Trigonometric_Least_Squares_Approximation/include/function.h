/**
* @file function.h
 * @brief Header file defining the mathematical function to be analyzed and its derivative.
 *
 * Declares the function `f(x)` which is the target for approximation in this project,
 * and its first derivative `df(x)`, which might be needed for other numerical methods
 * (though not strictly required for basic least-squares approximation).
 * It includes common.h for access to global constants (k, m, PI, a, b) used in the function definitions.
 */
#ifndef FUNCTION_H
#define FUNCTION_H

#include "common.h" // Provides k, m, PI, a, b

/**
 * @brief Computes the value of the primary function f(x) for the analysis.
 *
 * The function is defined mathematically as:
 * f(x) = sin(k * x / PI) * exp(-m * x / PI)
 * The parameters `k`, `m`, and the constant `PI` are defined globally
 * (declared in common.h, defined in function.c). This function forms the basis
 * for generating sample data and for comparing against the approximation results.
 *
 * @param x The input value (independent variable) at which to evaluate the function.
 * @return The computed value of f(x).
 */
double f(double x);

/**
 * @brief Computes the value of the first derivative f'(x) (or df/dx) of the primary function.
 *
 * The derivative of f(x) = sin(k*x/PI) * exp(-m*x/PI) is calculated using the product rule:
 * f'(x) = (1/PI) * exp(-m*x/PI) * [ k*cos(k*x/PI) - m*sin(k*x/PI) ]
 * This derivative might be used in more advanced approximation or interpolation techniques
 * (e.g., Hermite interpolation) or for analyzing the function's behavior.
 * The parameters `k`, `m`, and `PI` are global constants.
 *
 * @param x The input value (independent variable) at which to evaluate the derivative.
 * @return The computed value of the first derivative f'(x).
 */
double df(double x);

#endif // FUNCTION_H