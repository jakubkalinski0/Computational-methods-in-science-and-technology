#include "experiment.h"
#include "matrix_utils.h"
#include "gaussian_elim.h"
#include "thomas_algorithm.h"

static void run_single_n_size_experiment(
    int n_size,
    ExperimentResult* result_for_n) {

    result_for_n->size_n = n_size;
    clock_t start_c, end_c;

    Vector *x_true_ref = create_vector(n_size);
    generate_x_true(x_true_ref, n_size);

    const char* prec_f32 = "float";
    Matrix *A_full_f32 = create_matrix(n_size, n_size);
    Vector *b_f32_gauss = create_vector(n_size);
    Vector *x_sol_f32_gauss = create_vector(n_size);
    generate_A_tridiagonal_full(A_full_f32, n_size, M_PARAM, K_PARAM, prec_f32);
    matrix_vector_mult_full(A_full_f32, x_true_ref, b_f32_gauss, prec_f32);
    start_c = clock();
    solve_system_gauss_spp(A_full_f32, b_f32_gauss, x_sol_f32_gauss, prec_f32);
    end_c = clock();
    result_for_n->time_gauss_f32_sec = (double)(end_c - start_c) / CLOCKS_PER_SEC;
    result_for_n->err_gauss_f32 = calculate_max_abs_error(x_sol_f32_gauss, x_true_ref, prec_f32);
    free_matrix(A_full_f32); free_vector(b_f32_gauss); free_vector(x_sol_f32_gauss);

    Matrix *A_band_f32 = create_matrix(n_size, 3);
    Vector *b_f32_thomas = create_vector(n_size);
    Vector *x_sol_f32_thomas = create_vector(n_size);
    generate_A_tridiagonal_banded(A_band_f32, n_size, M_PARAM, K_PARAM, prec_f32);
    matrix_vector_mult_banded(A_band_f32, x_true_ref, b_f32_thomas, prec_f32);
    start_c = clock();
    thomas_solve(A_band_f32, b_f32_thomas, x_sol_f32_thomas, prec_f32);
    end_c = clock();
    result_for_n->time_thomas_f32_sec = (double)(end_c - start_c) / CLOCKS_PER_SEC;
    result_for_n->err_thomas_f32 = calculate_max_abs_error(x_sol_f32_thomas, x_true_ref, prec_f32);
    free_matrix(A_band_f32); free_vector(b_f32_thomas); free_vector(x_sol_f32_thomas);

    const char* prec_f64 = "double";
    Matrix *A_full_f64 = create_matrix(n_size, n_size);
    Vector *b_f64_gauss = create_vector(n_size);
    Vector *x_sol_f64_gauss = create_vector(n_size);
    generate_A_tridiagonal_full(A_full_f64, n_size, M_PARAM, K_PARAM, prec_f64);
    matrix_vector_mult_full(A_full_f64, x_true_ref, b_f64_gauss, prec_f64);
    start_c = clock();
    solve_system_gauss_spp(A_full_f64, b_f64_gauss, x_sol_f64_gauss, prec_f64);
    end_c = clock();
    result_for_n->time_gauss_f64_sec = (double)(end_c - start_c) / CLOCKS_PER_SEC;
    result_for_n->err_gauss_f64 = calculate_max_abs_error(x_sol_f64_gauss, x_true_ref, prec_f64);
    free_matrix(A_full_f64); free_vector(b_f64_gauss); free_vector(x_sol_f64_gauss);

    Matrix *A_band_f64 = create_matrix(n_size, 3);
    Vector *b_f64_thomas = create_vector(n_size);
    Vector *x_sol_f64_thomas = create_vector(n_size);
    generate_A_tridiagonal_banded(A_band_f64, n_size, M_PARAM, K_PARAM, prec_f64);
    matrix_vector_mult_banded(A_band_f64, x_true_ref, b_f64_thomas, prec_f64);
    start_c = clock();
    thomas_solve(A_band_f64, b_f64_thomas, x_sol_f64_thomas, prec_f64);
    end_c = clock();
    result_for_n->time_thomas_f64_sec = (double)(end_c - start_c) / CLOCKS_PER_SEC;
    result_for_n->err_thomas_f64 = calculate_max_abs_error(x_sol_f64_thomas, x_true_ref, prec_f64);
    free_matrix(A_band_f64); free_vector(b_f64_thomas); free_vector(x_sol_f64_thomas);

    result_for_n->mem_gauss_f32_kb = (double)n_size * n_size * sizeof(float) / 1024.0;
    result_for_n->mem_thomas_f32_kb = (double)n_size * 3 * sizeof(float) / 1024.0;
    result_for_n->mem_gauss_f64_kb = (double)n_size * n_size * sizeof(double) / 1024.0;
    result_for_n->mem_thomas_f64_kb = (double)n_size * 3 * sizeof(double) / 1024.0;

    free_vector(x_true_ref);
}


void run_all_experiments(
    const int* n_sizes_arr,
    int num_n_values,
    ExperimentResult* results_array) {

    printf("----- Running Experiments (m=%.1f, k=%.1f) -----\n", M_PARAM, K_PARAM);

    for (int i = 0; i < num_n_values; i++) {
        int current_n = n_sizes_arr[i];
        if (i % 10 == 0 || i == num_n_values - 1 || current_n <= 20 || current_n % 50 == 0) {
            printf("  Processing N = %d (%d/%d)...\n", current_n, i + 1, num_n_values);
        }
        run_single_n_size_experiment(current_n, &results_array[i]);

        if (i % 10 == 0 || i == num_n_values - 1 || current_n <= 20 || current_n % 50 == 0) {
             ExperimentResult* res = &results_array[i];
             printf("    N=%d: Gauss_f32(err:%.2e, t:%.3fs), Thomas_f32(err:%.2e, t:%.3fs)\n",
                    current_n, res->err_gauss_f32, res->time_gauss_f32_sec, res->err_thomas_f32, res->time_thomas_f32_sec);
             printf("           Gauss_f64(err:%.2e, t:%.3fs), Thomas_f64(err:%.2e, t:%.3fs)\n",
                    res->err_gauss_f64, res->time_gauss_f64_sec, res->err_thomas_f64, res->time_thomas_f64_sec);
        }
    }
    printf("----- Experiments Finished -----\n");
}