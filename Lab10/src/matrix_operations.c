#include "../include/matrix_operations.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h> // For fabs, sqrt

double** create_matrix(int rows, int cols) {
    double** matrix = (double**)malloc(rows * sizeof(double*));
    if (!matrix) return NULL;
    for (int i = 0; i < rows; i++) {
        matrix[i] = (double*)malloc(cols * sizeof(double));
        if (!matrix[i]) {
            // Free previously allocated rows
            for (int k = 0; k < i; k++) free(matrix[k]);
            free(matrix);
            return NULL;
        }
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 0.0; // Initialize to zero
        }
    }
    return matrix;
}

void free_matrix(double** matrix, int rows) {
    if (!matrix) return;
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void print_matrix(double** matrix, int rows, int cols, const char* name) {
    printf("Matrix %s (%dx%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%10.4f ", matrix[i][j]);
        }
        printf("\n");
    }
}

void copy_matrix(double** dest, double** src, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

double* create_vector(int size) {
    double* vector = (double*)malloc(size * sizeof(double));
    if (!vector) return NULL;
    for (int i = 0; i < size; i++) {
        vector[i] = 0.0; // Initialize to zero
    }
    return vector;
}

void free_vector(double* vector) {
    free(vector);
}

void print_vector(double* vector, int size, const char* name) {
    printf("Vector %s (size %d):\n", name, size);
    for (int i = 0; i < size; i++) {
        printf("%10.4f ", vector[i]);
        if ((i + 1) % 10 == 0 || i == size - 1) printf("\n");
    }
    if (size > 0 && size % 10 != 0) printf("\n"); // Ensure last newline
}

void copy_vector(double* dest, double* src, int size) {
    for (int i = 0; i < size; i++) {
        dest[i] = src[i];
    }
}

void vector_set_all(double* vec, int size, double value) {
    for (int i = 0; i < size; i++) {
        vec[i] = value;
    }
}

double vector_norm_l2(double* vector, int size) {
    double sum_sq = 0.0;
    for (int i = 0; i < size; i++) {
        sum_sq += vector[i] * vector[i];
    }
    return sqrt(sum_sq);
}

double vector_norm_max(double* vector, int size) {
    double max_val = 0.0;
    if (size > 0) max_val = fabs(vector[0]);
    for (int i = 1; i < size; i++) {
        if (fabs(vector[i]) > max_val) {
            max_val = fabs(vector[i]);
        }
    }
    return max_val;
}

void vector_subtract(double* result, double* v1, double* v2, int size) {
    for (int i = 0; i < size; i++) {
        result[i] = v1[i] - v2[i];
    }
}

void matrix_vector_multiply(double* result_vec, double** matrix, double* vector, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        result_vec[i] = 0.0;
        for (int j = 0; j < cols; j++) {
            result_vec[i] += matrix[i][j] * vector[j];
        }
    }
}

void scalar_vector_multiply(double* result_vec, double scalar, double* vector, int size) {
    for (int i = 0; i < size; i++) {
        result_vec[i] = scalar * vector[i];
    }
}

double vector_dot_product(double* v1, double* v2, int size) {
    double dot = 0.0;
    for (int i = 0; i < size; i++) {
        dot += v1[i] * v2[i];
    }
    return dot;
}

double calculate_max_abs_diff(double* v1, double* v2, int size) {
    double max_diff = 0.0;
    if (size <= 0) return 0.0;

    for (int i = 0; i < size; i++) {
        double diff = fabs(v1[i] - v2[i]);
        if (diff > max_diff) {
            max_diff = diff;
        }
    }
    return max_diff;
}