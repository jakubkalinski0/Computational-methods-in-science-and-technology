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
#include <stdbool.h> // For boolean types

// Maximum number of interpolation nodes allowed.
// Note: Splines require at least 2 nodes.
#define MAX_NODES 500

// External declaration of global constants defined in function.c
extern const double PI; // Mathematical constant Pi
extern const double k;  // Parameter 'k' for the function f(x)
extern const double m;  // Parameter 'm' for the function f(x)
extern const double a;  // Start of the interpolation interval
extern const double b;  // End of the interpolation interval

// Enum to define different boundary condition types for splines
typedef enum {
    BOUNDARY_NATURAL,       // Natural spline (cubic): S''(a)=0, S''(b)=0
    BOUNDARY_CLAMPED,       // Clamped spline (cubic/quadratic): S'(a)=f'(a), S'(b)=f'(b) (cubic) or S'(a)=f'(a) (quadratic)
    BOUNDARY_ZERO_SLOPE_START // Quadratic spline specific: S'(a)=0
    // Add other conditions here if needed, e.g., BOUNDARY_PERIODIC
} BoundaryConditionType;


#endif // COMMON_H