#include "../include/common.h"
#include "../include/matrix_operations.h"
#include "../include/problem_definition.h"
#include "../include/jacobi_solver.h"
#include "../include/spectral_radius.h"
#include "../include/file_io.h"

#include <time.h>
#include <string.h>

int main() {
    printf("Jacobi Iterative Solver Laboratory\n");
    printf("Matrix A type 'a', k=%.1f, m=%.1f\n\n", K_PARAM, M_PARAM);

    // --- Define Experiment Parameters ---
    int n_values[] = {50, 100, 200, 300, 500, 1000};
    int num_n_values = sizeof(n_values) / sizeof(n_values[0]);

    double rho_values[] = {1e-4, 1e-8, 1e-12};
    int num_rho_values = sizeof(rho_values) / sizeof(rho_values[0]);

    InitialVectorType x0_types[] = {X0_ZEROS, X0_ALTERNATING_HIGH};
    int num_x0_types = sizeof(x0_types) / sizeof(x0_types[0]);

    StopCriterionType criterion_to_test[] = {INCREMENT_CRITERION, RESIDUAL_CRITERION};
    int num_criterion_to_test = sizeof(criterion_to_test) / sizeof(criterion_to_test[0]);

    // --- Data Collection Structures ---
    int max_results = num_n_values * num_rho_values * num_x0_types * num_criterion_to_test;
    ExperimentData* all_experiment_data = (ExperimentData*)malloc(max_results * sizeof(ExperimentData));
    if (!all_experiment_data) {
        fprintf(stderr, "Failed to allocate memory for experiment data.\n");
        return 1;
    }
    int current_result_idx = 0;

    double* spectral_radii = (double*)malloc(num_n_values * sizeof(double));
    if (!spectral_radii) {
        fprintf(stderr, "Failed to allocate memory for spectral radii.\n");
        free(all_experiment_data);
        return 1;
    }

    // --- Open CSV for detailed results ---
    FILE* csv_fp_results;
    char results_csv_base[] = "jacobi_solver_results_all_criteria";
    open_csv_file(&csv_fp_results, results_csv_base, NULL);
    if (csv_fp_results == NULL) {
        fprintf(stderr, "Exiting due to CSV file opening error for main results.\n");
        free(all_experiment_data);
        free(spectral_radii);
        return 1;
    }

    // --- Task 2: Spectral Radius Calculation (First) ---
    printf("\n--- Calculating Spectral Radii ---\n");
    for (int n_idx = 0; n_idx < num_n_values; n_idx++) {
        int n = n_values[n_idx];
        printf("  N = %d: ", n);
        double** A_for_TJ = create_matrix(n, n);
        if (!A_for_TJ) { printf("Failed to create A_for_TJ for N=%d.\n", n); spectral_radii[n_idx] = NAN; continue; }
        fill_matrix_A(A_for_TJ, n, K_PARAM, M_PARAM);
        double** T_J = create_jacobi_iteration_matrix_T(A_for_TJ, n);
        if (T_J) {
            int power_iters;
            spectral_radii[n_idx] = estimate_spectral_radius_power_iteration(T_J, n, POWER_ITERATION_MAX_ITER, POWER_ITERATION_TOL, &power_iters);
            printf("rho(T_J) = %.6f (in %d power_iters). Convergence expected: %s\n",
                   spectral_radii[n_idx], power_iters,
                   (!isnan(spectral_radii[n_idx]) && spectral_radii[n_idx] < 1.0 && spectral_radii[n_idx] >= 0.0) ? "Yes" : "No/Uncertain");
            free_matrix(T_J, n);
        } else {
            printf("Failed to create T_J for N=%d.\n", n);
            spectral_radii[n_idx] = NAN;
        }
        free_matrix(A_for_TJ, n);
    }
    char spectral_csv_base[] = "spectral_radii_data";
    save_spectral_radius_to_csv(spectral_csv_base, n_values, spectral_radii, num_n_values);
    generate_gnuplot_script_spectral_radius(spectral_csv_base);
    generate_latex_table_spectral_radius("latex_table_spectral_radii", n_values, spectral_radii, num_n_values);

    // --- Task 1: Jacobi Method Experiments ---
    printf("\n--- Running Jacobi Solver Experiments (for all criteria) ---\n");

    for (int n_idx = 0; n_idx < num_n_values; n_idx++) {
        int n = n_values[n_idx];
        printf("Processing N = %d:\n", n);

        double** A = create_matrix(n, n);
        double* x_true = create_vector(n);
        double* b = create_vector(n);
        if (!A || !x_true || !b) {
            fprintf(stderr, "Memory allocation failed for N=%d setup.\n", n);
            // Poprawione zwalnianie pamięci
            if (A) free_matrix(A, n);
            if (x_true) free_vector(x_true);
            if (b) free_vector(b);
            continue;
        }
        fill_matrix_A(A, n, K_PARAM, M_PARAM);
        generate_true_solution_x(x_true, n, TRUE_X_SEED);
        calculate_vector_b(b, A, x_true, n);

        if (isnan(spectral_radii[n_idx]) || spectral_radii[n_idx] >= 1.0 - 1e-9) {
            printf("  Skipping Jacobi iterations for N=%d due to unfavorable spectral radius (%.4f).\n", n, spectral_radii[n_idx]);
            for (int crit_iter_idx_skip = 0; crit_iter_idx_skip < num_criterion_to_test; crit_iter_idx_skip++) {
                for (int x0_idx_skip = 0; x0_idx_skip < num_x0_types; x0_idx_skip++) {
                    for (int rho_idx_skip = 0; rho_idx_skip < num_rho_values; rho_idx_skip++) {
                        if(current_result_idx < max_results) {
                            all_experiment_data[current_result_idx].n = n;
                            all_experiment_data[current_result_idx].x0_type = x0_types[x0_idx_skip];
                            all_experiment_data[current_result_idx].criterion_type = criterion_to_test[crit_iter_idx_skip];
                            all_experiment_data[current_result_idx].rho = rho_values[rho_idx_skip];
                            all_experiment_data[current_result_idx].iterations_taken = -1;
                            all_experiment_data[current_result_idx].time_solve_sec = NAN;
                            all_experiment_data[current_result_idx].time_per_iteration_sec = NAN;
                            all_experiment_data[current_result_idx].final_abs_error = NAN;
                            all_experiment_data[current_result_idx].final_criterion_metric = NAN;
                            append_to_csv_file(csv_fp_results, &all_experiment_data[current_result_idx]);
                            current_result_idx++;
                        }
                    }
                }
            }
            free_matrix(A, n); free_vector(x_true); free_vector(b);
            continue;
        }

        for (int x0_idx = 0; x0_idx < num_x0_types; x0_idx++) {
            InitialVectorType current_x0_type = x0_types[x0_idx];
            double* x0 = create_initial_guess_x0(n, current_x0_type, (unsigned int)x0_idx);
            if (!x0) { fprintf(stderr, "Failed to create x0 for N=%d, type=%s.\n", n, get_x0_type_string(current_x0_type)); continue; }

            for (int crit_iter_idx = 0; crit_iter_idx < num_criterion_to_test; crit_iter_idx++) {
                StopCriterionType current_criterion = criterion_to_test[crit_iter_idx];

                for (int rho_idx = 0; rho_idx < num_rho_values; rho_idx++) {
                    double current_rho = rho_values[rho_idx];

                    printf("  N=%-4d, X0:%-7s, Crit:%-9s, Rho=%.1e: ",
                           n, get_x0_type_string(current_x0_type),
                           get_criterion_type_string(current_criterion), current_rho);

                    double* x_solution = create_vector(n);
                    if (!x_solution) { fprintf(stderr, "Failed to create x_solution for N=%d.\n", n); continue; }

                    ExperimentData* current_data_ptr = NULL;
                    if(current_result_idx < max_results) {
                        current_data_ptr = &all_experiment_data[current_result_idx];
                    } else {
                        static int buffer_full_warning_printed_for_n = 0;
                        if (buffer_full_warning_printed_for_n != n) { // Pokaż ostrzeżenie tylko raz dla danego N
                           fprintf(stderr, "\nWarning: Exceeded max_results buffer (%d) for N=%d. Some data for LaTeX might be lost.\n", max_results, n);
                           buffer_full_warning_printed_for_n = n;
                        }
                    }

                    if (current_data_ptr) {
                        current_data_ptr->n = n;
                        current_data_ptr->x0_type = current_x0_type;
                        current_data_ptr->criterion_type = current_criterion;
                        current_data_ptr->rho = current_rho;
                        current_data_ptr->iterations_taken = 0;
                        current_data_ptr->time_solve_sec = NAN;
                        current_data_ptr->time_per_iteration_sec = NAN;
                        current_data_ptr->final_abs_error = NAN;
                        current_data_ptr->final_criterion_metric = NAN;
                    }

                    int temp_iters_taken;
                    double temp_final_criterion_metric;
                    double temp_time_solve_sec;

                    int status = jacobi_solve(A, b, x_solution, n, x0, current_rho, current_criterion,
                                              JACOBI_MAX_ITER,
                                              current_data_ptr ? &current_data_ptr->iterations_taken : &temp_iters_taken,
                                              current_data_ptr ? &current_data_ptr->final_criterion_metric : &temp_final_criterion_metric,
                                              current_data_ptr ? &current_data_ptr->time_solve_sec : &temp_time_solve_sec);

                    int actual_iters_taken = current_data_ptr ? current_data_ptr->iterations_taken : temp_iters_taken;
                    double actual_time_solve_sec = current_data_ptr ? current_data_ptr->time_solve_sec : temp_time_solve_sec;
                    double calculated_abs_error = NAN;

                    if (status == 0 || status == 1) {
                        calculated_abs_error = calculate_max_abs_diff(x_solution, x_true, n);
                        if (current_data_ptr) {
                            current_data_ptr->final_abs_error = calculated_abs_error;
                            if (current_data_ptr->iterations_taken > 0) {
                                current_data_ptr->time_per_iteration_sec = current_data_ptr->time_solve_sec / current_data_ptr->iterations_taken;
                            } else {
                                current_data_ptr->time_per_iteration_sec = (current_data_ptr->iterations_taken == 0 && current_data_ptr->time_solve_sec > 0) ? current_data_ptr->time_solve_sec : 0.0;
                            }
                        }
                        printf("Iters=%-5d, Err=%.2e, Time=%.3fs. Status: %s\n",
                               actual_iters_taken, calculated_abs_error,
                               actual_time_solve_sec, (status==0 ? "Converged" : "MaxIter"));

                        if (current_data_ptr) {
                           append_to_csv_file(csv_fp_results, current_data_ptr);
                           if(current_result_idx < max_results) current_result_idx++;
                        }
                    } else {
                        printf("Jacobi solver failed (status code %d).\n", status);
                        if (current_data_ptr) {
                            current_data_ptr->iterations_taken = status;
                            current_data_ptr->final_abs_error = NAN;
                            current_data_ptr->time_per_iteration_sec = NAN;
                            current_data_ptr->final_criterion_metric = (status == -2) ? -2.0 : NAN;
                            append_to_csv_file(csv_fp_results, current_data_ptr);
                            if(current_result_idx < max_results) current_result_idx++;
                        }
                    }
                    free_vector(x_solution);
                }
            }
            free_vector(x0);
        }
        free_matrix(A, n);
        free_vector(x_true);
        free_vector(b);
        printf("Finished processing for N = %d.\n\n", n);
    }

    if (csv_fp_results) {
        close_csv_file(csv_fp_results);
        printf("Main results CSV file closed: data/%s.csv\n", results_csv_base);
    }

    printf("\n--- Generating Gnuplot Scripts and LaTeX Tables ---\n");
    for (int n_idx_main = 0; n_idx_main < num_n_values; n_idx_main++) {
        generate_gnuplot_scripts_for_n(results_csv_base, n_values[n_idx_main],
                                       x0_types, num_x0_types,
                                       criterion_to_test, num_criterion_to_test);
    }

    if (current_result_idx > 0) {
        double selected_rhos_for_table[] = {1e-8, 1e-12};
        int num_selected_rhos_table = sizeof(selected_rhos_for_table) / sizeof(selected_rhos_for_table[0]);

        for (int n_idx_main = 0; n_idx_main < num_n_values; ++n_idx_main) {
             generate_latex_table_selected_results("latex_jacobi_summary",
                                              all_experiment_data, current_result_idx,
                                              "Jacobi Solver Experiment Results",
                                              n_values[n_idx_main],
                                              selected_rhos_for_table, num_selected_rhos_table);
        }
    } else {
        printf("No results were stored in all_experiment_data buffer to generate LaTeX summary tables.\n");
    }

    free(all_experiment_data);
    free(spectral_radii);

    printf("\nLab finished. Check data/, scripts/, plots/, and latex_out/ directories.\n");
    printf("To generate plots, run the .gp files in the 'scripts' directory using Gnuplot.\n");
    printf("For example: gnuplot scripts/plot_spectral_radii_data.gp\n");
    printf("To generate heatmaps (if CSV data is suitable), use the Python script generate_heatmaps.py.\n");
    return 0;
}