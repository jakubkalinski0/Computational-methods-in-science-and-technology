#ifndef GAUSSIAN_ELIM_H
#define GAUSSIAN_ELIM_H

#include "common.h"

// Returns true if successful, false if matrix is singular (or effectively so)
// A_lu will contain L (excluding diagonal 1s) and U.
// pivot_indices will store the row permutation.
// scale_factors stores scaling factors for each row.
bool lu_factorize_spp(Matrix *A_lu, int *pivot_indices, double *scale_factors, const char* precision_dtype);

// Solves Ly' = Pb (forward substitution) then Ux = y' (backward substitution)
// A_lu: matrix containing L and U factors.
// pivot_indices: row permutation from factorization.
// b: right-hand side vector.
// x_sol: solution vector (output).
void lu_solve_permuted(const Matrix *A_lu, const int *pivot_indices, const Vector *b, Vector *x_sol, const char* precision_dtype);

// Solves Ax = b using LU factorization with scaled partial pivoting.
// A_orig: original matrix (will be copied internally).
// b_orig: original right-hand side vector.
// x_sol: solution vector (output).
// Returns true if solution is found, false if matrix is singular.
bool solve_system_spp(const Matrix *A_orig, const Vector *b_orig, Vector *x_sol, const char* precision_dtype);

// Computes A_inv using LU factorization and solving Ax_j = e_j for each column j.
// A_orig: original matrix.
// A_inv: inverted matrix (output).
// Returns true if inversion is successful, false if matrix is singular.
bool invert_matrix_spp(const Matrix *A_orig, Matrix *A_inv, const char* precision_dtype);

#endif // GAUSSIAN_ELIM_H