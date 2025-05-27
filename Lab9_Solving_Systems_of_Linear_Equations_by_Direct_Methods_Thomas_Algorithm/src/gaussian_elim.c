#include "gaussian_elim.h"
#include "matrix_utils.h"

// Internal helper: LU Factorization with SPP
static bool lu_factorize_spp_internal(Matrix *A_lu, int *pivot_indices, double *scale_factors, const char* precision_dtype) {
    int n = A_lu->rows;
    double current_eps_factor = 1.0;
    double machine_eps = get_prec_epsilon(precision_dtype);

    for (int i = 0; i < n; i++) {
        pivot_indices[i] = i;
        double s_max_row = 0.0;
        for (int j = 0; j < n; j++) {
            double val_abs = fabs(cast_to_prec(A_lu->data[i][j], precision_dtype));
            if (val_abs > s_max_row) s_max_row = val_abs;
        }
        scale_factors[i] = s_max_row;
    }

    for (int k = 0; k < n - 1; k++) {
        double r_max_scaled = 0.0;
        int pivot_row_k_map_idx = k;

        for (int i_map_idx = k; i_map_idx < n; i_map_idx++) {
            int actual_row_idx = pivot_indices[i_map_idx];
            if (scale_factors[actual_row_idx] < machine_eps * DBL_MIN) continue;

            double ratio = fabs(cast_to_prec(A_lu->data[actual_row_idx][k], precision_dtype)) / scale_factors[actual_row_idx];
            ratio = cast_to_prec(ratio, precision_dtype);

            if (ratio > r_max_scaled) {
                r_max_scaled = ratio;
                pivot_row_k_map_idx = i_map_idx;
            }
        }

        int temp_pivot_val = pivot_indices[k];
        pivot_indices[k] = pivot_indices[pivot_row_k_map_idx];
        pivot_indices[pivot_row_k_map_idx] = temp_pivot_val;

        int actual_pivot_row = pivot_indices[k];
        double pivot_element_val = cast_to_prec(A_lu->data[actual_pivot_row][k], precision_dtype);

        if (fabs(pivot_element_val) < machine_eps * current_eps_factor * scale_factors[actual_pivot_row]) {
             if (fabs(pivot_element_val) < machine_eps * DBL_MIN && scale_factors[actual_pivot_row] > machine_eps * DBL_MIN) {
                // fprintf(stderr, "GAUSS_WARN (%s): Singular or near-singular matrix detected at k=%d. Pivot %e, Scale %e\n", precision_dtype, k, pivot_element_val, scale_factors[actual_pivot_row] );
             }
        }

        for (int i_map_idx = k + 1; i_map_idx < n; i_map_idx++) {
            int current_elim_row = pivot_indices[i_map_idx];
            double multiplier;
            if (fabs(pivot_element_val) < machine_eps * DBL_MIN) {
                 multiplier = (A_lu->data[current_elim_row][k] == 0.0) ? 0.0 : copysign(INFINITY, A_lu->data[current_elim_row][k]/ (pivot_element_val != 0 ? pivot_element_val : copysign(machine_eps*DBL_MIN, pivot_element_val) ) );
            } else {
                multiplier = cast_to_prec(A_lu->data[current_elim_row][k], precision_dtype) / pivot_element_val;
            }
            multiplier = cast_to_prec(multiplier, precision_dtype);
            A_lu->data[current_elim_row][k] = multiplier;

            for (int j = k + 1; j < n; j++) {
                double term = cast_to_prec(multiplier * cast_to_prec(A_lu->data[actual_pivot_row][j], precision_dtype), precision_dtype);
                A_lu->data[current_elim_row][j] = cast_to_prec(cast_to_prec(A_lu->data[current_elim_row][j], precision_dtype) - term, precision_dtype);
            }
        }
    }
    if (n > 0 && fabs(cast_to_prec(A_lu->data[pivot_indices[n-1]][n-1], precision_dtype)) < machine_eps * DBL_MIN) {
        // fprintf(stderr, "GAUSS_WARN (%s): Last pivot element is near zero. Matrix likely singular.\n", precision_dtype);
    }
    return true;
}

static void lu_solve_permuted_internal(const Matrix *A_lu, const int *pivot_indices, const Vector *b_permuted_rhs, Vector *x_sol, const char* precision_dtype) {
    int n = A_lu->rows;
    Vector *y_prime = create_vector(n);

    for (int i = 0; i < n; i++) {
        double sum_val = 0.0;
        for (int j = 0; j < i; j++) {
            double term = cast_to_prec(cast_to_prec(A_lu->data[pivot_indices[i]][j], precision_dtype) * y_prime->data[j], precision_dtype);
            sum_val = cast_to_prec(sum_val + term, precision_dtype);
        }
        y_prime->data[i] = cast_to_prec(cast_to_prec(b_permuted_rhs->data[pivot_indices[i]], precision_dtype) - sum_val, precision_dtype);
    }

    for (int i = n - 1; i >= 0; i--) {
        double sum_val = 0.0;
        for (int j = i + 1; j < n; j++) {
            double term = cast_to_prec(cast_to_prec(A_lu->data[pivot_indices[i]][j], precision_dtype) * x_sol->data[j], precision_dtype);
            sum_val = cast_to_prec(sum_val + term, precision_dtype);
        }
        double U_ii = cast_to_prec(A_lu->data[pivot_indices[i]][i], precision_dtype);
        if (fabs(U_ii) < get_prec_epsilon(precision_dtype) * DBL_MIN) {
            x_sol->data[i] = (fabs(cast_to_prec(y_prime->data[i] - sum_val, precision_dtype)) < get_prec_epsilon(precision_dtype) * DBL_MIN) ?
                             NAN :
                             copysign(INFINITY, cast_to_prec(y_prime->data[i] - sum_val, precision_dtype));
        } else {
             x_sol->data[i] = cast_to_prec((cast_to_prec(y_prime->data[i] - sum_val, precision_dtype)) / U_ii, precision_dtype);
        }
    }
    free_vector(y_prime);
}


bool solve_system_gauss_spp(const Matrix *A_orig, const Vector *b_orig, Vector *x_sol, const char* precision_dtype) {
    int n = A_orig->rows;
    Matrix *A_lu = copy_matrix(A_orig);
    int *pivot_indices = (int*)malloc(n * sizeof(int));
    CHECK_ALLOC(pivot_indices, "pivot_indices in solve_system_spp");
    double *scale_factors = (double*)malloc(n * sizeof(double));
    CHECK_ALLOC(scale_factors, "scale_factors in solve_system_spp");

    for(int r=0; r<n; ++r) {
        for(int c=0; c<n; ++c) {
            A_lu->data[r][c] = cast_to_prec(A_lu->data[r][c], precision_dtype);
        }
    }
    Vector* b_precise = create_vector(n);
    for(int i=0; i<n; ++i) b_precise->data[i] = cast_to_prec(b_orig->data[i], precision_dtype);

    lu_factorize_spp_internal(A_lu, pivot_indices, scale_factors, precision_dtype);
    lu_solve_permuted_internal(A_lu, pivot_indices, b_precise, x_sol, precision_dtype);

    free_matrix(A_lu);
    free(pivot_indices);
    free(scale_factors);
    free_vector(b_precise);

    for(int i=0; i<n; ++i) {
        if (isnan(x_sol->data[i]) || isinf(x_sol->data[i])) {
            return true;
        }
    }
    return true;
}