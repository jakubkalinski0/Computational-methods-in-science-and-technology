#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "common.h"
#include "matrix_utils.h" // For Matrix, Vector types

// Calculates the L1 condition number of a matrix.
double calculate_condition_number_l1(const Matrix *A_orig, const char* precision_dtype);

// Calculates the maximum absolute error between two vectors.
double calculate_max_abs_error(const Vector *v_computed, const Vector *v_true, const char* precision_dtype);

// Runs experiments for a given matrix type, for a list of sizes, for both float and double.
// Stores results in results_float and results_double arrays.
// Caller is responsible for allocating these arrays.
void run_experiments_for_matrix(
    const char* matrix_name,
    void (*generate_matrix_func)(Matrix*, int, const char*),
    const int* sizes,
    int num_sizes,
    ExperimentResult* results_float,
    ExperimentResult* results_double
);

#endif // EXPERIMENT_H