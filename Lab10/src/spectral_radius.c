#include "../include/spectral_radius.h"
#include "../include/matrix_operations.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double** create_jacobi_iteration_matrix_T(double** A, int n) {
    if (n <= 0 || !A) {
        fprintf(stderr, "Error [create_jacobi_iteration_matrix_T]: Invalid input matrix A or size n.\n");
        return NULL;
    }

    double** T_J = create_matrix(n, n);
    if (!T_J) {
        fprintf(stderr, "Error: Memory allocation failed for T_J matrix in create_jacobi_iteration_matrix_T.\n");
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        if (fabs(A[i][i]) < 1e-12) {
            fprintf(stderr, "Error [create_jacobi_iteration_matrix_T]: Diagonal element A[%d][%d] (%.2e) is near zero.\n", i, i, A[i][i]);
            free_matrix(T_J, n);
            return NULL;
        }
        for (int j = 0; j < n; j++) {
            if (i == j) {
                T_J[i][j] = 0.0;
            } else {
                T_J[i][j] = -A[i][j] / A[i][i];
            }
        }
    }
    return T_J;
}

double estimate_spectral_radius_power_iteration(double** T_J_matrix, int n,
                                                int max_iter, double tolerance,
                                                int* iterations_taken_ptr) {
    if (n <= 0 || !T_J_matrix) {
        if (iterations_taken_ptr) *iterations_taken_ptr = 0;
        fprintf(stderr, "Error [estimate_spectral_radius]: Invalid input matrix T_J_matrix or size n.\n");
        return NAN;
    }

    double* v_current = create_vector(n);
    double* v_next = create_vector(n);

    if (!v_current || !v_next) {
        fprintf(stderr, "Error: Memory allocation failed for vectors in power iteration.\n");
        free_vector(v_current);
        free_vector(v_next);
        if (iterations_taken_ptr) *iterations_taken_ptr = 0;
        return NAN;
    }

    #ifdef TRUE_X_SEED // Użyj stałego seeda dla powtarzalności, jeśli TRUE_X_SEED jest zdefiniowane
        srand(TRUE_X_SEED + 123);
    #else // W przeciwnym razie, użyj dynamicznego seeda
        srand(time(NULL));
    #endif

    for (int i = 0; i < n; i++) {
         v_current[i] = (double)rand() / (double)RAND_MAX;
         if (n==1 && v_current[i] == 0.0) v_current[i] = 1.0; // Uniknij wektora zerowego dla n=1
    }
    double norm_v = vector_norm_l2(v_current, n);
    if (fabs(norm_v) < 1e-12) {
        if (n > 0) v_current[0] = 1.0;
        norm_v = 1.0;
        if (n == 0) norm_v = 0.0;
    }
    if (norm_v > 1e-12) {
        for (int i = 0; i < n; i++) {
            v_current[i] /= norm_v;
        }
    }

    double lambda_new = 0.0;
    double lambda_old = 0.0;
    int iter_count = 0;

    for (iter_count = 0; iter_count < max_iter; iter_count++) {
        matrix_vector_multiply(v_next, T_J_matrix, v_current, n, n);

        lambda_new = vector_norm_l2(v_next, n);

        norm_v = vector_norm_l2(v_next, n);
        if (fabs(norm_v) < 1e-12) {
            lambda_new = 0.0;
            break;
        }
        for (int i = 0; i < n; i++) {
            v_current[i] = v_next[i] / norm_v;
        }

        if (iter_count > 0 && fabs(lambda_new - lambda_old) < tolerance) {
            break;
        }
        lambda_old = lambda_new;
    }

    if (iterations_taken_ptr) {
        *iterations_taken_ptr = (iter_count < max_iter) ? (iter_count + 1) : max_iter;
         if (max_iter == 0) *iterations_taken_ptr = 0;
    }

    free_vector(v_current);
    free_vector(v_next);

    return lambda_new;
}