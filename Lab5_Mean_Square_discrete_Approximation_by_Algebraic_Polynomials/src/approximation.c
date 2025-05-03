/**
 * @file approximation.c
 * @brief Implementation of least-squares polynomial approximation functions.
 *
 * Contains the implementation for calculating polynomial coefficients via
 * least squares and for evaluating a polynomial using Horner's method.
 */
#include "../include/approximation.h"
#include "../include/linear_algebra.h" // For gaussianElimination to solve the normal equations
#include <stdio.h>                     // For fprintf (error messages)
#include <stdlib.h>                    // For malloc, free
#include <math.h>                      // For pow

/**
 * @brief Calculates the coefficients of the best-fit polynomial using least squares.
 *
 * Implements the least-squares method by constructing and solving the normal equations:
 * G * a = B
 * where:
 * - 'a' is the vector of unknown polynomial coefficients [a_0, ..., a_m].
 * - G is the Gram matrix (size (m+1) x (m+1)), where G[j][k] = Sum_{i=0}^{n-1} (x_i^(j+k)).
 * - B is the right-hand side vector (size m+1), where B[j] = Sum_{i=0}^{n-1} (y_i * x_i^j).
 *
 * The function performs the following steps:
 * 1. Validates input parameters (n > m, m >= 0).
 * 2. Allocates memory for the Gram matrix G and the vector B.
 * 3. Constructs the matrix G based on the powers of `points_x`.
 * 4. Constructs the vector B based on `points_y` and powers of `points_x`.
 *    (Optimizations are used for powers 0 and 1).
 * 5. Calls `gaussianElimination` to solve the linear system G * a = B for the coefficients 'a'.
 * 6. Frees the allocated memory for G and B.
 * 7. Returns the status of the operation.
 *
 * @param points_x Array of x-coordinates of the data points (size `n`).
 * @param points_y Array of y-coordinates of the data points (size `n`).
 * @param n The number of data points. Must be greater than `degree_m`.
 * @param degree_m The degree of the approximating polynomial. Must be non-negative and less than `n`.
 * @param coeffs_out Output array (size `degree_m + 1`) where the calculated coefficients [a_0, ..., a_m] will be stored.
 * @return 0 on success.
 * @return -1 on failure (invalid input, memory allocation failure, or linear system solve failure).
 *         Error messages are printed to stderr.
 */
int leastSquaresApprox(double points_x[], double points_y[], int n, int degree_m, double coeffs_out[]) {
    // --- Input Validation ---
    if (n <= degree_m) {
        fprintf(stderr, "Error [leastSquaresApprox]: Number of points (%d) must be greater than polynomial degree (%d) for least squares.\n", n, degree_m);
        return -1; // Not enough data points for the desired degree
    }
    if (degree_m < 0) {
         fprintf(stderr, "Error [leastSquaresApprox]: Polynomial degree (%d) cannot be negative.\n", degree_m);
         return -1;
    }

    int num_coeffs = degree_m + 1; // Dimension of the linear system (number of coefficients)

    // --- Allocate Memory for Normal Equations ---
    // Using size_t for allocation size is good practice.
    double *G = (double *)malloc((size_t)num_coeffs * num_coeffs * sizeof(double)); // Gram matrix G
    double *B = (double *)malloc((size_t)num_coeffs * sizeof(double));              // Right-hand side vector B
    if (!G || !B) {
        fprintf(stderr, "Error [leastSquaresApprox]: Memory allocation failed for Gram matrix or B vector.\n");
        free(G); // Free G if it was allocated
        free(B); // Free B if it was allocated
        return -1; // Memory allocation failure
    }

    // --- Construct the Gram Matrix G ---
    // G is symmetric, G[j][k] = G[k][j] = Sum_{i=0}^{n-1} x_i^(j+k)
    // We compute each element directly.
    for (int j = 0; j < num_coeffs; j++) { // Row index
        for (int k = 0; k < num_coeffs; k++) { // Column index
            double sum_xk = 0.0;
            int power = j + k; // The power for x_i in this element
            for (int i = 0; i < n; i++) { // Sum over all data points
                // Optimization: Avoid calling pow for common low powers.
                if (power == 0) {
                    sum_xk += 1.0; // x_i^0 = 1
                } else if (power == 1) {
                    sum_xk += points_x[i]; // x_i^1 = x_i
                } else {
                    sum_xk += pow(points_x[i], power); // General case x_i^(j+k)
                }
            }
            G[j * num_coeffs + k] = sum_xk; // Store in row-major order G[j][k]
        }
    }

    // --- Construct the Right-Hand Side Vector B ---
    // B[j] = Sum_{i=0}^{n-1} y_i * x_i^j
    for (int j = 0; j < num_coeffs; j++) { // Index for B corresponds to power of x
        double sum_yx = 0.0;
        for (int i = 0; i < n; i++) { // Sum over all data points
            // Optimization: Avoid calling pow for common low powers.
             if (j == 0) {
                 sum_yx += points_y[i]; // y_i * x_i^0
             } else if (j == 1) {
                 sum_yx += points_y[i] * points_x[i]; // y_i * x_i^1
             } else {
                 sum_yx += points_y[i] * pow(points_x[i], j); // General case y_i * x_i^j
             }
        }
        B[j] = sum_yx; // Store the result for B[j]
    }

    // --- Solve the Linear System G * coeffs_out = B ---
    // gaussianElimination solves the system for coeffs_out, modifying G and B in place.
    int solve_status = gaussianElimination(G, B, coeffs_out, num_coeffs);

    // --- Cleanup Allocated Memory ---
    free(G);
    free(B);

    // --- Check Solver Status ---
    if (solve_status != 0) {
        // gaussianElimination already printed an error message about singularity.
        fprintf(stderr, "Error [leastSquaresApprox]: Failed to solve the normal equations (matrix likely singular or ill-conditioned for degree m=%d).\n", degree_m);
        return -1; // Indicate failure from the linear solver
    }

    return 0; // Indicate success
}

/**
 * @brief Evaluates a polynomial at a given point x using Horner's method.
 *
 * Computes P(x) = c_0 + c_1*x + c_2*x^2 + ... + c_degree*x^degree,
 * where `coeffs` = [c_0, c_1, ..., c_degree].
 * Horner's method rearranges the computation as:
 * P(x) = c_0 + x*(c_1 + x*(c_2 + ... + x*(c_{degree-1} + x*c_degree)...))
 * This is generally faster and numerically more stable than direct computation
 * of powers of x.
 *
 * @param x The point at which to evaluate the polynomial.
 * @param coeffs Array of polynomial coefficients [c_0, c_1, ..., c_degree].
 * @param degree The degree of the polynomial (size of `coeffs` is `degree + 1`).
 * @return The value of the polynomial P(x).
 */
double evaluatePolynomial(double x, double coeffs[], int degree) {
    double result = 0.0;
    // Iterate from the highest coefficient down to the constant term
    for (int i = degree; i >= 0; i--) {
        result = result * x + coeffs[i]; // The core step of Horner's method
    }
    return result;
}