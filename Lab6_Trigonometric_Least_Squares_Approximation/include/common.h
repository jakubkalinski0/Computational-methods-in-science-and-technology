/**
* @file common.h
 * @brief Common header file providing essential includes, constants, and definitions for the project.
 *
 * Includes standard C libraries frequently used throughout the project.
 * Defines project-wide constants such as the maximum number of nodes/points
 * allowed and declares external global constants related to the function being
 * analyzed (defined in function.c), like the interval [a, b] and function parameters k, m.
 * This promotes consistency and avoids redundant declarations.
 */
#ifndef COMMON_H
#define COMMON_H

// Standard Library Includes
#include <stdio.h>  // For standard input/output functions (printf, fprintf, fopen, etc.)
#include <stdlib.h> // For general utility functions (malloc, free, exit, etc.)
#include <math.h>   // For mathematical functions (sin, exp, fabs, pow, cos, NAN, etc.)

// Project-Specific Constants
/**
 * @brief Maximum number of interpolation nodes or approximation sample points allowed.
 *
 * Used to statically declare array sizes in some parts of the code (e.g., main.c).
 * Adjust if larger datasets are needed, but consider memory implications.
 */
#define MAX_NODES 500

// External Global Constant Declarations (defined in function.c)
// These define the mathematical context of the problem being solved.
extern const double PI; // Mathematical constant Pi (~3.14159...)
extern const double k;  // Parameter 'k' used in the definition of function f(x).
extern const double m;  // Parameter 'm' used in the definition of function f(x).
extern const double a;  // Start point of the primary interval [a, b] for analysis.
extern const double b;  // End point of the primary interval [a, b] for analysis.

#endif // COMMON_H