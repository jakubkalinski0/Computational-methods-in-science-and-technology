#include "experiment.h"
#include "matrix_utils.h"
#include "gaussian_elim.h"
#include "thomas_algorithm.h"
// memory_tracker.h jest już dołączony przez common.h, a common.h przez experiment.h

// Helper function to convert a full tridiagonal matrix to banded format
static void convert_full_to_banded(const Matrix* A_full, Matrix* A_banded, int n, const char* precision_dtype) {
    for (int i = 0; i < n; i++) {
        if (i > 0) {
            A_banded->data[i][0] = cast_to_prec(A_full->data[i][i-1], precision_dtype);
        } else {
            A_banded->data[i][0] = cast_to_prec(0.0, precision_dtype);
        }
        A_banded->data[i][1] = cast_to_prec(A_full->data[i][i], precision_dtype);
        if (i < n - 1) {
            A_banded->data[i][2] = cast_to_prec(A_full->data[i][i+1], precision_dtype);
        } else {
            A_banded->data[i][2] = cast_to_prec(0.0, precision_dtype);
        }
    }
}


static void run_single_n_size_experiment(
    int n_size,
    ExperimentResult* result_for_n) {

    result_for_n->size_n = n_size;
    clock_t start_c, end_c;

    const char* prec_f32 = "float";
    const char* prec_f64 = "double";

    // Zapisz stan liczników pamięci PRZED operacjami dla tego N.
    // To pozwoli sprawdzić, czy run_single_n_size_experiment samo w sobie nie ma wycieków
    // i czy poprawnie zwalnia całą pamięć, którą alokuje.
    size_t initial_current_usage_for_this_N = g_current_memory_usage;
    size_t initial_total_allocs_for_this_N = g_total_allocations;
    size_t initial_total_deallocs_for_this_N = g_total_deallocations;

    // Alokacja x_true_ref. Peak jest resetowany przed każdym blokiem metody.
    Vector *x_true_ref = create_vector(n_size);
    generate_x_true(x_true_ref, n_size);


    // --- Gaussian Elimination (f32) ---
    mem_tracker_reset_peak(); // Peak dla tej metody, od obecnego stanu pamięci.
    Matrix *A_full_f32_gauss = create_matrix(n_size, n_size);
    Vector *b_f32_gauss = create_vector(n_size);
    Vector *x_sol_f32_gauss = create_vector(n_size);
    generate_A_tridiagonal_full(A_full_f32_gauss, n_size, M_PARAM, K_PARAM, prec_f32);
    matrix_vector_mult_full(A_full_f32_gauss, x_true_ref, b_f32_gauss, prec_f32);

    start_c = clock();
    solve_system_gauss_spp(A_full_f32_gauss, b_f32_gauss, x_sol_f32_gauss, prec_f32);
    end_c = clock();
    result_for_n->time_gauss_f32_sec = (double)(end_c - start_c) / CLOCKS_PER_SEC;
    result_for_n->err_gauss_f32 = calculate_max_abs_error(x_sol_f32_gauss, x_true_ref, prec_f32);
    result_for_n->mem_gauss_f32_kb = (double)g_peak_memory_usage / 1024.0;

    free_matrix(A_full_f32_gauss); free_vector(b_f32_gauss); free_vector(x_sol_f32_gauss);

    // --- Thomas Algorithm (Banded Storage, f32) ---
    mem_tracker_reset_peak();
    Matrix *A_band_f32_thomas = create_matrix(n_size, 3);
    Vector *b_f32_thomas_banded = create_vector(n_size);
    Vector *x_sol_f32_thomas_banded = create_vector(n_size);
    generate_A_tridiagonal_banded(A_band_f32_thomas, n_size, M_PARAM, K_PARAM, prec_f32);
    matrix_vector_mult_banded(A_band_f32_thomas, x_true_ref, b_f32_thomas_banded, prec_f32);

    start_c = clock();
    thomas_solve(A_band_f32_thomas, b_f32_thomas_banded, x_sol_f32_thomas_banded, prec_f32);
    end_c = clock();
    result_for_n->time_thomas_banded_f32_sec = (double)(end_c - start_c) / CLOCKS_PER_SEC;
    result_for_n->err_thomas_banded_f32 = calculate_max_abs_error(x_sol_f32_thomas_banded, x_true_ref, prec_f32);
    result_for_n->mem_thomas_banded_f32_kb = (double)g_peak_memory_usage / 1024.0;

    free_matrix(A_band_f32_thomas); free_vector(b_f32_thomas_banded); free_vector(x_sol_f32_thomas_banded);

    // --- Thomas Algorithm (Full Storage, then conversion, f32) ---
    mem_tracker_reset_peak();
    Matrix *A_full_f32_thomas_full = create_matrix(n_size, n_size);
    Matrix *A_band_f32_thomas_conv = create_matrix(n_size, 3);
    Vector *b_f32_thomas_full = create_vector(n_size);
    Vector *x_sol_f32_thomas_full = create_vector(n_size);
    generate_A_tridiagonal_full(A_full_f32_thomas_full, n_size, M_PARAM, K_PARAM, prec_f32);
    matrix_vector_mult_full(A_full_f32_thomas_full, x_true_ref, b_f32_thomas_full, prec_f32);

    start_c = clock();
    convert_full_to_banded(A_full_f32_thomas_full, A_band_f32_thomas_conv, n_size, prec_f32);
    thomas_solve(A_band_f32_thomas_conv, b_f32_thomas_full, x_sol_f32_thomas_full, prec_f32);
    end_c = clock();
    result_for_n->time_thomas_full_f32_sec = (double)(end_c - start_c) / CLOCKS_PER_SEC;
    result_for_n->err_thomas_full_f32 = calculate_max_abs_error(x_sol_f32_thomas_full, x_true_ref, prec_f32);
    result_for_n->mem_thomas_full_f32_kb = (double)g_peak_memory_usage / 1024.0;

    free_matrix(A_full_f32_thomas_full); free_matrix(A_band_f32_thomas_conv);
    free_vector(b_f32_thomas_full); free_vector(x_sol_f32_thomas_full);


    // --- Gaussian Elimination (f64) ---
    mem_tracker_reset_peak();
    Matrix *A_full_f64_gauss = create_matrix(n_size, n_size);
    Vector *b_f64_gauss = create_vector(n_size);
    Vector *x_sol_f64_gauss = create_vector(n_size);
    generate_A_tridiagonal_full(A_full_f64_gauss, n_size, M_PARAM, K_PARAM, prec_f64);
    matrix_vector_mult_full(A_full_f64_gauss, x_true_ref, b_f64_gauss, prec_f64);

    start_c = clock();
    solve_system_gauss_spp(A_full_f64_gauss, b_f64_gauss, x_sol_f64_gauss, prec_f64);
    end_c = clock();
    result_for_n->time_gauss_f64_sec = (double)(end_c - start_c) / CLOCKS_PER_SEC;
    result_for_n->err_gauss_f64 = calculate_max_abs_error(x_sol_f64_gauss, x_true_ref, prec_f64);
    result_for_n->mem_gauss_f64_kb = (double)g_peak_memory_usage / 1024.0;

    free_matrix(A_full_f64_gauss); free_vector(b_f64_gauss); free_vector(x_sol_f64_gauss);

    // --- Thomas Algorithm (Banded Storage, f64) ---
    mem_tracker_reset_peak();
    Matrix *A_band_f64_thomas = create_matrix(n_size, 3);
    Vector *b_f64_thomas_banded = create_vector(n_size);
    Vector *x_sol_f64_thomas_banded = create_vector(n_size);
    generate_A_tridiagonal_banded(A_band_f64_thomas, n_size, M_PARAM, K_PARAM, prec_f64);
    matrix_vector_mult_banded(A_band_f64_thomas, x_true_ref, b_f64_thomas_banded, prec_f64);

    start_c = clock();
    thomas_solve(A_band_f64_thomas, b_f64_thomas_banded, x_sol_f64_thomas_banded, prec_f64);
    end_c = clock();
    result_for_n->time_thomas_banded_f64_sec = (double)(end_c - start_c) / CLOCKS_PER_SEC;
    result_for_n->err_thomas_banded_f64 = calculate_max_abs_error(x_sol_f64_thomas_banded, x_true_ref, prec_f64);
    result_for_n->mem_thomas_banded_f64_kb = (double)g_peak_memory_usage / 1024.0;

    free_matrix(A_band_f64_thomas); free_vector(b_f64_thomas_banded); free_vector(x_sol_f64_thomas_banded);

    // --- Thomas Algorithm (Full Storage, then conversion, f64) ---
    mem_tracker_reset_peak();
    Matrix *A_full_f64_thomas_full = create_matrix(n_size, n_size);
    Matrix *A_band_f64_thomas_conv = create_matrix(n_size, 3);
    Vector *b_f64_thomas_full = create_vector(n_size);
    Vector *x_sol_f64_thomas_full = create_vector(n_size);
    generate_A_tridiagonal_full(A_full_f64_thomas_full, n_size, M_PARAM, K_PARAM, prec_f64);
    matrix_vector_mult_full(A_full_f64_thomas_full, x_true_ref, b_f64_thomas_full, prec_f64);

    start_c = clock();
    convert_full_to_banded(A_full_f64_thomas_full, A_band_f64_thomas_conv, n_size, prec_f64);
    thomas_solve(A_band_f64_thomas_conv, b_f64_thomas_full, x_sol_f64_thomas_full, prec_f64);
    end_c = clock();
    result_for_n->time_thomas_full_f64_sec = (double)(end_c - start_c) / CLOCKS_PER_SEC;
    result_for_n->err_thomas_full_f64 = calculate_max_abs_error(x_sol_f64_thomas_full, x_true_ref, prec_f64);
    result_for_n->mem_thomas_full_f64_kb = (double)g_peak_memory_usage / 1024.0;

    free_matrix(A_full_f64_thomas_full); free_matrix(A_band_f64_thomas_conv);
    free_vector(b_f64_thomas_full); free_vector(x_sol_f64_thomas_full);

    // --- Cleanup common data for this N ---
    free_vector(x_true_ref);

    // Sprawdzenie, czy wszystkie alokacje W RAMACH TEGO WYWOŁANIA run_single_n_size_experiment zostały zwolnione.
    // g_current_memory_usage powinno wrócić do wartości, jaką miało PRZED tym wywołaniem.
    if (g_current_memory_usage != initial_current_usage_for_this_N) {
        fprintf(stderr, "MEM_LEAK_WARN (N=%d): Memory usage mismatch after single run. Start of N: %zu, End of N: %zu. ",
                n_size, initial_current_usage_for_this_N, g_current_memory_usage);
        if (g_current_memory_usage > initial_current_usage_for_this_N) {
            fprintf(stderr, "Diff: +%zu B (leak)\n", g_current_memory_usage - initial_current_usage_for_this_N);
        } else {
            // To jest sytuacja błędu "Current usage < freed size" wykrytego przez my_free_proxy,
            // gdzie initial_current_usage_for_this_N mogło być już niepoprawne.
            // Lub poprawnie, jeśli np. pamięć była zwolniona przez funkcję wyższego rzędu.
            // Ale w tym kontekście oznacza to, że zwolniliśmy więcej niż zaalokowaliśmy *w tym zakresie*.
            fprintf(stderr, "Diff: -%zu B (over-freed or error in tracker)\n", initial_current_usage_for_this_N - g_current_memory_usage);
        }
        fprintf(stderr, "  Allocs in this N run: %zu, Deallocs in this N run: %zu\n",
                g_total_allocations - initial_total_allocs_for_this_N,
                g_total_deallocations - initial_total_deallocs_for_this_N);
    }
}


void run_all_experiments(
    const int* n_sizes_arr,
    int num_n_values,
    ExperimentResult* results_array) {

    // mem_tracker_init(); // Inicjalizacja jest teraz tylko raz w main()
    printf("----- Running Experiments (m=%.1f, k=%.1f) -----\n", M_PARAM, K_PARAM);

    for (int i = 0; i < num_n_values; i++) {
        int current_n = n_sizes_arr[i];

        // Logika wyświetlania postępu
        bool should_print_progress = (i == 0) || (i == num_n_values - 1) ||
                                     (current_n <= 20 && i > 0 && (i+1) % 2 == 0) || // Co drugi dla małych N <= 20 (pomijając i=0)
                                     (current_n > 20 && current_n <= 100 && i > 0 && (i+1) % 10 == 0) || // Co 10 dla N (20, 100]
                                     (current_n > 100 && i > 0 && (i+1) % 20 == 0); // Co 20 dla N > 100

        if (i == 0 || should_print_progress) { // Zawsze drukuj dla i=0
             printf("  Processing N = %d (%d/%d)...\n", current_n, i + 1, num_n_values);
        }

        run_single_n_size_experiment(current_n, &results_array[i]);

        if (i == 0 || should_print_progress) { // Zawsze drukuj dla i=0
             ExperimentResult* res = &results_array[i];
             printf("    N=%d:\n", current_n);
             printf("      Gauss_f32:  err:%.2e, t:%.3fs, mem:%.2fKB\n",
                    res->err_gauss_f32, res->time_gauss_f32_sec, res->mem_gauss_f32_kb);
             printf("      Gauss_f64:  err:%.2e, t:%.3fs, mem:%.2fKB\n",
                    res->err_gauss_f64, res->time_gauss_f64_sec, res->mem_gauss_f64_kb);
             printf("      ThomasB_f32:err:%.2e, t:%.3fs, mem:%.2fKB\n",
                    res->err_thomas_banded_f32, res->time_thomas_banded_f32_sec, res->mem_thomas_banded_f32_kb);
             printf("      ThomasB_f64:err:%.2e, t:%.3fs, mem:%.2fKB\n",
                    res->err_thomas_banded_f64, res->time_thomas_banded_f64_sec, res->mem_thomas_banded_f64_kb);
             printf("      ThomasF_f32:err:%.2e, t:%.3fs, mem:%.2fKB\n",
                    res->err_thomas_full_f32, res->time_thomas_full_f32_sec, res->mem_thomas_full_f32_kb);
             printf("      ThomasF_f64:err:%.2e, t:%.3fs, mem:%.2fKB\n",
                    res->err_thomas_full_f64, res->time_thomas_full_f64_sec, res->mem_thomas_full_f64_kb);
        }
    }
    printf("----- Experiments Finished -----\n");
}