#include "matrix_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memset
#include <float.h>  // For DBL_MAX, FLT_MAX etc.

// Helper for casting based on precision_dtype string
static double cast_to_precision(double val, const char* precision_dtype) {
    if (strcmp(precision_dtype, "float") == 0) {
        return (float)val;
    }
    return val; // double
}


Matrix* create_matrix(int rows, int cols) {
    Matrix *m = (Matrix*)malloc(sizeof(Matrix));
    CHECK_ALLOC(m, "Matrix structure");
    m->rows = rows;
    m->cols = cols;
    m->data = (double**)malloc(rows * sizeof(double*));
    CHECK_ALLOC(m->data, "Matrix data rows");
    for (int i = 0; i < rows; i++) {
        m->data[i] = (double*)malloc(cols * sizeof(double));
        CHECK_ALLOC(m->data[i], "Matrix data columns");
        memset(m->data[i], 0, cols * sizeof(double)); // Initialize to zero
    }
    return m;
}

void free_matrix(Matrix *m) {
    if (m) {
        if (m->data) {
            for (int i = 0; i < m->rows; i++) {
                free(m->data[i]);
            }
            free(m->data);
        }
        free(m);
    }
}

Matrix* copy_matrix(const Matrix* src) {
    if (!src) return NULL;
    Matrix* dest = create_matrix(src->rows, src->cols);
    for (int i = 0; i < src->rows; i++) {
        for (int j = 0; j < src->cols; j++) {
            dest->data[i][j] = src->data[i][j];
        }
    }
    return dest;
}


Vector* create_vector(int size) {
    Vector *v = (Vector*)malloc(sizeof(Vector));
    CHECK_ALLOC(v, "Vector structure");
    v->size = size;
    v->data = (double*)malloc(size * sizeof(double));
    CHECK_ALLOC(v->data, "Vector data");
    memset(v->data, 0, size * sizeof(double)); // Initialize to zero
    return v;
}

void free_vector(Vector *v) {
    if (v) {
        free(v->data);
        free(v);
    }
}

void generate_matrix_I(Matrix *A, int n, const char* precision_dtype) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == 0 || j == 0) {
                A->data[i][j] = cast_to_precision(1.0, precision_dtype);
            } else {
                // Using 0-based indexing (i, j) for C, corresponds to (i+1, j+1) in 1-based formula
                // Formula: 1 / ( (row_1_based) + (col_1_based) - 1 )
                //  = 1 / ( (i+1) + (j+1) - 1 ) = 1 / (i+j+1)
                A->data[i][j] = cast_to_precision(1.0 / ((double)i + (double)j + 1.0), precision_dtype);
            }
        }
    }
}

void generate_matrix_II(Matrix *A, int m, const char* precision_dtype) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            // Using 0-based indexing (i, j) for C
            // Formula: a_rc = 2*r_1_based / c_1_based for c_1_based >= r_1_based
            // r_1_based = i+1, c_1_based = j+1
            if (j >= i) { // Equivalent to (j+1) >= (i+1)
                A->data[i][j] = cast_to_precision((2.0 * ((double)i + 1.0)) / ((double)j + 1.0), precision_dtype);
            } else {
                A->data[i][j] = A->data[j][i]; // Symmetric
            }
        }
    }
}

void generate_x_true(Vector *x, int size, int current_matrix_size) {
    // Seed depends on current_matrix_size to match Python behavior where RandomState was re-initialized
    unsigned int seed = FIXED_SEED + current_matrix_size;
    srand(seed); // Seed the random number generator

    for (int i = 0; i < size; i++) {
        // rand() / RAND_MAX gives a float in [0,1]
        if ((double)rand() / RAND_MAX < 0.5) {
            x->data[i] = 1.0;
        } else {
            x->data[i] = -1.0;
        }
    }
}

void matrix_vector_mult(const Matrix *A, const Vector *x, Vector *b, const char* precision_dtype) {
    if (A->cols != x->size || A->rows != b->size) {
        fprintf(stderr, "Error: Matrix/vector dimension mismatch for multiplication.\n");
        return;
    }
    for (int i = 0; i < A->rows; i++) {
        double sum = 0.0;
        for (int j = 0; j < A->cols; j++) {
            // Perform multiplication in double, then cast if needed
            sum += A->data[i][j] * x->data[j];
        }
        b->data[i] = cast_to_precision(sum, precision_dtype);
    }
}

double matrix_norm_1(const Matrix *A, const char* precision_dtype) {
    double max_col_sum = 0.0;
    bool is_float = (strcmp(precision_dtype, "float") == 0);

    for (int j = 0; j < A->cols; j++) {
        double current_col_sum = 0.0;
        for (int i = 0; i < A->rows; i++) {
            double val = A->data[i][j];
            if (is_float) val = (float)val; // consider value at specified precision

            if (isinf(val) || isnan(val)) return INFINITY; // Propagate Inf/NaN
            current_col_sum += fabs(val);
            if (is_float) current_col_sum = (float)current_col_sum; // Keep sum at precision
        }
        if (current_col_sum > max_col_sum) {
            max_col_sum = current_col_sum;
        }
    }
    return cast_to_precision(max_col_sum, precision_dtype);
}