/**
 * @file main.c
 * @brief Main program for performing and analyzing various spline interpolations.
 *
 * This program compares different types of spline interpolation:
 * - Cubic Spline with Natural Boundary Conditions
 * - Cubic Spline with Clamped Boundary Conditions (using true function derivatives)
 * - Quadratic Spline with Clamped Start Boundary Condition (using true function derivative at 'a')
 * - Quadratic Spline with Zero Start Derivative Boundary Condition
 *
 * The analysis is performed using both uniformly spaced nodes and Chebyshev nodes
 * for the function f(x) = sin(k*x/PI) * exp(-m*x/PI) over the interval [a, b].
 *
 * It iterates through the number of interpolation nodes (n) from 2 up to a user-specified maximum.
 * For each 'n', node type, and spline type, it:
 * 1. Generates interpolation nodes and calculates function values.
 * 2. Performs the spline interpolation, evaluating it on a dense grid of points.
 * 3. Calculates the maximum absolute error and mean squared error (MSE) compared to the true function.
 * 4. Saves the original function data, node data, and interpolated curve data to `.dat` files in the `data/` directory.
 * 5. Prints the errors for the current step.
 *
 * After iterating through all 'n', it:
 * 6. Saves the collected error trends (max error and MSE vs. n) for all 8 combinations (4 spline types x 2 node types) into separate `.csv` files in the `data/` directory.
 * 7. Generates Gnuplot scripts (`.gp` files) in the `scripts/` directory:
 *    - `plot_spline_interpolation.gp`: To create individual plots for each interpolation case, showing the original function, the spline, and the nodes. Output plots are saved to `plots/`.
 *    - `plot_spline_errors.gp`: To create a single plot comparing the maximum absolute errors of all methods versus the number of nodes. Output plot is saved to `plots/`.
 * 8. Prints instructions on how to generate the plots using the Gnuplot scripts or a Makefile target.
 */
#include "../include/common.h"      // Common definitions (constants, types like BoundaryCondition)
#include "../include/function.h"    // Function f(x) and its derivative df(x)
#include "../include/nodes.h"       // Node generation (uniform, Chebyshev)
#include "../include/interpolation.h" // Spline interpolation algorithms
#include "../include/error.h"       // Error calculation (max, MSE) and ErrorResult struct
#include "../include/fileio.h"      // File I/O and Gnuplot script generation
#include <stdio.h>                 // Standard I/O for printf, scanf, fprintf
#include <string.h>                // For sprintf

/**
 * @brief Main entry point of the spline interpolation analysis program.
 *
 * Orchestrates the entire process: user input, data generation,
 * looping through node counts, performing interpolations, calculating errors,
 * saving results, and generating plot scripts.
 *
 * @return 0 on successful execution.
 * @return 1 on error (e.g., invalid user input for the number of nodes).
 */
int main() {
    int maxNodes; // Variable to store the maximum number of nodes from user input

    // Prompt user for the maximum number of nodes (minimum 2 for splines)
    printf("Enter the maximum number of interpolation nodes (2-%d): ", MAX_NODES);
    if (scanf("%d", &maxNodes) != 1) { // Validate input format
        fprintf(stderr, "Error reading the number of nodes.\n");
        return 1; // Exit with error code
    }
    // Validate input range
    if (maxNodes < 2 || maxNodes > MAX_NODES) {
        fprintf(stderr, "Invalid number of nodes. Must be between 2 and %d\n", MAX_NODES);
        return 1; // Exit with error code
    }

    // --- Setup for Plotting Original Function ---
    const int numPoints = 1000; // Number of points for smooth plotting
    double x_plot[numPoints];   // x-coordinates for plotting
    double y_true[numPoints];   // True function values f(x) at plot points

    // Generate a dense grid for plotting the original function
    double plot_step = (b - a) / (numPoints - 1.0);
    for (int i = 0; i < numPoints; i++) {
        x_plot[i] = a + i * plot_step;
        y_true[i] = f(x_plot[i]); // Calculate f(x) at each plot point
    }
     // Ensure the last point is exactly 'b' to cover the interval perfectly
    if (numPoints > 1) x_plot[numPoints - 1] = b;
    // Save the original function data
    saveDataToFile("original_function.dat", x_plot, y_true, numPoints);

    // --- Calculate True Derivatives for Clamped Boundary Conditions ---
    double true_deriv_a = df(a); // True derivative at the start of the interval
    double true_deriv_b = df(b); // True derivative at the end of the interval
    printf("\nTrue function derivative at a=%.4f: f'(a) = %.6f\n", a, true_deriv_a);
    printf("True function derivative at b=%.4f: f'(b) = %.6f\n", b, true_deriv_b);

    // --- Allocate Arrays to Store Error Results ---
    // Size is maxNodes-1 because loop runs from n=2 to maxNodes
    int error_array_size = maxNodes - 1;
    // Max Absolute Errors (8 types)
    double errors_cubic_natural_uniform[error_array_size];
    double errors_cubic_clamped_uniform[error_array_size];
    double errors_quad_clamped_uniform[error_array_size];
    double errors_quad_zero_start_uniform[error_array_size];
    double errors_cubic_natural_chebyshev[error_array_size];
    double errors_cubic_clamped_chebyshev[error_array_size];
    double errors_quad_clamped_chebyshev[error_array_size];
    double errors_quad_zero_start_chebyshev[error_array_size];
    // Mean Squared Errors (8 types)
    double mse_cubic_natural_uniform[error_array_size];
    double mse_cubic_clamped_uniform[error_array_size];
    double mse_quad_clamped_uniform[error_array_size];
    double mse_quad_zero_start_uniform[error_array_size];
    double mse_cubic_natural_chebyshev[error_array_size];
    double mse_cubic_clamped_chebyshev[error_array_size];
    double mse_quad_clamped_chebyshev[error_array_size];
    double mse_quad_zero_start_chebyshev[error_array_size];


    // --- Temporary Arrays for Calculations within the Loop ---
    double nodes[MAX_NODES];    // Stores x-coordinates of interpolation nodes for current 'n'
    double values[MAX_NODES];   // Stores y-coordinates (f(nodes)) for current 'n'
    double y_interp[numPoints]; // Stores interpolated y-values at plotting points 'x_plot'

    // --- Print Header Information ---
    printf("\nStarting Spline Interpolation Analysis for n = 2 to %d nodes...\n", maxNodes);
    printf("Interpolating f(x) = sin(%.1fx/pi) * exp(-%.1fx/pi) on [%.2f, %.2f]\n", k, m, a, b);
    printf("Comparing Cubic (Natural, Clamped) and Quadratic (Clamped Start, Zero Start Deriv.) Splines.\n");
    printf("Using both Uniform and Chebyshev nodes.\n");
    printf("=========================================================================\n");

    // --- Main Loop: Iterate through different numbers of nodes 'n' ---
    for (int n = 2; n <= maxNodes; n++) {
        char filename[150]; // Buffer for constructing filenames
        int error_index = n - 2; // Index for error arrays (0 corresponds to n=2)

        printf("\n--- Processing for n = %d nodes ---\n", n);

        // ========================================
        // === Part 1: Uniformly Spaced Nodes ===
        // ========================================
        printf("--> Uniform Nodes <--\n");

        // Generate n uniform nodes and their corresponding f(x) values
        uniformNodes(nodes, n);
        for (int j = 0; j < n; j++) values[j] = f(nodes[j]);
        // Save the nodes and values to a file
        sprintf(filename, "uniform_nodes_n%d.dat", n);
        saveNodesToFile(filename, nodes, values, n);

        // --- 1.1 Cubic Spline - Natural Boundary Conditions (Uniform Nodes) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = cubicSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_NATURAL, 0, 0); // Boundary values ignored
        sprintf(filename, "cubic_natural_uniform_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints); // Save interpolated curve
        ErrorResult err_cn_u = calculateError(y_true, y_interp, numPoints); // Calculate errors
        errors_cubic_natural_uniform[error_index] = err_cn_u.max_error;
        mse_cubic_natural_uniform[error_index] = err_cn_u.mean_squared_error;
        printf("  Cubic Natural (Uniform):        MaxErr=%.3e, MSE=%.3e\n", err_cn_u.max_error, err_cn_u.mean_squared_error);

        // --- 1.2 Cubic Spline - Clamped Boundary Conditions (Uniform Nodes) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = cubicSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_CLAMPED, true_deriv_a, true_deriv_b); // Use true derivatives
        sprintf(filename, "cubic_clamped_uniform_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_cc_u = calculateError(y_true, y_interp, numPoints);
        errors_cubic_clamped_uniform[error_index] = err_cc_u.max_error;
        mse_cubic_clamped_uniform[error_index] = err_cc_u.mean_squared_error;
        printf("  Cubic Clamped (Uniform):        MaxErr=%.3e, MSE=%.3e\n", err_cc_u.max_error, err_cc_u.mean_squared_error);

        // --- 1.3 Quadratic Spline - Clamped Start Boundary Condition (Uniform Nodes) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = quadraticSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_CLAMPED, true_deriv_a); // Use true derivative at 'a'
        sprintf(filename, "quadratic_clamped_uniform_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_qc_u = calculateError(y_true, y_interp, numPoints);
        errors_quad_clamped_uniform[error_index] = err_qc_u.max_error;
        mse_quad_clamped_uniform[error_index] = err_qc_u.mean_squared_error;
        printf("  Quadratic Clamped (Uniform):    MaxErr=%.3e, MSE=%.3e\n", err_qc_u.max_error, err_qc_u.mean_squared_error);

        // --- 1.4 Quadratic Spline - Zero Slope Start Boundary Condition (Uniform Nodes) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = quadraticSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_ZERO_SLOPE_START, 0); // Use derivative = 0 at 'a'
        sprintf(filename, "quadratic_zero_start_uniform_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_qz_u = calculateError(y_true, y_interp, numPoints);
        errors_quad_zero_start_uniform[error_index] = err_qz_u.max_error;
        mse_quad_zero_start_uniform[error_index] = err_qz_u.mean_squared_error;
        printf("  Quadratic Zero St (Uniform):    MaxErr=%.3e, MSE=%.3e\n", err_qz_u.max_error, err_qz_u.mean_squared_error);

        // =====================================
        // === Part 2: Chebyshev Nodes       ===
        // =====================================
        printf("--> Chebyshev Nodes <--\n");

        // Generate n Chebyshev nodes and their corresponding f(x) values
        chebyshevNodes(nodes, n);
        for (int j = 0; j < n; j++) values[j] = f(nodes[j]);
        // Save the nodes and values
        sprintf(filename, "chebyshev_nodes_n%d.dat", n);
        saveNodesToFile(filename, nodes, values, n);

        // --- 2.1 Cubic Spline - Natural Boundary Conditions (Chebyshev Nodes) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = cubicSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_NATURAL, 0, 0);
        sprintf(filename, "cubic_natural_chebyshev_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_cn_c = calculateError(y_true, y_interp, numPoints);
        errors_cubic_natural_chebyshev[error_index] = err_cn_c.max_error;
        mse_cubic_natural_chebyshev[error_index] = err_cn_c.mean_squared_error;
        printf("  Cubic Natural (Chebyshev):      MaxErr=%.3e, MSE=%.3e\n", err_cn_c.max_error, err_cn_c.mean_squared_error);

        // --- 2.2 Cubic Spline - Clamped Boundary Conditions (Chebyshev Nodes) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = cubicSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_CLAMPED, true_deriv_a, true_deriv_b);
        sprintf(filename, "cubic_clamped_chebyshev_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_cc_c = calculateError(y_true, y_interp, numPoints);
        errors_cubic_clamped_chebyshev[error_index] = err_cc_c.max_error;
        mse_cubic_clamped_chebyshev[error_index] = err_cc_c.mean_squared_error;
        printf("  Cubic Clamped (Chebyshev):      MaxErr=%.3e, MSE=%.3e\n", err_cc_c.max_error, err_cc_c.mean_squared_error);

        // --- 2.3 Quadratic Spline - Clamped Start Boundary Condition (Chebyshev Nodes) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = quadraticSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_CLAMPED, true_deriv_a);
        sprintf(filename, "quadratic_clamped_chebyshev_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_qc_c = calculateError(y_true, y_interp, numPoints);
        errors_quad_clamped_chebyshev[error_index] = err_qc_c.max_error;
        mse_quad_clamped_chebyshev[error_index] = err_qc_c.mean_squared_error;
        printf("  Quadratic Clamped (Chebyshev):  MaxErr=%.3e, MSE=%.3e\n", err_qc_c.max_error, err_qc_c.mean_squared_error);

        // --- 2.4 Quadratic Spline - Zero Slope Start Boundary Condition (Chebyshev Nodes) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = quadraticSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_ZERO_SLOPE_START, 0);
        sprintf(filename, "quadratic_zero_start_chebyshev_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_qz_c = calculateError(y_true, y_interp, numPoints);
        errors_quad_zero_start_chebyshev[error_index] = err_qz_c.max_error;
        mse_quad_zero_start_chebyshev[error_index] = err_qz_c.mean_squared_error;
        printf("  Quadratic Zero St (Chebyshev):  MaxErr=%.3e, MSE=%.3e\n", err_qz_c.max_error, err_qz_c.mean_squared_error);

    } // End of loop over n

    // --- Post-Loop Processing ---
    printf("\n=========================================================================\n");
    printf("Calculations complete. Saving error summaries...\n");

    // Save collected error data (Max Error and MSE vs. n) to separate CSV files
    saveSplineErrorsToFile("cubic_natural_uniform_errors", maxNodes, errors_cubic_natural_uniform, mse_cubic_natural_uniform);
    saveSplineErrorsToFile("cubic_clamped_uniform_errors", maxNodes, errors_cubic_clamped_uniform, mse_cubic_clamped_uniform);
    saveSplineErrorsToFile("quadratic_clamped_uniform_errors", maxNodes, errors_quad_clamped_uniform, mse_quad_clamped_uniform);
    saveSplineErrorsToFile("quadratic_zero_start_uniform_errors", maxNodes, errors_quad_zero_start_uniform, mse_quad_zero_start_uniform);
    saveSplineErrorsToFile("cubic_natural_chebyshev_errors", maxNodes, errors_cubic_natural_chebyshev, mse_cubic_natural_chebyshev);
    saveSplineErrorsToFile("cubic_clamped_chebyshev_errors", maxNodes, errors_cubic_clamped_chebyshev, mse_cubic_clamped_chebyshev);
    saveSplineErrorsToFile("quadratic_clamped_chebyshev_errors", maxNodes, errors_quad_clamped_chebyshev, mse_quad_clamped_chebyshev);
    saveSplineErrorsToFile("quadratic_zero_start_chebyshev_errors", maxNodes, errors_quad_zero_start_chebyshev, mse_quad_zero_start_chebyshev);
    printf("Error summaries saved to data/*.csv\n");

    // Generate Gnuplot scripts for visualization
    printf("Generating Gnuplot scripts...\n");
    // Script for plotting individual interpolation results (one PNG per case)
    generateSplineGnuplotScript(maxNodes);
    // Script for plotting the comparison of maximum errors vs. number of nodes
    generateSplineErrorPlotScript(maxNodes,
                                  errors_cubic_natural_uniform, errors_cubic_clamped_uniform,
                                  errors_quad_clamped_uniform, errors_quad_zero_start_uniform,
                                  errors_cubic_natural_chebyshev, errors_cubic_clamped_chebyshev,
                                  errors_quad_clamped_chebyshev, errors_quad_zero_start_chebyshev);

    // --- Final Output and Instructions ---
    printf("\n=========================================================================\n");
    printf("Analysis complete.\n");
    printf("Data files (.dat) saved in: data/\n");
    printf("CSV error summaries (.csv) saved in: data/\n");
    printf("Gnuplot scripts (.gp) saved in: scripts/\n");
    printf("\nTo generate the plots, navigate to the project root directory and run:\n");
    printf("  gnuplot scripts/plot_spline_interpolation.gp\n");
    printf("  gnuplot scripts/plot_spline_errors.gp\n");
    printf("Alternatively, use the Makefile target:\n");
    printf("  make plots\n");
    printf("Generated plots (.png files) will be saved in: plots/\n");


    return 0; // Indicate successful execution
}