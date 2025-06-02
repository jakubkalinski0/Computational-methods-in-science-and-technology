#ifndef COMMON_H
#define COMMON_H

// Najpierw standardowe nagłówki, które mogą definiować malloc/free/calloc
#include <stdio.h>
#include <stdlib.h> // Definiuje standardowe malloc, free, calloc
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <float.h>
#include <time.h>

// TERAZ dołącz tracker, który (jeśli MEMORY_TRACKING_ENABLED) nadpisze malloc/free/calloc makrami
#include "memory_tracker.h"

// ----- Task Specific Constants -----
#define M_PARAM 4.0
#define K_PARAM 5.0
#define N_MIN 2
#define N_MAX 500 // Można zmniejszyć do testów, np. 50
#define FIXED_SEED 30

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
    double mem_gauss_f32_kb; // Dynamic peak memory
    double err_gauss_f64;
    double time_gauss_f64_sec;
    double mem_gauss_f64_kb; // Dynamic peak memory

    // Thomas Algorithm (Banded Matrix Nx3 storage)
    double err_thomas_banded_f32;
    double time_thomas_banded_f32_sec;
    double mem_thomas_banded_f32_kb; // Dynamic peak memory
    double err_thomas_banded_f64;
    double time_thomas_banded_f64_sec;
    double mem_thomas_banded_f64_kb; // Dynamic peak memory

    // Thomas Algorithm (Full Matrix NxN storage, then conversion for solve)
    double err_thomas_full_f32;
    double time_thomas_full_f32_sec;
    double mem_thomas_full_f32_kb; // Dynamic peak memory
    double err_thomas_full_f64;
    double time_thomas_full_f64_sec;
    double mem_thomas_full_f64_kb; // Dynamic peak memory

} ExperimentResult;

// ----- Utility Macros -----
#define CHECK_ALLOC(ptr, msg) \
do { \
if (!(ptr)) { \
fprintf(stderr, "ERROR: Memory allocation failed for %s in %s at line %d.\n", msg, __FILE__, __LINE__); \
/* mem_tracker_print_stats("CHECK_ALLOC_FAIL"); // Opcjonalnie, jeśli chcesz zobaczyć stan trackera */ \
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