#include "thomas_algorithm.h"
#include "matrix_utils.h" 

bool thomas_solve(const Matrix *A_banded_nx3, const Vector *d_rhs, Vector *x_sol, const char* precision_dtype) {
    int n = A_banded_nx3->rows;
    if (n == 0) return true;
    
    double_t current_eps_factor = 10.0; 
    double machine_eps = get_prec_epsilon(precision_dtype);

    Vector *c_prime = create_vector(n); 
    Vector *d_prime = create_vector(n); 

    double b0 = cast_to_prec(A_banded_nx3->data[0][1], precision_dtype);
    if (fabs(b0) < machine_eps * DBL_MIN * current_eps_factor) { 
        fprintf(stderr, "THOMAS_ERROR (%s): Division by zero or near-zero pivot at first row. b0=%e\n", precision_dtype, b0);
        free_vector(c_prime);
        free_vector(d_prime);
        for(int i=0; i<n; ++i) x_sol->data[i] = NAN;
        return false; 
    }
    c_prime->data[0] = cast_to_prec(cast_to_prec(A_banded_nx3->data[0][2], precision_dtype) / b0, precision_dtype);
    d_prime->data[0] = cast_to_prec(cast_to_prec(d_rhs->data[0], precision_dtype) / b0, precision_dtype);

    for (int i = 1; i < n; i++) {
        double l_i = cast_to_prec(A_banded_nx3->data[i][0], precision_dtype); 
        double d_i_main = cast_to_prec(A_banded_nx3->data[i][1], precision_dtype); 
        double u_i = cast_to_prec(A_banded_nx3->data[i][2], precision_dtype); 
        
        double c_prev_prime = cast_to_prec(c_prime->data[i-1], precision_dtype);
        double d_prev_prime_rhs = cast_to_prec(d_prime->data[i-1], precision_dtype);

        double denominator = cast_to_prec(d_i_main - cast_to_prec(l_i * c_prev_prime, precision_dtype), precision_dtype);

        if (fabs(denominator) < machine_eps * DBL_MIN * current_eps_factor) {
            fprintf(stderr, "THOMAS_ERROR (%s): Division by zero or near-zero pivot at row %d. Denom=%e\n", precision_dtype, i, denominator);
            free_vector(c_prime);
            free_vector(d_prime);
            for(int k=0; k<n; ++k) x_sol->data[k] = NAN;
            return false; 
        }

        if (i < n - 1) { 
            c_prime->data[i] = cast_to_prec(u_i / denominator, precision_dtype);
        }
        d_prime->data[i] = cast_to_prec(
            (cast_to_prec(d_rhs->data[i], precision_dtype) - cast_to_prec(l_i * d_prev_prime_rhs, precision_dtype)) / denominator, 
            precision_dtype
        );
    }

    x_sol->data[n-1] = cast_to_prec(d_prime->data[n-1], precision_dtype);
    for (int i = n - 2; i >= 0; i--) {
        x_sol->data[i] = cast_to_prec(
            cast_to_prec(d_prime->data[i], precision_dtype) - 
            cast_to_prec(cast_to_prec(c_prime->data[i], precision_dtype) * x_sol->data[i+1], precision_dtype),
            precision_dtype
        );
    }

    free_vector(c_prime);
    free_vector(d_prime);
    return true;
}