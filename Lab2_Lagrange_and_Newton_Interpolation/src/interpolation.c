/**
 * @file interpolation.c
 * @brief Implementation of Lagrange and Newton interpolation algorithms.
 */
#include "../include/interpolation.h"
#include <stdio.h> // For error reporting (fprintf, stderr)
#include <stdlib.h> // For dynamic memory allocation (malloc, free)
#include <math.h>  // For fabs, isnan, isinf

/**
 * @brief Performs Lagrange interpolation.
 *
 * Calculates the interpolated value P(x) at a point 'x' using the formula:
 * P(x) = ∑ (i = 0 to n - 1) [ y_i * L_i(x) ]
 * where L_i(x) is the i-th Lagrange basis polynomial:
 * L_i(x) = ∏ (j = 0 to n - 1, j != i) [ (x - x_j) / (x_i - x_j) ]
 *
 * @param x The point at which to evaluate the interpolation.
 * @param nodes Array of x-coordinates (x_i) of the interpolation nodes.
 * @param values Array of y-coordinates (y_i) (function values) at the nodes.
 * @param n The number of interpolation nodes (degree of polynomial is n-1).
 * @return The interpolated value P(x) at point 'x'.
 */
double lagrangeInterpolation(double x, double nodes[], double values[], int n) {
    double result = 0.0; // Initialize the sum for P(x)

    // Iterate through each node i to calculate the term y_i * L_i(x)
    for (int i = 0; i < n; i++) {
        // Calculate the i-th Lagrange basis polynomial L_i(x)
        double Li = 1.0; // Initialize the product for L_i(x)
        // Iterate through each node j to compute the product terms
        for (int j = 0; j < n; j++) {
            if (i != j) { // Exclude the term where j equals i
                // Calculate the denominator (x_i - x_j)
                double denom = nodes[i] - nodes[j];
                // Check for potential division by zero (if nodes are too close or identical)
                // This should ideally not happen if nodes are generated correctly.
                if (fabs(denom) < 1e-15) { // Use a small tolerance for floating point comparison
                    fprintf(stderr, "Warning: Potential division by zero or very close nodes in Lagrange (nodes[%d]=%g, nodes[%d]=%g). Result might be inaccurate.\n", i, nodes[i], j, nodes[j]);
                    // If x is exactly equal to nodes[j], the numerator (x - nodes[j]) will be zero,
                    // handling the case where the point x coincides with another node.
                    // However, if nodes[i] == nodes[j], this indicates an issue with node generation.
                    // Returning NaN or a specific error code might be more robust depending on requirements.
                }
                // Multiply by the term (x - x_j) / (x_i - x_j)
                 // Check denominator again before division to avoid NaN/Inf propagation if warning occurred
                if (fabs(denom) >= 1e-15) {
                    Li *= (x - nodes[j]) / denom;
                } else if (fabs(x - nodes[j]) < 1e-15) {
                     // If x is very close to nodes[j] AND nodes[i] is very close to nodes[j]
                     // This is a tricky case. If x == nodes[j] == nodes[i], the basis polynomial definition
                     // implies L_i(x) should be 1 if x=nodes[i] and 0 if x=nodes[j] (j!=i).
                     // If nodes are distinct, this case is fine. If nodes are identical, it's an error.
                     // If x == nodes[j] but != nodes[i], Li should become 0.
                     Li = 0.0; // Effectively handles the x == nodes[j] case when denom is near zero.
                     break; // No need to multiply further if Li becomes zero
                 } else {
                     // Denominator is near zero, but x is not near nodes[j]. This implies potential Inf.
                     // This signals a problem, likely identical nodes.
                     Li = NAN; // Propagate error
                     break;
                 }
            }
        }
        // Add the term y_i * L_i(x) to the total sum
        if (isnan(Li)) {
             result = NAN; // Propagate NaN if basis polynomial calculation failed
             break;
        }
        result += values[i] * Li;
    }

    return result; // Return the final interpolated value
}

/**
 * @brief Performs Newton interpolation using divided differences.
 *
 * Calculates the interpolated value P(x) at point 'x' using the Newton polynomial:
 * P(x) = f[x_0] + f[x_0, x_1](x-x_0) + f[x_0, x_1, x_2](x-x_0)(x-x_1) + ...
 * where f[x_0, ..., x_k] are the divided differences.
 * The divided differences are calculated iteratively:
 * f[x_i] = y_i
 * f[x_i, ..., x_{i+j}] = (f[x_{i+1}, ..., x_{i+j}] - f[x_i, ..., x_{i+j-1}]) / (x_{i+j} - x_i)
 *
 * @param x The point at which to evaluate the interpolation.
 * @param nodes Array of x-coordinates (x_i) of the interpolation nodes.
 * @param values Array of y-coordinates (y_i) (function values) at the nodes.
 * @param n The number of interpolation nodes.
 * @return The interpolated value P(x) at point 'x'. Returns NAN if n < 1 or if
 *         division by zero occurs during divided difference calculation.
 */
double newtonInterpolation(double x, double nodes[], double values[], int n) {
    // Check for invalid number of nodes (must have at least one node)
    if (n < 1) {
         fprintf(stderr, "Error: Newton interpolation called with n=%d (must be >= 1)\n", n);
         return NAN; // Return Not-a-Number for invalid input
    }

    // Check if n exceeds the statically allocated size (if using static array)
    if (n > MAX_NODES) {
         fprintf(stderr, "Error: Newton interpolation called with n=%d > MAX_NODES=%d\n", n, MAX_NODES);
         return NAN; // Return Not-a-Number
    }

    // Allocate memory for the divided difference table.
    // Using a static 2D array (VLA `double divDiff[n][n]` could be used but depends on compiler/stack).
    // Size is MAX_NODES x MAX_NODES, but only n x n part is used.
    double divDiff[MAX_NODES][MAX_NODES];

    // Initialize the first column of the divided difference table (j=0)
    // f[x_i] = y_i
    for (int i = 0; i < n; i++) {
        divDiff[i][0] = values[i];
    }

    // Calculate the divided differences for columns j = 1 to n-1
    for (int j = 1; j < n; j++) { // Column index
        for (int i = 0; i < n - j; i++) { // Row index (starts from 0 up to n-j-1)
             // Calculate the denominator (x_{i+j} - x_i)
             double denom = nodes[i+j] - nodes[i];
             // Check for potential division by zero (identical or very close nodes)
             if (fabs(denom) < 1e-15) {
                 fprintf(stderr, "Warning: Potential division by zero in Newton divided differences (nodes[%d]=%g, nodes[%d]=%g). Result might be inaccurate.\n", i+j, nodes[i+j], i, nodes[i]);
                 // If denominator is zero, the divided difference is undefined. Assign NaN.
                 divDiff[i][j] = NAN;
             } else {
                 // Calculate the divided difference using the formula
                 divDiff[i][j] = (divDiff[i+1][j-1] - divDiff[i][j-1]) / denom;
             }
        }
    }

    // Evaluate the Newton polynomial P(x)
    // P(x) = divDiff[0][0] + divDiff[0][1]*(x-x0) + divDiff[0][2]*(x-x0)*(x-x1) + ...
    // divDiff[0][i] corresponds to f[x_0, ..., x_i]

    // Initialize result with the first term (f[x_0])
    double result = divDiff[0][0];
    // Initialize product term (starts as 1 for the first term, then becomes (x-x0), then (x-x0)(x-x1), etc.)
    double prod = 1.0;

    // Iterate through the remaining terms of the polynomial (i = 1 to n-1)
    for (int i = 1; i < n; i++) {
        // Update the product term: prod = prod * (x - x_{i-1})
        prod *= (x - nodes[i-1]);

        // Check if the required divided difference is valid (not NaN) before using it
        if (isnan(divDiff[0][i])) {
             fprintf(stderr, "Warning: Using NaN divided difference (term %d) in Newton polynomial calculation. Final result will be NaN.\n", i);
             result = NAN; // Propagate NaN if a coefficient is NaN
             break; // Stop calculation if NaN is encountered
        }

        // Add the next term: f[x_0, ..., x_i] * prod
        result += divDiff[0][i] * prod;
    }

    // Final check if the result itself became NaN or Inf due to large intermediate values
    if (isnan(result) || isinf(result)) {
         fprintf(stderr, "Warning: Newton interpolation result is %f for x=%g with n=%d nodes. This might indicate numerical instability or issues with nodes/function values.\n", result, x, n);
    }

    return result; // Return the final interpolated value
}