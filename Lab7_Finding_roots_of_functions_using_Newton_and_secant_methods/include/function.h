/**
* @file function.h
 * @brief Header file defining the mathematical function for root finding and its derivative.
 *
 * Declares the function `f(x)` whose root is sought, and its first derivative `df(x)`,
 * required for Newton's method. Includes common.h for access to global constants (a, b, N_param, M_param).
 */
#ifndef FUNCTION_H
#define FUNCTION_H

#include "common.h" // Provides N_param, M_param, a, b

/**
 * @brief Computes the value of the primary function f(x) = x^n + x^m for root finding.
 *
 * The function is defined mathematically as:
 * f(x) = x^n + x^m
 * The parameters `n` (N_param) and `m` (M_param) are defined globally
 * (declared in common.h, defined in function.c). We are looking for x such that f(x) = 0.
 *
 * @param x The input value (independent variable) at which to evaluate the function.
 * @return The computed value of f(x).
 */
double f(double x);

/**
 * @brief Computes the value of the first derivative f'(x) of the primary function.
 *
 * The derivative of f(x) = x^n + x^m is:
 * f'(x) = n*x^(n-1) + m*x^(m-1)
 * This derivative is needed for Newton's method.
 * The parameters `n` (N_param) and `m` (M_param) are global constants.
 *
 * @param x The input value (independent variable) at which to evaluate the derivative.
 * @return The computed value of the first derivative f'(x). Returns NAN if the calculation involves undefined terms (e.g., negative base with fractional exponent, although not the case here).
 */
double df(double x);

#endif // FUNCTION_H