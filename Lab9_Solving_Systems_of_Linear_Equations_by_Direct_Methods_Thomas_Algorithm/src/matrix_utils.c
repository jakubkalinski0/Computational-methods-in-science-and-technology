#include "matrix_utils.h"

Matrix* create_matrix(int rows, int cols) {
    Matrix *m = (Matrix*)malloc(sizeof(Matrix));
    CHECK_ALLOC(m, "Matrix structure");
    m->rows = rows;
    m->cols = cols;
    m->data = (double**)malloc(rows * sizeof(double*));
    CHECK_ALLOC(m->data, "Matrix data rows");
    for (int i = 0; i < rows; i++) {
        m->data[i] = (double*)calloc(cols, sizeof(double)); // Initialize to zero
        CHECK_ALLOC(m->data[i], "Matrix data columns");
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
    v->data = (double*)calloc(size, sizeof(double)); // Initialize to zero
    CHECK_ALLOC(v->data, "Vector data");
    return v;
}

void free_vector(Vector *v) {
    if (v) {
        free(v->data);
        free(v);
    }
}

void generate_A_tridiagonal_full(Matrix *A, int n, double m_param, double k_param, const char* precision_dtype) {
    // Assumes A is already allocated and zeroed out
    for (int i = 0; i < n; i++) {
        A->data[i][i] = cast_to_prec(-m_param * (i + 1.0) - k_param, precision_dtype);
        if (i + 1 < n) {
            A->data[i][i+1] = cast_to_prec((double)(i + 1.0), precision_dtype);
        }
        if (i > 0) {
            A->data[i][i-1] = cast_to_prec(m_param / (double)(i + 1.0), precision_dtype);
        }
    }
}

void generate_A_tridiagonal_banded(Matrix *A_banded, int n, double m_param, double k_param, const char* precision_dtype) {
    // A_banded is nx3: col 0 = lower, col 1 = main, col 2 = upper
    for (int i = 0; i < n; i++) {
        if (i > 0) {
            A_banded->data[i][0] = cast_to_prec(m_param / (double)(i + 1.0), precision_dtype);
        } else {
            A_banded->data[i][0] = cast_to_prec(0.0, precision_dtype);
        }
        A_banded->data[i][1] = cast_to_prec(-m_param * (i + 1.0) - k_param, precision_dtype);
        if (i < n - 1) {
            A_banded->data[i][2] = cast_to_prec((double)(i + 1.0), precision_dtype);
        } else {
            A_banded->data[i][2] = cast_to_prec(0.0, precision_dtype);
        }
    }
}

void generate_x_true(Vector *x, int n_size) {
    srand(FIXED_SEED);
    for (int i = 0; i < n_size; i++) {
        x->data[i] = ((double)rand() / RAND_MAX < 0.5) ? 1.0 : -1.0;
    }
}

void matrix_vector_mult_full(const Matrix *A_full, const Vector *x, Vector *b, const char* precision_dtype) {
    for (int i = 0; i < A_full->rows; i++) {
        double sum_val = 0.0;
        for (int j = 0; j < A_full->cols; j++) {
            sum_val += A_full->data[i][j] * x->data[j];
        }
        b->data[i] = cast_to_prec(sum_val, precision_dtype);
    }
}

void matrix_vector_mult_banded(const Matrix *A_banded, const Vector *x, Vector *b, const char* precision_dtype) {
    int n = A_banded->rows;
    for (int i = 0; i < n; i++) {
        double sum_val = 0.0;
        if (i > 0) {
            sum_val += A_banded->data[i][0] * x->data[i-1];
        }
        sum_val += A_banded->data[i][1] * x->data[i];
        if (i < n - 1) {
            sum_val += A_banded->data[i][2] * x->data[i+1];
        }
        b->data[i] = cast_to_prec(sum_val, precision_dtype);
    }
}

double calculate_max_abs_error(const Vector *v_computed, const Vector *v_true, const char* precision_dtype) {
    double max_err = 0.0;
    for (int i = 0; i < v_computed->size; i++) {
        double val_true_at_prec = cast_to_prec(v_true->data[i], precision_dtype);
        if (isinf(v_computed->data[i]) || isnan(v_computed->data[i])) return 0.0;

        double err = fabs(v_computed->data[i] - val_true_at_prec);
        err = cast_to_prec(err, precision_dtype);

        if (err > max_err) {
            max_err = err;
        }
    }
    return isfinite(max_err) ? max_err : 0.0;
}