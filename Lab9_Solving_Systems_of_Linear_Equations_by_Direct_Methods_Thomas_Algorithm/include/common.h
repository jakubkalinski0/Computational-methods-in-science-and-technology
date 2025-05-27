#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <time.h>

// ----- Task Specific Constants -----
#define M_PARAM 4.0
#define K_PARAM 5.0
#define N_MIN 2
#define N_MAX 500
#define FIXED_SEED 30 // Seed for generating x_true

// ----- Core Structures -----
typedef struct {
    double **data;
    int rows;
    int cols;
} Matrix;

typedef struct {
    double *data;
    int size;
} Vector;

// ----- Experiment Result Structure -----
typedef struct {
    int size_n; // N

    // Gaussian Elimination (Full Matrix)
    double err_gauss_f32;
    double time_gauss_f32_sec;
    double err_gauss_f64;
    double time_gauss_f64_sec;

    // Thomas Algorithm (Banded Matrix nx3)
    double err_thomas_f32;
    double time_thomas_f32_sec;
    double err_thomas_f64;
    double time_thomas_f64_sec;

    // Theoretical Memory for Matrix A storage (in KB)
    double mem_gauss_f32_kb;
    double mem_thomas_f32_kb;
    double mem_gauss_f64_kb;
    double mem_thomas_f64_kb;
} ExperimentResult;

// ----- Utility Macros -----
#define CHECK_ALLOC(ptr, msg) \
do { \
if (!(ptr)) { \
fprintf(stderr, "ERROR: Memory allocation failed for %s in %s at line %d.\n", msg, __FILE__, __LINE__); \
exit(EXIT_FAILURE); \
} \
} while (0)

// Helper for casting values to specified precision
static inline double cast_to_prec(double val, const char* precision_dtype) {
    if (strcmp(precision_dtype, "float") == 0) {
        return (float)val;
    }
    return val; // double
}

// Helper for getting precision-specific epsilon
static inline double get_prec_epsilon(const char* precision_dtype) {
    if (strcmp(precision_dtype, "float") == 0) {
        return FLT_EPSILON;
    }
    return DBL_EPSILON;
}

#endif // COMMON_H