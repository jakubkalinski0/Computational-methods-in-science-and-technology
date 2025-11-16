#include "../include/problem_definition.h"

void fill_matrix_A(double** A, int n, double k_param, double m_param) {
    for (int r = 0; r < n; r++) { // 0-indexed row
        for (int c = 0; c < n; c++) { // 0-indexed col
            if (r == c) {
                A[r][c] = k_param;
            } else {
                // Convert 0-indexed r, c to 1-indexed i, j for the formula
                double i_1idx = (double)r + 1.0;
                double j_1idx = (double)c + 1.0;
                A[r][c] = m_param / ( (double)n - i_1idx - j_1idx + 0.5 );
            }
        }
    }
}

void generate_true_solution_x(double* x_true, int n, unsigned int seed) {
    srand(seed);
    for (int i = 0; i < n; i++) {
        x_true[i] = (rand() % 2 == 0) ? 1.0 : -1.0;
    }
}

void calculate_vector_b(double* b, double** A, double* x_true, int n) {
    matrix_vector_multiply(b, A, x_true, n, n);
}

double* create_initial_guess_x0(int n, InitialVectorType type, unsigned int seed_offset) {
    double* x0 = create_vector(n);
    if (!x0) return NULL;

    switch (type) {
        case X0_ZEROS:
            // create_vector already initializes to zeros
                break;
        case X0_ONES:
            vector_set_all(x0, n, 1.0);
        break;
        case X0_RANDOM_LOW:
            srand(TRUE_X_SEED + seed_offset + 100); // Different seed
        for (int i = 0; i < n; i++) {
            x0[i] = (double)(rand() % 21 - 10); // Random in [-10, 10]
        }
        break;
        case X0_ALTERNATING_HIGH:
            for (int i = 0; i < n; i++) {
                x0[i] = (i % 2 == 0) ? 100.0 : -100.0;
            }
        break;
        default:
            // Default to zeros
                break;
    }
    return x0;
}