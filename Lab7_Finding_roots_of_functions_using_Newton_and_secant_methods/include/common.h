/**
* @file common.h
 * @brief Common header file providing essential includes, constants, and definitions for the root-finding project.
 *
 * Includes standard C libraries frequently used throughout the project.
 * Defines project-wide constants such as maximum iterations and declares
 * external global constants related to the function being analyzed (defined in function.c),
 * like the interval [a, b] and function parameters N_param, M_param.
 */
#ifndef COMMON_H
#define COMMON_H

// Standard Library Includes
#include <stdio.h>  // For standard input/output functions (printf, fprintf, fopen, etc.)
#include <stdlib.h> // For general utility functions (malloc, free, exit, etc.)
#include <math.h>   // For mathematical functions (fabs, pow, NAN, etc.)

// Project-Specific Constants
/**
 * @brief Maximum number of iterations allowed for root-finding algorithms.
 * Prevents infinite loops in case of non-convergence.
 */
#define MAX_ITERATIONS 1000

// ZERO_TOLERANCE constant removed

// External Global Constant Declarations (defined in function.c)
// These define the mathematical context of the problem being solved.
extern const double a;       // Start point of the primary interval [a, b].
extern const double b;       // End point of the primary interval [a, b].
extern const double N_param; // Parameter 'n' in the function f(x) = x^n + x^m.
extern const double M_param; // Parameter 'm' in the function f(x) = x^n + x^m.

#endif // COMMON_H