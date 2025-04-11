/**
* @file function.h
 * @brief Header file for the function to be interpolated.
 *
 * Defines the function signature and necessary includes.
 */
#ifndef FUNCTION_H
#define FUNCTION_H

#include "common.h"

/**
 * @brief Computes the value of the function f(x) to be interpolated.
 *
 * The function is defined as f(x) = sin(k * x / PI) * exp(-m * x / PI).
 * Parameters k, m, and PI are defined in common.h / function.c.
 *
 * @param x The input value (independent variable).
 * @return The computed value of the function f(x).
 */
double f(double x);

/**
 * @brief Computes the first derivative of the function f(x) with respect to x.
 *
 * This function calculates f'(x), the derivative of f(x) = sin(k * x / PI) * exp(-m * x / PI).
 * Needed for clamped boundary conditions.
 *
 * @param x The input value (independent variable) at which to evaluate the derivative.
 * @return The computed value of the first derivative f'(x).
 */
double df(double x);

#endif // FUNCTION_H