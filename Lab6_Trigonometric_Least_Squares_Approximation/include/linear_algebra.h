/**
* @file linear_algebra.h
 * @brief Header file for linear algebra functions, specifically Gaussian elimination.
 *
 * Defines the function prototype for solving a system of linear equations Ax = b
 * using Gaussian elimination with basic partial pivoting.
 */
#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

#include <stddef.h> // For size_t (though not directly used in prototype, it's often used with sizes)

/**
 * @brief Solves a system of linear equations Ax = b using Gaussian elimination with partial pivoting.
 *
 * This function implements the Gaussian elimination algorithm to find the solution vector `x`
 * for a given square matrix `A` and right-hand side vector `b`.
 * It modifies the input matrix `A` and vector `b` **in place**. After execution, `A` will
 * be transformed into an upper triangular matrix (or nearly so, due to pivoting).
 * Partial pivoting (swapping rows to use the largest available pivot element in the current column)
 * is used to improve numerical stability.
 *
 * @param A Input square matrix (n x n) of coefficients, stored row-major in a 1D array (size n*n).
 *          **Modified in place** during the elimination process.
 * @param b Input right-hand side vector (size n). **Modified in place** during forward elimination.
 * @param x Output vector (size n) where the solution will be stored. The caller must allocate memory for `x`.
 * @param n The dimension of the system (number of equations/unknowns, size of the matrix A is n x n).
 * @return 0 on success.
 * @return -1 if the matrix `A` is detected as singular or near-singular during the process
 *         (i.e., a pivot element is close to zero), indicating that a unique solution may not exist
 *         or cannot be reliably found. An error message is printed to stderr in this case.
 */
int gaussianElimination(double *A, double *b, double *x, int n);

#endif // LINEAR_ALGEBRA_H