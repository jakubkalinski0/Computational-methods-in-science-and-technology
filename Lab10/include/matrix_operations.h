#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

#include "common.h"

// --- Matrix Functions ---
double** create_matrix(int rows, int cols);
void free_matrix(double** matrix, int rows);
void print_matrix(double** matrix, int rows, int cols, const char* name);
void copy_matrix(double** dest, double** src, int rows, int cols);

// --- Vector Functions ---
double* create_vector(int size);
void free_vector(double* vector);
void print_vector(double* vector, int size, const char* name);
void copy_vector(double* dest, double* src, int size);
void vector_set_all(double* vec, int size, double value);
double vector_norm_l2(double* vector, int size);
double vector_norm_max(double* vector, int size); // Max absolute value
void vector_subtract(double* result, double* v1, double* v2, int size);
void matrix_vector_multiply(double* result_vec, double** matrix, double* vector, int rows, int cols);
void scalar_vector_multiply(double* result_vec, double scalar, double* vector, int size);
double vector_dot_product(double* v1, double* v2, int size);
double calculate_max_abs_diff(double* v1, double* v2, int size);


#endif // MATRIX_OPERATIONS_H