#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <time.h>

// Global constants
#define FIXED_SEED 30
#define MAX_N_I 20
#define MAX_M_II 200

// Matrix Structure
typedef struct {
    double **data; // Always use double for matrix data internally for robustness during setup
    int rows;
    int cols;
} Matrix;

// Vector Structure
typedef struct {
    double *data; // Always use double for vector data
    int size;
} Vector;

// Experiment Result Structure
typedef struct {
    int size;
    double max_abs_error;    // Store as double, format on output
    double condition_number; // Store as double
    double time_solve_sec;
    double time_cond_sec;
} ExperimentResult;

// Utility Macro for memory allocation checking
#define CHECK_ALLOC(ptr, msg) \
do { \
if (!(ptr)) { \
fprintf(stderr, "Error: Memory allocation failed for %s in %s at line %d.\n", msg, __FILE__, __LINE__); \
exit(EXIT_FAILURE); \
} \
} while (0)

// Define an epsilon for comparisons, can be type-specific if needed
#define DEFAULT_EPS DBL_EPSILON // General purpose epsilon

#endif // COMMON_H