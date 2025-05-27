#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include "common.h"

// Matrix and Vector Allocation/Deallocation
Matrix* create_matrix(int rows, int cols);
void free_matrix(Matrix *m);
Matrix* copy_matrix(const Matrix* src);
Vector* create_vector(int size);
void free_vector(Vector *v);

// Matrix Generation (Task 3b specific)
void generate_A_tridiagonal_full(Matrix *A, int n, double m_param, double k_param, const char* precision_dtype);
void generate_A_tridiagonal_banded(Matrix *A_banded, int n, double m_param, double k_param, const char* precision_dtype);

// Vector Generation
void generate_x_true(Vector *x, int n_size); // Uses FIXED_SEED

// Matrix-Vector Multiplication
void matrix_vector_mult_full(const Matrix *A_full, const Vector *x, Vector *b, const char* precision_dtype);
void matrix_vector_mult_banded(const Matrix *A_banded, const Vector *x, Vector *b, const char* precision_dtype);

// Error Calculation
double calculate_max_abs_error(const Vector *v_computed, const Vector *v_true, const char* precision_dtype);

#endif // MATRIX_UTILS_H