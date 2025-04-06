/**
 * @file main.c
 * @brief Main program for performing and analyzing polynomial interpolation.
 *
 * This program compares Lagrange and Newton interpolation methods using both
 * uniformly spaced nodes and Chebyshev nodes for the function
 * f(x) = sin(k*x/PI) * exp(-m*x/PI) over the interval [a, b].
 * It calculates interpolation results for varying numbers of nodes (n),
 * computes the maximum absolute error and mean squared error for each case,
 * saves the results and errors to data files (.dat, .csv), and generates
 * Gnuplot scripts (.gp) to visualize the interpolated functions and the error trends.
 */
#include "../include/common.h"      // Common definitions (constants, includes)
#include "../include/function.h"    // Function f(x) to interpolate
#include "../include/nodes.h"       // Node generation (uniform, Chebyshev)
#include "../include/interpolation.h" // Interpolation algorithms (Lagrange, Newton)
#include "../include/error.h"       // Error calculation
#include "../include/fileio.h"      // File I/O and Gnuplot script generation
#include <stdio.h>                 // Standard I/O for printf, scanf

/**
 * @brief Main entry point of the program.
 *
 * 1. Prompts the user for the maximum number of nodes.
 * 2. Generates data points for the original function f(x).
 * 3. Iterates from n=1 to maxNodes:
 *    a. Generates uniform nodes and calculates f(x) at these nodes.
 *    b. Performs Lagrange and Newton interpolation using uniform nodes.
 *    c. Calculates errors for uniform nodes.
 *    d. Generates Chebyshev nodes and calculates f(x) at these nodes.
 *    e. Performs Lagrange and Newton interpolation using Chebyshev nodes.
 *    f. Calculates errors for Chebyshev nodes.
 *    g. Saves node data and interpolation results to .dat files.
 *    h. Prints error summary periodically.
 * 4. Saves error trends (max error, MSE vs. n) to .csv files.
 * 5. Generates Gnuplot scripts for visualizing results and errors.
 * 6. Prints instructions for generating plots using Gnuplot.
 *
 * @return 0 on successful execution, 1 on error (e.g., invalid input).
 */
int main() {
    int maxNodes; // Variable to store the maximum number of nodes from user input

    // Prompt user for the maximum number of nodes and validate input
    printf("Enter the maximum number of interpolation nodes (1-%d): ", MAX_NODES);
    if (scanf("%d", &maxNodes) != 1) { // Check if scanf successfully read an integer
        printf("Error reading the number of nodes.\n");
        return 1; // Exit with error code
    }

    // Validate the range of the entered number of nodes
    if (maxNodes < 1 || maxNodes > MAX_NODES) {
        printf("Invalid number of nodes. Must be between 1 and %d\n", MAX_NODES);
        return 1; // Exit with error code
    }

    // Define the number of points for plotting the continuous functions (original and interpolated)
    const int numPoints = 1000;
    // Arrays to store x and y coordinates for plotting
    double x[numPoints];        // x-coordinates for plotting
    double y_true[numPoints];   // True function values f(x) at plotting points

    // Generate a dense grid of points for visualizing the original function smoothly
    double plot_step = (b - a) / (numPoints - 1.0); // Calculate step size for plotting points
    for (int i = 0; i < numPoints; i++) {
        x[i] = a + i * plot_step; // Calculate x-coordinate
        y_true[i] = f(x[i]);      // Calculate true function value f(x)
    }
    // Ensure the last plotting point is exactly 'b'
     if (numPoints > 1) x[numPoints-1] = b;


    // Save the original function data points to a file for plotting
    saveDataToFile("original_function.dat", x, y_true, numPoints);

    // --- Arrays to Store Error Results ---
    // Store maximum absolute error for each method and node type, for n=1 to maxNodes
    double lagrange_uniform_errors[maxNodes];
    double lagrange_chebyshev_errors[maxNodes];
    double newton_uniform_errors[maxNodes];
    double newton_chebyshev_errors[maxNodes];

    // Store mean squared error (MSE) for each method and node type, for n=1 to maxNodes
    double lagrange_uniform_mse[maxNodes];
    double lagrange_chebyshev_mse[maxNodes];
    double newton_uniform_mse[maxNodes];
    double newton_chebyshev_mse[maxNodes];

    // --- Temporary Arrays for Calculations within the loop ---
    // Max size needed is maxNodes
    double nodes[MAX_NODES];   // Stores x-coordinates of interpolation nodes for current 'n'
    double values[MAX_NODES];  // Stores y-coordinates (f(nodes)) for current 'n'
    // Note: Derivatives are not needed for Lagrange/Newton, only for Hermite interpolation
    // double derivatives[MAX_NODES];

    // Array to store the interpolated y-values at the plotting points 'x'
    double y_interp[numPoints];

    // Print header information about the analysis
    printf("\nInterpolation analysis for f(x) = sin(%.1fx/pi) * exp(-%.1fx/pi) on [%.2f, %.2f]\n",
           k, m, a, b);
    printf("=========================================================================\n");

    // --- Main Loop: Iterate through different numbers of nodes 'n' ---
    for (int n = 1; n <= maxNodes; n++) {
        char filename[100]; // Buffer for constructing filenames

        // --- Part 1: Uniformly Spaced Nodes ---

        // Generate 'n' uniform nodes in the interval [a, b]
        uniformNodes(nodes, n);

        // Calculate the true function values f(x) at these uniform nodes
        for (int j = 0; j < n; j++) {
            values[j] = f(nodes[j]);
        }

        // Save the uniform nodes and their values to a file
        sprintf(filename, "uniform_nodes_n%d.dat", n);
        saveNodesToFile(filename, nodes, values, n);

        // Perform Lagrange interpolation using uniform nodes
        // Evaluate the interpolating polynomial at the dense set of plotting points 'x'
        for (int j = 0; j < numPoints; j++) {
            y_interp[j] = lagrangeInterpolation(x[j], nodes, values, n);
        }
        // Save the interpolated curve data
        sprintf(filename, "lagrange_uniform_n%d.dat", n);
        saveDataToFile(filename, x, y_interp, numPoints);
        // Calculate and store the errors (max and MSE)
        ErrorResult error_lu = calculateError(y_true, y_interp, numPoints);
        lagrange_uniform_errors[n-1] = error_lu.max_error; // Store error for this 'n'
        lagrange_uniform_mse[n-1] = error_lu.mean_squared_error;

        // Perform Newton interpolation using the same uniform nodes
        for (int j = 0; j < numPoints; j++) {
            y_interp[j] = newtonInterpolation(x[j], nodes, values, n);
        }
        // Save the interpolated curve data
        sprintf(filename, "newton_uniform_n%d.dat", n);
        saveDataToFile(filename, x, y_interp, numPoints);
        // Calculate and store the errors (max and MSE)
        ErrorResult error_nu = calculateError(y_true, y_interp, numPoints);
        newton_uniform_errors[n-1] = error_nu.max_error;
        newton_uniform_mse[n-1] = error_nu.mean_squared_error;

        // --- Part 2: Chebyshev Nodes ---

        // Generate 'n' Chebyshev nodes in the interval [a, b]
        chebyshevNodes(nodes, n);

        // Calculate the true function values f(x) at these Chebyshev nodes
        for (int j = 0; j < n; j++) {
            values[j] = f(nodes[j]);
        }

        // Save the Chebyshev nodes and their values to a file
        sprintf(filename, "chebyshev_nodes_n%d.dat", n);
        saveNodesToFile(filename, nodes, values, n);

        // Perform Lagrange interpolation using Chebyshev nodes
        for (int j = 0; j < numPoints; j++) {
            y_interp[j] = lagrangeInterpolation(x[j], nodes, values, n);
        }
        // Save the interpolated curve data
        sprintf(filename, "lagrange_chebyshev_n%d.dat", n);
        saveDataToFile(filename, x, y_interp, numPoints);
        // Calculate and store the errors (max and MSE)
        ErrorResult error_lc = calculateError(y_true, y_interp, numPoints);
        lagrange_chebyshev_errors[n-1] = error_lc.max_error;
        lagrange_chebyshev_mse[n-1] = error_lc.mean_squared_error;

        // Perform Newton interpolation using the same Chebyshev nodes
        for (int j = 0; j < numPoints; j++) {
            y_interp[j] = newtonInterpolation(x[j], nodes, values, n);
        }
        // Save the interpolated curve data
        sprintf(filename, "newton_chebyshev_n%d.dat", n);
        saveDataToFile(filename, x, y_interp, numPoints);
        // Calculate and store the errors (max and MSE)
        ErrorResult error_nc = calculateError(y_true, y_interp, numPoints);
        newton_chebyshev_errors[n-1] = error_nc.max_error;
        newton_chebyshev_mse[n-1] = error_nc.mean_squared_error;

        // --- Print Progress/Results Periodically ---
        // Display errors every 5 steps, for the first step (n=1), and for the last step
        if (n % 5 == 0 || n == maxNodes || n == 1) {
             printf("\nResults for Number of Nodes: %d\n", n);
             printf("-----------------------------------\n");
             printf("Maximum Absolute Errors:\n");
             printf("  Lagrange (Uniform):   %.3e\n", lagrange_uniform_errors[n-1]);
             printf("  Lagrange (Chebyshev): %.3e\n", lagrange_chebyshev_errors[n-1]);
             printf("  Newton (Uniform):     %.3e\n", newton_uniform_errors[n-1]);
             printf("  Newton (Chebyshev):   %.3e\n", newton_chebyshev_errors[n-1]);

             printf("\nMean Squared Errors (MSE):\n");
             printf("  Lagrange (Uniform):   %.3e\n", lagrange_uniform_mse[n-1]);
             printf("  Lagrange (Chebyshev): %.3e\n", lagrange_chebyshev_mse[n-1]);
             printf("  Newton (Uniform):     %.3e\n", newton_uniform_mse[n-1]);
             printf("  Newton (Chebyshev):   %.3e\n", newton_chebyshev_mse[n-1]);
        }
    } // End of loop over 'n'

    // --- Post-Loop Processing ---

    // Save the collected error data (max error and MSE vs. n) to separate CSV files
    saveLagrangeUniformErrorsToFile(maxNodes, lagrange_uniform_errors, lagrange_uniform_mse);
    saveLagrangeChebyshevErrorsToFile(maxNodes, lagrange_chebyshev_errors, lagrange_chebyshev_mse);
    saveNewtonUniformErrorsToFile(maxNodes, newton_uniform_errors, newton_uniform_mse);
    saveNewtonChebyshevErrorsToFile(maxNodes, newton_chebyshev_errors, newton_chebyshev_mse);

    // Generate Gnuplot scripts for visualization
    // Script 1: Plots individual interpolation results for each 'n'
    generateGnuplotScript(maxNodes);

    // Script 2: Plots the summary of maximum errors vs. number of nodes
    generateErrorPlotScript(maxNodes,
                          lagrange_uniform_errors, lagrange_chebyshev_errors,
                          newton_uniform_errors, newton_chebyshev_errors);

    // --- Final Output and Instructions ---
    printf("\n=========================================================================\n");
    printf("Analysis complete.\n");
    printf("Data files saved in the data/ directory.\n");
    printf("Gnuplot scripts saved in the scripts/ directory.\n");
    printf("To generate the plots, navigate to the project root directory and run:\n");
    printf("  gnuplot scripts/plot_interpolation.gp\n");
    printf("  gnuplot scripts/plot_errors.gp\n");
    printf("Alternatively, use the Makefile target:\n");
    printf("  make plots\n");
    printf("Generated plots (.png files) will be saved in the plots/ directory.\n");

    return 0; // Indicate successful execution
}