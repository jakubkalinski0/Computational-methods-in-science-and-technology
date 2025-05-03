/**
 * @file linear_algebra.c
 * @brief Implementation of Gaussian elimination for solving linear systems.
 *
 * Contains the function `gaussianElimination` which solves Ax = b using
 * forward elimination with partial pivoting followed by back substitution.
 */
#include "../include/linear_algebra.h"
#include <math.h>   // For fabs()
#include <stdio.h>  // For fprintf (error messages)
#include <stdlib.h> // For memory allocation (though not used directly in this basic version)

/**
 * @brief Macro for accessing elements of a matrix stored in a 1D row-major array.
 * Facilitates 2D-like indexing (row, column) for a matrix A of size n x n.
 * Example: MAT(A, r, c, n) accesses the element at row `r` and column `c`.
 */
#define MAT(A, r, c, n) (A[(r)*(n) + (c)])

/**
 * @brief Solves the linear system Ax = b using Gaussian elimination with partial pivoting.
 *
 * Implements the standard Gaussian elimination algorithm with modifications for numerical stability:
 * 1.  **Forward Elimination:** Transforms the matrix A into an upper triangular form.
 *     -   **Partial Pivoting:** In each step `k`, finds the row `max_row` (from `k` downwards)
 *         containing the element with the largest absolute value in column `k`. Swaps row `k`
 *         with `max_row` in both matrix `A` and vector `b`. This helps to avoid division by
 *         small numbers and reduces round-off errors.
 *     -   **Singularity Check:** After pivoting, checks if the pivot element `A[k][k]` is close to zero
 *         (using a tolerance `1e-12`). If it is, the matrix is considered singular or near-singular,
 *         and the function returns an error.
 *     -   **Elimination:** For each row `i` below the pivot row `k`, calculates a factor `f = A[i][k] / A[k][k]`
 *         and subtracts `f` times row `k` from row `i` to eliminate the element `A[i][k]`.
 *         The same operation `b[i] -= f * b[k]` is applied to the right-hand side vector `b`.
 * 2.  **Back Substitution:** Solves the resulting upper triangular system `Ux = y` (where U is the modified A
 *     and y is the modified b) for the unknown vector `x`, starting from the last equation and working backwards.
 *
 * Note: The input matrix `A` and vector `b` are modified **in place**.
 *
 * @param A Input square matrix (n x n), stored row-major. Modified in place to upper triangular form.
 * @param b Input right-hand side vector (size n). Modified in place during forward elimination.
 * @param x Output vector (size n) where the solution is stored. The caller must allocate memory for `x`.
 * @param n The dimension of the system.
 * @return 0 on success.
 * @return -1 if singularity or near-singularity is detected. Error message printed to stderr.
 */
int gaussianElimination(double *A, double *b, double *x, int n) {
    int i, j, k, max_row;
    double factor, temp, max_val;
    const double PIVOT_TOLERANCE = 1e-12; // Tolerance for checking if pivot is near zero

    // --- Forward Elimination with Partial Pivoting ---
    for (k = 0; k < n - 1; k++) { // Iterate through columns (pivots) from 0 to n-2
        // --- Find Pivot Row ---
        // Find the row index `max_row` (from k to n-1) with the largest absolute value in column k.
        max_row = k;
        max_val = fabs(MAT(A, k, k, n)); // Start with the current diagonal element
        for (i = k + 1; i < n; i++) {
            double current_val = fabs(MAT(A, i, k, n));
            if (current_val > max_val) {
                max_val = current_val;
                max_row = i;
            }
        }

        // --- Swap Rows if Necessary ---
        // If the pivot row `max_row` is different from the current row `k`, swap them.
        if (max_row != k) {
            // Swap elements in matrix A for columns k through n-1
            for (j = k; j < n; j++) {
                temp = MAT(A, k, j, n);
                MAT(A, k, j, n) = MAT(A, max_row, j, n);
                MAT(A, max_row, j, n) = temp;
            }
            // Swap corresponding elements in vector b
            temp = b[k];
            b[k] = b[max_row];
            b[max_row] = temp;
        }

        // --- Check for Singularity ---
        // After pivoting, check if the pivot element A[k][k] is too small.
        if (fabs(MAT(A, k, k, n)) < PIVOT_TOLERANCE) {
            // fprintf(stderr, "Error [gaussianElimination]: Matrix is singular or near-singular (pivot close to zero) at step k=%d.\n", k);
            return -1; // Indicate failure due to singularity
        }

        // --- Eliminate Elements Below the Pivot ---
        // For each row `i` below the current pivot row `k`.
        for (i = k + 1; i < n; i++) {
            // Calculate the factor needed to eliminate A[i][k]
            factor = MAT(A, i, k, n) / MAT(A, k, k, n);

            // Explicitly set the element below the pivot to zero (optional, but good practice)
            // MAT(A, i, k, n) = 0.0;
            // Note: This element is often skipped in the inner loop (j starts from k+1).
            // If set here, the inner loop must still start at k+1. Let's keep it implicit.

            // Update the rest of the row `i` in matrix A
            for (j = k + 1; j < n; j++) {
                MAT(A, i, j, n) -= factor * MAT(A, k, j, n);
            }
            // Update the corresponding element in vector b
            b[i] -= factor * b[k];
        }
    } // End of forward elimination loop

    // --- Check Singularity of the Last Diagonal Element ---
    // The loop finishes at n-2, so the last pivot A[n-1][n-1] needs checking if n > 0.
     if (n > 0 && fabs(MAT(A, n - 1, n - 1, n)) < PIVOT_TOLERANCE) {
         // fprintf(stderr, "Error [gaussianElimination]: Matrix is singular or near-singular (last diagonal element close to zero).\n");
         return -1; // Indicate failure
     }


    // --- Back Substitution ---
    // Solve the upper triangular system Ax = b (where A and b are now modified)
    for (i = n - 1; i >= 0; i--) { // Iterate backwards from the last row (n-1) to the first (0)
        double sum = 0.0;
        // Calculate the sum of A[i][j] * x[j] for j > i
        for (j = i + 1; j < n; j++) {
            sum += MAT(A, i, j, n) * x[j]; // x[j] values are already known from previous steps
        }
        // Solve for x[i]: x[i] = (b[i] - sum) / A[i][i]
        // Division by A[i][i] relies on the pivot check preventing division by near zero.
        x[i] = (b[i] - sum) / MAT(A, i, i, n);
    }

    return 0; // Indicate successful completion
}