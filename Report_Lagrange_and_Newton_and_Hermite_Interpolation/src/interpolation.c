/**
 * @file interpolation.c
 * @brief Implementation of Lagrange, Newton and Hermite interpolation algorithms.
 */
#include "../include/interpolation.h"
#include <stdio.h> // For error reporting (fprintf, stderr)
#include <stdlib.h> // For dynamic memory allocation (malloc, free)
#include <math.h>  // For fabs, NAN, isnan, isinf
// #include <string.h> // For memset (optional, if initializing fz_flat to zero was desired)

/**
 * @brief Performs Lagrange interpolation.
 *
 * Calculates the interpolated value P(x) at a point 'x' using the formula:
 * P(x) = sum_{i=0}^{n-1} [ y_i * L_i(x) ]
 * where L_i(x) is the i-th Lagrange basis polynomial:
 * L_i(x) = product_{j=0, j!=i}^{n-1} [ (x - x_j) / (x_i - x_j) ]
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

/**
 * @brief Performs Hermite interpolation using divided differences.
 *
 * Calculates the interpolated value P(x) at point 'x' using the Hermite polynomial.
 * The Hermite polynomial matches both function values f(x_i) and first derivative
 * values f'(x_i) at n distinct nodes x_0, ..., x_{n-1}.
 * The degree of the resulting polynomial is 2n - 1.
 *
 * This implementation uses a generalized divided difference table approach.
 * A sequence z_0, z_1, ..., z_{2n-1} is constructed where each node x_i appears twice:
 * z_0 = x_0, z_1 = x_0, z_2 = x_1, z_3 = x_1, ..., z_{2n-2} = x_{n-1}, z_{2n-1} = x_{n-1}.
 *
 * The divided differences f[z_i, ..., z_{i+j}] are calculated:
 * - f[z_i] = f(x_k) if z_i corresponds to x_k.
 * - f[z_i, z_{i+1}] = f'(x_k) if z_i = z_{i+1} = x_k.
 * - f[z_i, ..., z_{i+j}] = (f[z_{i+1}, ..., z_{i+j}] - f[z_i, ..., z_{i+j-1}]) / (z_{i+j} - z_i) if z_{i+j} != z_i.
 *
 * The Hermite polynomial is then:
 * P(x) = f[z_0] + f[z_0, z_1](x-z_0) + f[z_0, z_1, z_2](x-z_0)(x-z_1) + ... + f[z_0, ..., z_{2n-1}] * product_{k=0}^{2n-2} (x-z_k)
 *
 * Due to the potentially large size of the divided difference table (2n x 2n),
 * dynamic memory allocation is used for it.
 *
 * @param x The point at which to evaluate the interpolation.
 * @param nodes Array of x-coordinates of the distinct interpolation nodes (size n).
 * @param values Array of y-coordinates (function values f(x_i)) at the nodes (size n).
 * @param derivatives Array of first derivative values (f'(x_i)) at the nodes (size n).
 * @param n The number of *distinct* interpolation nodes.
 * @return The interpolated value P(x) at point 'x'. Returns NAN on error.
 */
double hermiteInterpolation(double x, double nodes[], double values[], double derivatives[], int n) {
    // Total number of interpolation conditions (n function values + n derivative values)
    int m = 2 * n;
    // If no nodes (n=0), return 0 or handle as appropriate.
    if (m == 0) return 0.0; // Or perhaps NAN if n=0 is invalid input

    // Check if the number of nodes exceeds the limit (if MAX_NODES applies here too)
    if (n > MAX_NODES) {
         fprintf(stderr, "Error: Too many distinct nodes for Hermite interpolation (%d > MAX_NODES=%d)\n", n, MAX_NODES);
         return NAN;
    }

    // Create the array 'z' of duplicated nodes on the stack (size 2n).
    // Ensure MAX_NODES*2 doesn't cause stack overflow; consider dynamic allocation if n can be very large.
    double z[MAX_NODES * 2]; // Needs to hold 2*n elements

    // Dynamically allocate memory for the divided difference table 'fz'.
    // The table size is m x m (or 2n x 2n).
    // We allocate a flat 1D array and use a macro for 2D indexing.
    size_t table_size = (size_t)m * (size_t)m; // Use size_t for potentially large allocations
    double *fz_flat = (double*)malloc(table_size * sizeof(double));
    if (fz_flat == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for Hermite divided differences table (size %d x %d)\n", m, m);
        return NAN; // Return Not-a-Number if allocation fails
    }

    // --- Define a macro for convenient 2D access to the flat array 'fz_flat' ---
    // FZ(row, col) maps to the correct index in the 1D array.
    // This macro is only visible within this function scope after this point.
    #define FZ(row, col) fz_flat[(size_t)(row) * m + (size_t)(col)]

    // --- Step 1: Prepare the 'z' array and initialize the first column (j=0) of 'fz' ---
    // Populate 'z' by duplicating each node x_i.
    // Initialize FZ[i][0] with the corresponding function value f(x_k).
    for (int i = 0; i < n; i++) {
        z[2*i]     = nodes[i];      // z_0, z_2, z_4, ...
        z[2*i+1]   = nodes[i];      // z_1, z_3, z_5, ...
        FZ(2*i, 0)   = values[i];   // FZ[0][0], FZ[2][0], ... = f(x_0), f(x_1), ...
        FZ(2*i+1, 0) = values[i];   // FZ[1][0], FZ[3][0], ... = f(x_0), f(x_1), ...
    }

    // --- Step 2: Calculate the divided differences ---
    // Iterate through columns j = 1 to m-1
    for (int j = 1; j < m; j++) {
        // Iterate through rows i = 0 to m-j-1
        for (int i = 0; i < m - j; i++) {
            // Calculate the denominator z[i+j] - z[i]
            double denominator = z[i+j] - z[i];

            // Check if the denominator is close to zero (i.e., z[i+j] == z[i])
            if (fabs(denominator) < 1e-15) {
                 // Special case for Hermite: If z[i+j] == z[i], this occurs when j=1 and i is even.
                 // In this case, the divided difference f[z_i, z_{i+1}] is defined as f'(x_k), where z_i=z_{i+1}=x_k.
                 if (j == 1 && (i % 2 == 0)) {
                     // Check array bounds for the derivatives array
                     if (i / 2 < n) {
                        // Assign the derivative value
                        FZ(i, j) = derivatives[i / 2]; // FZ[0][1]=f'(x0), FZ[2][1]=f'(x1), ...
                     } else {
                         // Index out of bounds error, should not happen if loops are correct
                         fprintf(stderr, "Error: Index out of bounds accessing derivatives array at i=%d (n=%d)\n", i, n);
                         FZ(i, j) = NAN; // Assign NaN on error
                     }
                } else {
                    // If denominator is zero for j > 1 or odd i when j=1, it indicates an issue
                    // (possibly identical distinct nodes passed in, or numerical instability).
                    // Standard divided difference is undefined. Assigning 0 or NaN can be debated.
                    // Let's use 0 and issue a warning, as NaN might propagate too aggressively.
                    // Using 0 effectively assumes higher-order derivatives are zero in this problematic case.
                    fprintf(stderr, "Warning: Near-zero denominator encountered in Hermite DD calculation at i=%d, j=%d where it is not expected f'(x_k). Using 0.0.\n", i, j);
                    FZ(i, j) = 0.0; // Or potentially NAN
                }
            } else {
                 // Standard divided difference calculation: (f[z_{i+1},..] - f[z_i,..]) / (z_{i+j} - z_i)
                 // Check if the terms used in the numerator are valid (not NaN)
                 if(isnan(FZ(i+1, j-1)) || isnan(FZ(i, j-1))) {
                     FZ(i, j) = NAN; // Propagate NaN if inputs are invalid
                 } else {
                     FZ(i, j) = (FZ(i+1, j-1) - FZ(i, j-1)) / denominator;
                 }
            }
        }
    }

    // --- Step 3: Evaluate the Hermite polynomial using the computed divided differences ---
    // P(x) = FZ[0][0] + FZ[0][1]*(x-z0) + FZ[0][2]*(x-z0)*(x-z1) + ...
    double result = 0.0;
    // Check if the first term FZ[0][0] is valid before starting
    if (m > 0) {
       if (isnan(FZ(0, 0))) {
           fprintf(stderr, "Error: First divided difference FZ[0][0] is NaN.\n");
           free(fz_flat); // Free allocated memory before returning
           return NAN;
       }
       result = FZ(0, 0); // Initialize result with the first term f[z_0]
    }

    // Initialize the product term (x-z_0)(x-z_1)...(x-z_{k-1})
    double product_term = 1.0;

    // Iterate through the remaining terms of the polynomial (k = 1 to m-1)
    for (int k = 1; k < m; k++) {
        // Update the product term: multiply by (x - z_{k-1})
        product_term *= (x - z[k-1]);

        // Check if the divided difference FZ[0][k] or the product term is NaN
        if (isnan(FZ(0, k)) || isnan(product_term)) {
             fprintf(stderr, "Warning: NaN encountered during Hermite polynomial evaluation at term k=%d.\n", k);
             result = NAN; // Propagate NaN
             break;        // Stop evaluation if NaN occurs
        }

        // Add the next term: FZ[0][k] * product_term
        result += FZ(0, k) * product_term;

        // Check if the result itself became NaN (e.g., due to Inf * 0)
        if (isnan(result)) {
            fprintf(stderr, "Warning: Result became NaN during Hermite polynomial evaluation sum at term k=%d.\n", k);
            break;
        }
    }

    // --- Cleanup: Free the dynamically allocated memory ---
    free(fz_flat);

    // --- Undefine the macro to limit its scope (good practice) ---
    // Although its scope is limited to this function anyway after C99,
    // explicitly undefining can prevent potential issues if the file structure changes.
    #undef FZ

    // Final check on the result before returning
    if (isnan(result) || isinf(result)) {
         fprintf(stderr, "Warning: Hermite interpolation final result is %f for x=%g with n=%d nodes.\n", result, x, n);
    }

    return result; // Return the final interpolated value
}