/**
* @file common.h
 * @brief Common header file for includes, constants, and definitions.
 *
 * Includes standard libraries and defines project-wide constants
 * like the interval [a, b], function parameters, and maximum node count.
 */
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>  // Standard input/output functions
#include <stdlib.h> // Standard library functions (malloc, exit, etc.)
#include <math.h>   // Mathematical functions (sin, exp, fabs, etc.)

// Maximum number of interpolation nodes allowed.
#define MAX_NODES 500

// External declaration of global constants defined in function.c
extern const double PI; // Mathematical constant Pi
extern const double k;  // Parameter 'k' for the function f(x)
extern const double m;  // Parameter 'm' for the function f(x)
extern const double a;  // Start of the interpolation interval
extern const double b;  // End of the interpolation interval

#endif // COMMON_H