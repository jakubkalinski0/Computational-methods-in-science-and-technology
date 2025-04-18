/**
* @file main.c
 * @brief Main program for performing and analyzing Hermite polynomial interpolation.
 *
 * This program analyzes Hermite interpolation using both uniformly spaced nodes
 * and Chebyshev nodes for the function f(x) = sin(k*x/PI) * exp(-m*x/PI)
 * and its derivative df(x) over the interval [a, b].
 * It calculates interpolation results for varying numbers of nodes (n),
 * computes the maximum absolute error and mean squared error for each case,
 * saves the results and errors to data files (.dat, likely .csv for errors),
 * and generates Gnuplot scripts (.gp) to visualize the interpolated functions
 * and the error trends.
 */
#include "../include/common.h"      // Common definitions (constants like MAX_NODES, a, b, k, m, PI)
#include "../include/function.h"    // Function f(x) and its derivative df(x)
#include "../include/nodes.h"       // Node generation (uniform, Chebyshev)
#include "../include/interpolation.h" // Interpolation algorithms (Hermite)
#include "../include/error.h"       // Error calculation (max, MSE)
#include "../include/fileio.h"      // File I/O (saving data, nodes, errors) and Gnuplot script generation
#include <stdio.h>                 // Standard I/O for printf, scanf

/**
 * @brief Main entry point of the program.
 *
 * 1. Prompts the user for the maximum number of interpolation nodes (`maxNodes`).
 * 2. Generates a dense set of data points for plotting the original function `f(x)` and saves it.
 * 3. Iterates from n=1 to `maxNodes`:
 *    a. Generates `n` uniform nodes.
 *    b. Calculates `f(x)` and the derivative `df(x)` at these uniform nodes.
 *    c. Saves the uniform nodes, function values, and derivative values (implicitly used by interpolation).
 *    d. Performs Hermite interpolation using uniform nodes and saves the resulting curve data.
 *    e. Calculates maximum absolute error and MSE for Hermite interpolation with uniform nodes.
 *    f. Generates `n` Chebyshev nodes.
 *    g. Calculates `f(x)` and `df(x)` at these Chebyshev nodes.
 *    h. Saves the Chebyshev nodes, function values, and derivative values.
 *    i. Performs Hermite interpolation using Chebyshev nodes and saves the resulting curve data.
 *    j. Calculates maximum absolute error and MSE for Hermite interpolation with Chebyshev nodes.
 *    k. Prints the error summary (max error, MSE) for the current `n`.
 * 4. Saves the collected error trends (max error vs. n, MSE vs. n) for both node types to files.
 * 5. Generates Gnuplot scripts for visualizing the individual interpolation results and the error trends.
 * 6. Prints instructions for generating the plots using the saved Gnuplot scripts.
 *
 * @return 0 on successful execution, 1 on error (e.g., invalid user input).
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

    // Zapisanie oryginalnej funkcji do pliku
    saveDataToFile("original_function.dat", x, y_true, numPoints);

    // --- Arrays to Store Error Results ---
    // Store maximum absolute error for each node type, for n=1 to maxNodes
    double hermite_uniform_errors[maxNodes];
    double hermite_chebyshev_errors[maxNodes];

    // Store mean squared error (MSE) for each node type, for n=1 to maxNodes
    double hermite_uniform_mse[maxNodes];
    double hermite_chebyshev_mse[maxNodes];

    // --- Temporary Arrays for Calculations within the loop ---
    // Max size needed is maxNodes
    double nodes[MAX_NODES];         // Stores x-coordinates of interpolation nodes for current 'n'
    double values[MAX_NODES];        // Stores y-coordinates (f(nodes)) for current 'n'
    double derivatives[MAX_NODES];   // Stores function derivative (df(nodes)) values at nodes

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

        // Calculate the true function values f(x) and derivatives df(x) at these uniform nodes
        for (int j = 0; j < n; j++) {
            values[j] = f(nodes[j]);
            derivatives[j] = df(nodes[j]);
        }

        // Save the uniform nodes and their values to a file
        sprintf(filename, "uniform_nodes_n%d.dat", n);
        saveNodesToFile(filename, nodes, values, n);

        // Perform Hermite interpolation using uniform nodes
        for (int j = 0; j < numPoints; j++) {
            y_interp[j] = hermiteInterpolation(x[j], nodes, values, derivatives, n);
        }
        // Save the interpolated curve data
        sprintf(filename, "hermite_uniform_n%d.dat", n);
        saveDataToFile(filename, x, y_interp, numPoints);
        // Calculate and store the errors (max and MSE)
        ErrorResult error_hu = calculateError(y_true, y_interp, numPoints);
        hermite_uniform_errors[n-1] = error_hu.max_error;
        hermite_uniform_mse[n-1] = error_hu.mean_squared_error;

        // --- Part 2: Chebyshev Nodes ---

        // Generate 'n' Chebyshev nodes in the interval [a, b]
        chebyshevNodes(nodes, n);

        // Calculate the true function values f(x) and derivatives df(x) at these Chebyshev nodes
        for (int j = 0; j < n; j++) {
            values[j] = f(nodes[j]);
            derivatives[j] = df(nodes[j]);
        }

        // Save the Chebyshev nodes and their values to a file
        sprintf(filename, "chebyshev_nodes_n%d.dat", n);
        saveNodesToFile(filename, nodes, values, n);

        // Perform Hermite interpolation using Chebyshev nodes
        for (int j = 0; j < numPoints; j++) {
            y_interp[j] = hermiteInterpolation(x[j], nodes, values, derivatives, n);
        }
        // Save the interpolated curve data
        sprintf(filename, "hermite_chebyshev_n%d.dat", n);
        saveDataToFile(filename, x, y_interp, numPoints);
        // Calculate and store the errors (max and MSE)
        ErrorResult error_hc = calculateError(y_true, y_interp, numPoints);
        hermite_chebyshev_errors[n-1] = error_hc.max_error;
        hermite_chebyshev_mse[n-1] = error_hc.mean_squared_error;

        // --- Print Progress/Results ---
        printf("\nResults for Number of Nodes: %d\n", n);
        printf("-----------------------------------\n");
        printf("Maximum Absolute Errors:\n");
        printf("  Hermite (Uniform):      %.3e\n", hermite_uniform_errors[n-1]);
        printf("  Hermite (Chebyshev):    %.3e\n", hermite_chebyshev_errors[n-1]);

        printf("\nMean Squared Errors (MSE):\n");
        printf("  Hermite (Uniform):      %.3e\n", hermite_uniform_mse[n-1]);
        printf("  Hermite (Chebyshev):    %.3e\n", hermite_chebyshev_mse[n-1]);
    }

    // Save the collected error data (max error and MSE vs. n) to separate CSV files
    saveHermiteUniformErrorsToFile(maxNodes, hermite_uniform_errors, hermite_uniform_mse);
    saveHermiteChebyshevErrorsToFile(maxNodes, hermite_chebyshev_errors, hermite_chebyshev_mse);

    // Generate Gnuplot scripts for visualization
    // Script 1: Plots individual interpolation results for each 'n'
    generateGnuplotScript(maxNodes);

    // Script 2: Plots the summary of maximum errors vs. number of nodes
    generateErrorPlotScript(maxNodes,
                          hermite_uniform_errors, hermite_chebyshev_errors);

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