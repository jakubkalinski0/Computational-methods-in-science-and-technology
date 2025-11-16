#ifndef PROBLEM_DEFINITION_H
#define PROBLEM_DEFINITION_H

#include "common.h"
#include "matrix_operations.h"

void fill_matrix_A(double** A, int n, double k_param, double m_param);
void generate_true_solution_x(double* x_true, int n, unsigned int seed);
void calculate_vector_b(double* b, double** A, double* x_true, int n);
double* create_initial_guess_x0(int n, InitialVectorType type, unsigned int seed_offset);

#endif // PROBLEM_DEFINITION_H