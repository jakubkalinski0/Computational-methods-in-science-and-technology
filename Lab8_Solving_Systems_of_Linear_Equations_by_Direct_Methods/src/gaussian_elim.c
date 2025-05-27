#include "gaussian_elim.h"
#include "matrix_utils.h" // For copy_matrix
#include <float.h>    // For DBL_EPSILON, FLT_EPSILON
#include <string.h>   // For strcmp


// Helper function to get precision-specific epsilon
static double get_epsilon(const char* precision_dtype) {
    if (strcmp(precision_dtype, "float") == 0) {
        return FLT_EPSILON;
    }
    return DBL_EPSILON;
}

// Helper for casting based on precision_dtype string
static double cast_val(double val, const char* precision_dtype) {
    if (strcmp(precision_dtype, "float") == 0) {
        return (float)val;
    }
    return val; // double
}


bool lu_factorize_spp(Matrix *A_lu, int *pivot_indices, double *scale_factors, const char* precision_dtype) {
    int n = A_lu->rows;
    double current_eps = get_epsilon(precision_dtype);

    // Initialize pivot_indices and scale_factors
    for (int i = 0; i < n; i++) {
        pivot_indices[i] = i;
        double s_max_row = 0.0;
        for (int j = 0; j < n; j++) {
            double val_abs = fabs(cast_val(A_lu->data[i][j], precision_dtype));
            if (val_abs > s_max_row) {
                s_max_row = val_abs;
            }
        }
        scale_factors[i] = s_max_row;
        if (s_max_row < current_eps * DBL_MIN ) { // Check against a very small number if scale factor is near zero
            // This row is effectively zero, or matrix is ill-conditioned
            // Mark as singular for practical purposes if it's too small.
            // Let it proceed, potential division by zero will be handled.
            // fprintf(stderr, "Warning (lu_factorize_spp %s): Row %d has zero or very small scale factor %e. Matrix might be singular.\n", precision_dtype, i, s_max_row);
             if (s_max_row == 0.0) {
                 // A row of all zeros implies singularity unless it's a 1x1 [0] matrix.
                 // For this lab, this generally means issues.
             }
        }
    }

    for (int k = 0; k < n - 1; k++) { // For each column k (pivot column)
        double r_max_scaled = 0.0;
        int pivot_row_k = k; // Index in pivot_indices array, not actual row index yet

        // Find pivot row
        for (int i = k; i < n; i++) {
            int actual_row_idx = pivot_indices[i];
            if (scale_factors[actual_row_idx] == 0.0) continue; // Skip if scale factor is zero (should be rare after init)
            
            double ratio = fabs(cast_val(A_lu->data[actual_row_idx][k], precision_dtype)) / scale_factors[actual_row_idx];
             ratio = cast_val(ratio, precision_dtype); // Ratio considered at current precision

            if (ratio > r_max_scaled) {
                r_max_scaled = ratio;
                pivot_row_k = i; // This is the index in pivot_indices we want to swap with k
            }
        }

        // Swap pivot_indices[k] and pivot_indices[pivot_row_k]
        int temp_pivot_idx = pivot_indices[k];
        pivot_indices[k] = pivot_indices[pivot_row_k];
        pivot_indices[pivot_row_k] = temp_pivot_idx;

        int actual_pivot_row = pivot_indices[k]; // The true row index for the pivot

        // Check for singularity (pivot element too small)
        double pivot_element_val = cast_val(A_lu->data[actual_pivot_row][k], precision_dtype);
        if (fabs(pivot_element_val) < current_eps * scale_factors[actual_pivot_row]) {
           // fprintf(stderr, "Warning (lu_factorize_spp %s): Pivot element A[%d][%d]=%e is small relative to scale_factor[%d]=%e at step k=%d. May lead to instability.\n",
           //         precision_dtype, actual_pivot_row, k, pivot_element_val, actual_pivot_row, scale_factors[actual_pivot_row], k);
            if (fabs(pivot_element_val) < current_eps * DBL_MIN) { // Extremely small pivot
                // This matrix is singular or numerically singular for this precision
                // Continue, but expect Inf/NaN results later
            }
        }
        if (pivot_element_val == 0.0) { // If pivot is exactly zero, it's singular
            // Allow division by zero to produce inf/nan for multipliers
            // fprintf(stderr, "Error (lu_factorize_spp %s): Zero pivot element at A[%d][%d]. Matrix singular.\n", precision_dtype, actual_pivot_row, k);
            // return false; // Or let it produce inf/nan
        }


        // Elimination step
        for (int i = k + 1; i < n; i++) {
            int current_elim_row = pivot_indices[i];
            double multiplier;
            if (pivot_element_val == 0.0) { // Explicit check to form inf/nan correctly
                 multiplier = (A_lu->data[current_elim_row][k] == 0.0) ? 0.0 : copysign(INFINITY, A_lu->data[current_elim_row][k]/pivot_element_val);
            } else {
                multiplier = cast_val(A_lu->data[current_elim_row][k], precision_dtype) / pivot_element_val;
            }
            multiplier = cast_val(multiplier, precision_dtype);

            A_lu->data[current_elim_row][k] = multiplier; // Store multiplier in L part

            for (int j = k + 1; j < n; j++) {
                double term = cast_val(multiplier * cast_val(A_lu->data[actual_pivot_row][j], precision_dtype), precision_dtype);
                A_lu->data[current_elim_row][j] = cast_val(cast_val(A_lu->data[current_elim_row][j], precision_dtype) - term, precision_dtype);
            }
        }
    }
    // Check the last diagonal element of U
    if (fabs(cast_val(A_lu->data[pivot_indices[n-1]][n-1], precision_dtype)) < current_eps * DBL_MIN) {
        // fprintf(stderr, "Warning (lu_factorize_spp %s): Last diagonal element A_lu[%d][%d]=%e is near zero. Matrix may be singular.\n", 
        //         precision_dtype, pivot_indices[n-1], n-1, A_lu->data[pivot_indices[n-1]][n-1]);
        // This could be an indicator of singularity.
    }

    return true; // Factorization "completed", quality depends on pivots
}


void lu_solve_permuted(const Matrix *A_lu, const int *pivot_indices, const Vector *b, Vector *x_sol, const char* precision_dtype) {
    int n = A_lu->rows;
    Vector *y_prime = create_vector(n);

    // Forward substitution: Ly' = Pb
    for (int i = 0; i < n; i++) {
        double sum_val = 0.0;
        for (int j = 0; j < i; j++) {
            double term = cast_val(cast_val(A_lu->data[pivot_indices[i]][j], precision_dtype) * y_prime->data[j], precision_dtype);
            sum_val = cast_val(sum_val + term, precision_dtype);
        }
        y_prime->data[i] = cast_val(cast_val(b->data[pivot_indices[i]], precision_dtype) - sum_val, precision_dtype);
    }

    // Backward substitution: Ux = y'
    for (int i = n - 1; i >= 0; i--) {
        double sum_val = 0.0;
        for (int j = i + 1; j < n; j++) {
             double term = cast_val(cast_val(A_lu->data[pivot_indices[i]][j], precision_dtype) * x_sol->data[j], precision_dtype);
            sum_val = cast_val(sum_val + term, precision_dtype);
        }
        double U_ii = cast_val(A_lu->data[pivot_indices[i]][i], precision_dtype);
        if (U_ii == 0.0) {
            x_sol->data[i] = (y_prime->data[i] - sum_val == 0.0) ? NAN : copysign(INFINITY, y_prime->data[i] - sum_val);
        } else {
             x_sol->data[i] = cast_val((cast_val(y_prime->data[i] - sum_val, precision_dtype)) / U_ii, precision_dtype);
        }
    }
    free_vector(y_prime);
}

bool solve_system_spp(const Matrix *A_orig, const Vector *b_orig, Vector *x_sol, const char* precision_dtype) {
    int n = A_orig->rows;
    Matrix *A_lu = copy_matrix(A_orig); // Work on a copy
    int *pivot_indices = (int*)malloc(n * sizeof(int));
    CHECK_ALLOC(pivot_indices, "pivot_indices in solve_system_spp");
    double *scale_factors = (double*)malloc(n * sizeof(double));
    CHECK_ALLOC(scale_factors, "scale_factors in solve_system_spp");

    // Apply precision to A_lu before factorization
    for(int r=0; r<n; ++r) {
        for(int c=0; c<n; ++c) {
            A_lu->data[r][c] = cast_val(A_lu->data[r][c], precision_dtype);
        }
    }
    Vector* b_precise = create_vector(n);
    for(int i=0; i<n; ++i) b_precise->data[i] = cast_val(b_orig->data[i], precision_dtype);


    bool factorized_ok = lu_factorize_spp(A_lu, pivot_indices, scale_factors, precision_dtype);
    
    if (!factorized_ok) { // This currently only fails if a very early check in factorize triggers false.
                          // Most "singularities" will proceed and result in Inf/NaN in x_sol.
        free_matrix(A_lu);
        free(pivot_indices);
        free(scale_factors);
        free_vector(b_precise);
        for(int i=0; i<n; ++i) x_sol->data[i] = NAN; // Indicate failure
        return false;
    }

    lu_solve_permuted(A_lu, pivot_indices, b_precise, x_sol, precision_dtype);

    free_matrix(A_lu);
    free(pivot_indices);
    free(scale_factors);
    free_vector(b_precise);
    
    // Check if x_sol contains NaN or Inf as an indicator of issues during solve
    for(int i=0; i<n; ++i) {
        if (isnan(x_sol->data[i]) || isinf(x_sol->data[i])) {
            return true; // "Solved" in the sense that the algorithm ran, but solution is not finite.
        }
    }
    return true;
}


bool invert_matrix_spp(const Matrix *A_orig, Matrix *A_inv, const char* precision_dtype) {
    int n = A_orig->rows;
    if (A_inv->rows != n || A_inv->cols != n) {
        fprintf(stderr, "Error (invert_matrix_spp): Output matrix A_inv has incorrect dimensions.\n");
        return false;
    }

    Matrix *A_lu = copy_matrix(A_orig);
    int *pivot_indices = (int*)malloc(n * sizeof(int));
    CHECK_ALLOC(pivot_indices, "pivot_indices in invert_matrix_spp");
    double *scale_factors = (double*)malloc(n * sizeof(double));
    CHECK_ALLOC(scale_factors, "scale_factors in invert_matrix_spp");

    // Apply precision to A_lu before factorization
    for(int r=0; r<n; ++r) {
        for(int c=0; c<n; ++c) {
            A_lu->data[r][c] = cast_val(A_lu->data[r][c], precision_dtype);
        }
    }

    bool factorized_ok = lu_factorize_spp(A_lu, pivot_indices, scale_factors, precision_dtype);
    if (!factorized_ok) { // Should be rare, see comments in lu_factorize_spp
        free_matrix(A_lu);
        free(pivot_indices);
        free(scale_factors);
        for(int r=0; r<n; ++r) for(int c=0; c<n; ++c) A_inv->data[r][c] = NAN;
        return false;
    }

    Vector *e_j = create_vector(n);      // Standard basis vector
    Vector *col_j_inv = create_vector(n); // j-th column of A_inv

    for (int j = 0; j < n; j++) {
        // Create e_j (j-th standard basis vector) at current precision
        for (int i = 0; i < n; i++) {
            e_j->data[i] = cast_val((i == j) ? 1.0 : 0.0, precision_dtype);
        }
        
        lu_solve_permuted(A_lu, pivot_indices, e_j, col_j_inv, precision_dtype);
        
        for (int i = 0; i < n; i++) {
            A_inv->data[i][j] = col_j_inv->data[i]; // Already at correct precision from lu_solve
        }
    }

    free_vector(e_j);
    free_vector(col_j_inv);
    free_matrix(A_lu);
    free(pivot_indices);
    free(scale_factors);
    return true; // Inversion process "completed". A_inv may contain Inf/NaN.
}