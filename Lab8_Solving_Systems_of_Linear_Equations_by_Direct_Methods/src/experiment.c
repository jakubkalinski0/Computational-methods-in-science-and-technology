#include "experiment.h"
#include "gaussian_elim.h" // For solve_system_spp, invert_matrix_spp
#include <stdio.h>
#include <time.h>
#include <float.h>  // For DBL_MAX, FLT_MAX etc.
#include <string.h> // For strcmp


// Helper for casting based on precision_dtype string
static double cast_val_exp(double val, const char* precision_dtype) {
    if (strcmp(precision_dtype, "float") == 0) {
        return (float)val;
    }
    return val; // double
}


double calculate_condition_number_l1(const Matrix *A_orig, const char* precision_dtype) {
    int n = A_orig->rows;
    Matrix *A_inv = create_matrix(n, n);
    Matrix *A_precise_for_norm = copy_matrix(A_orig); // Copy for norm calculation at target precision
    for(int r=0; r<n; ++r) for(int c=0; c<n; ++c) A_precise_for_norm->data[r][c] = cast_val_exp(A_precise_for_norm->data[r][c], precision_dtype);

    double norm_A = matrix_norm_1(A_precise_for_norm, precision_dtype);
    free_matrix(A_precise_for_norm);

    if (isinf(norm_A) || norm_A == 0.0) { // If norm_A is Inf or 0, cond num is Inf or undefined (treat as Inf)
        free_matrix(A_inv);
        return INFINITY;
    }

    // invert_matrix_spp will internally handle precision for A_orig copy and A_inv
    bool inverted_ok = invert_matrix_spp(A_orig, A_inv, precision_dtype); 
                                                                          
    if (!inverted_ok) { // Should be rare, if lu_factorize fails very early
        free_matrix(A_inv);
        return INFINITY;
    }

    // A_inv is already at target_precision from invert_matrix_spp
    double norm_A_inv = matrix_norm_1(A_inv, precision_dtype);
    free_matrix(A_inv);

    if (isinf(norm_A_inv) || norm_A_inv == 0.0) {
        return INFINITY;
    }
    
    double cond_num = cast_val_exp(norm_A * norm_A_inv, precision_dtype);
    return isfinite(cond_num) ? cond_num : INFINITY;
}

double calculate_max_abs_error(const Vector *v_computed, const Vector *v_true, const char* precision_dtype) {
    if (v_computed->size != v_true->size) {
        fprintf(stderr, "Error (calc_max_abs_error): Vector sizes mismatch.\n");
        return INFINITY;
    }
    double max_err = 0.0;
    for (int i = 0; i < v_computed->size; i++) {
        // Values in v_computed are already at target precision. v_true is double.
        double val_comp = v_computed->data[i]; // Assumed to be at precision_dtype from solve
        double val_true_at_prec = cast_val_exp(v_true->data[i], precision_dtype); // x_true cast to target precision for comparison

        if (isinf(val_comp) || isnan(val_comp)) return INFINITY; // If computed is inf/nan, error is inf

        double err = fabs(val_comp - val_true_at_prec);
        err = cast_val_exp(err, precision_dtype); // Error itself considered at current precision

        if (err > max_err) {
            max_err = err;
        }
    }
    return isfinite(max_err) ? max_err : INFINITY;
}


static void run_single_precision_experiment(
    void (*generate_matrix_func)(Matrix*, int, const char*),
    int size,
    const char* precision_dtype_str, // "float" or "double"
    ExperimentResult* result) {

    result->size = size;
    clock_t start_time, end_time;

    Matrix *A = create_matrix(size, size);
    Vector *x_true = create_vector(size); // x_true always generated as double
    Vector *b = create_vector(size);
    Vector *x_computed = create_vector(size);

    generate_matrix_func(A, size, precision_dtype_str); // A is now at target precision
    generate_x_true(x_true, size, size);             // x_true is double
    
    // b is calculated using A (at target precision) and x_true (double), result cast to target precision
    matrix_vector_mult(A, x_true, b, precision_dtype_str); 

    // --- Solve System ---
    start_time = clock();
    bool solved_ok = solve_system_spp(A, b, x_computed, precision_dtype_str); // x_computed will be at target precision
    end_time = clock();
    result->time_solve_sec = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    if (!solved_ok) { // Early failure in solve_system_spp (very rare)
        result->max_abs_error = INFINITY;
    } else {
        result->max_abs_error = calculate_max_abs_error(x_computed, x_true, precision_dtype_str);
    }
    // Ensure Inf if any intermediate was Inf/NaN
    if (!isfinite(result->max_abs_error)) result->max_abs_error = INFINITY;


    // --- Calculate Condition Number ---
    start_time = clock();
    // A is already at target precision from generate_matrix_func
    result->condition_number = calculate_condition_number_l1(A, precision_dtype_str);
    end_time = clock();
    result->time_cond_sec = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    if (!isfinite(result->condition_number)) result->condition_number = INFINITY;


    free_matrix(A);
    free_vector(x_true);
    free_vector(b);
    free_vector(x_computed);
}


void run_experiments_for_matrix(
    const char* matrix_name,
    void (*generate_matrix_func)(Matrix*, int, const char*),
    const int* sizes,
    int num_sizes,
    ExperimentResult* results_float,
    ExperimentResult* results_double) {

    printf("----- Running experiments for Matrix: %s -----\n", matrix_name);

    printf("  --- Precision: float ---\n");
    for (int i = 0; i < num_sizes; i++) {
        int current_size = sizes[i];
         if (i % 10 == 0 || i == num_sizes - 1 || current_size <= 20 ) { // Print more often for smaller sizes or start/end
            printf("    Size: %d (%d/%d)\n", current_size, i + 1, num_sizes);
        }
        run_single_precision_experiment(generate_matrix_func, current_size, "float", &results_float[i]);
        if (i % 10 == 0 || i == num_sizes - 1 || current_size <= 20) {
             printf("      Float (N=%d): Err: %.3e, Cond: %.3e, T_sol: %.4fs, T_cond: %.4fs\n",
                   results_float[i].size, results_float[i].max_abs_error, results_float[i].condition_number,
                   results_float[i].time_solve_sec, results_float[i].time_cond_sec);
        }
    }

    printf("  --- Precision: double ---\n");
    for (int i = 0; i < num_sizes; i++) {
        int current_size = sizes[i];
        if (i % 10 == 0 || i == num_sizes - 1 || current_size <= 20) {
             printf("    Size: %d (%d/%d)\n", current_size, i + 1, num_sizes);
        }
        run_single_precision_experiment(generate_matrix_func, current_size, "double", &results_double[i]);
        if (i % 10 == 0 || i == num_sizes - 1 || current_size <= 20) {
            printf("      Double (N=%d): Err: %.3e, Cond: %.3e, T_sol: %.4fs, T_cond: %.4fs\n",
                   results_double[i].size, results_double[i].max_abs_error, results_double[i].condition_number,
                   results_double[i].time_solve_sec, results_double[i].time_cond_sec);
        }
    }
}