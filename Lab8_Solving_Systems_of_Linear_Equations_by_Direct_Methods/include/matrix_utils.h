#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include "common.h"

// Matrix and Vector Allocation/Deallocation
Matrix* create_matrix(int rows, int cols);
void free_matrix(Matrix *m);
Vector* create_vector(int size);
void free_vector(Vector *v);
Matrix* copy_matrix(const Matrix* src);


// Matrix Generation
void generate_matrix_I(Matrix *A, int n, const char* precision_dtype); // precision_dtype for type casting if needed
void generate_matrix_II(Matrix *A, int m, const char* precision_dtype);

// Vector Generation
void generate_x_true(Vector *x, int size, int current_matrix_size);

// Matrix Operations
void matrix_vector_mult(const Matrix *A, const Vector *x, Vector *b, const char* precision_dtype);
double matrix_norm_1(const Matrix *A, const char* precision_dtype); // L1 norm

#endif // MATRIX_UTILS_H