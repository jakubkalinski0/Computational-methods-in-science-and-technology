#include "../include/jacobi_solver.h"
#include "../include/matrix_operations.h" // Dla operacji na wektorach i macierzach
#include <math.h>                         // Dla fabs, isnan
#include <stdio.h>                        // Dla fprintf
#include <time.h>                         // Dla clock()
#include <stdlib.h>                       // Dla malloc/free (choć create_vector jest w matrix_operations)

// Definicja funkcji musi być zgodna z deklaracją w jacobi_solver.h
int jacobi_solve(double** A, double* b, double* x_solution, int n,
                 double* x_initial, double rho_tolerance, StopCriterionType criterion_type,
                 int max_iterations, int* iterations_taken_ptr,
                 double* final_criterion_metric_ptr, double* time_taken_sec_ptr) {

    clock_t start_time = clock();

    double* x_current = create_vector(n);
    double* x_next = create_vector(n);
    double* temp_vec_for_criterion = create_vector(n);

    if (!x_current || !x_next || !temp_vec_for_criterion) {
        free_vector(x_current);
        free_vector(x_next);
        free_vector(temp_vec_for_criterion);
        if (iterations_taken_ptr) *iterations_taken_ptr = 0;
        if (final_criterion_metric_ptr) *final_criterion_metric_ptr = NAN;
        if (time_taken_sec_ptr) *time_taken_sec_ptr = 0;
        fprintf(stderr, "Error: Memory allocation failed in jacobi_solve for auxiliary vectors.\n");
        return -1;
    }

    copy_vector(x_current, x_initial, n);

    int iter_count = 0;
    double current_criterion_value = NAN; // Inicjalizuj na wypadek, gdyby pętla nie wykonała się ani razu

    for (iter_count = 0; iter_count < max_iterations; iter_count++) {
        for (int i = 0; i < n; i++) {
            double sigma = 0.0;
            for (int j = 0; j < n; j++) {
                if (i != j) {
                    sigma += A[i][j] * x_current[j];
                }
            }
            if (fabs(A[i][i]) < 1e-12) {
                fprintf(stderr, "Error: Diagonal element A[%d][%d] (%.2e) is near zero in jacobi_solve.\n", i, i, A[i][i]);
                free_vector(x_current);
                free_vector(x_next);
                free_vector(temp_vec_for_criterion);
                if (iterations_taken_ptr) *iterations_taken_ptr = iter_count;
                if (final_criterion_metric_ptr) *final_criterion_metric_ptr = NAN;
                if (time_taken_sec_ptr) *time_taken_sec_ptr = (double)(clock() - start_time) / CLOCKS_PER_SEC;
                return -2;
            }
            x_next[i] = (b[i] - sigma) / A[i][i];
        }

        if (criterion_type == INCREMENT_CRITERION) {
            vector_subtract(temp_vec_for_criterion, x_next, x_current, n);
            current_criterion_value = vector_norm_l2(temp_vec_for_criterion, n);
        } else { // RESIDUAL_CRITERION
            matrix_vector_multiply(temp_vec_for_criterion, A, x_next, n, n);
            vector_subtract(temp_vec_for_criterion, temp_vec_for_criterion, b, n);
            current_criterion_value = vector_norm_l2(temp_vec_for_criterion, n);
        }

        if (isnan(current_criterion_value)) {
            fprintf(stderr, "Warning: Criterion value became NaN at iteration %d. Stopping.\n", iter_count + 1);
            if (iterations_taken_ptr) *iterations_taken_ptr = iter_count + 1;
            if (final_criterion_metric_ptr) *final_criterion_metric_ptr = NAN;
            if (time_taken_sec_ptr) *time_taken_sec_ptr = (double)(clock() - start_time) / CLOCKS_PER_SEC;
            copy_vector(x_solution, x_current, n);
            free_vector(x_current);
            free_vector(x_next);
            free_vector(temp_vec_for_criterion);
            return -3;
        }

        copy_vector(x_current, x_next, n);

        if (current_criterion_value < rho_tolerance) {
            break;
        }
    }

    copy_vector(x_solution, x_current, n);

    if (iterations_taken_ptr) {
        // Jeśli pętla zakończyła się przez break, iter_count jest indeksem iteracji, w której warunek został spełniony.
        // Liczba wykonanych iteracji to iter_count + 1.
        // Jeśli pętla dobiegła do max_iterations, iter_count = max_iterations.
        *iterations_taken_ptr = (iter_count < max_iterations) ? (iter_count + 1) : max_iterations;
        if (max_iterations == 0) *iterations_taken_ptr = 0;
    }
    if (final_criterion_metric_ptr) {
        // Jeśli max_iterations == 0, kryterium nie zostało obliczone, więc current_criterion_value może być nieustawione.
        // W takim przypadku, można ustawić na NAN lub wartość początkową.
        // Tutaj, jeśli pętla nie wykonała się (np. max_iterations = 0), current_criterion_value jest NAN.
        // Jeśli pętla się wykonała, current_criterion_value zawiera ostatnią obliczoną wartość kryterium.
        *final_criterion_metric_ptr = current_criterion_value;
    }

    clock_t end_time = clock();
    if (time_taken_sec_ptr) {
        *time_taken_sec_ptr = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    }

    free_vector(x_current);
    free_vector(x_next);
    free_vector(temp_vec_for_criterion);

    return (iter_count < max_iterations) ? 0 : 1;
}