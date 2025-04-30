/**
 * @file main.c
 * @brief Main program driver for performing and analyzing least-squares polynomial approximation
 *        across ranges of sample points (n) and polynomial degrees (m). Generates data and
 *        plotting scripts for error heatmaps and individual approximation plots for all (n, m) pairs.
 *
 * This program performs the following steps:
 * 1. Prompts the user for the maximum number of sample points (max_n) and the maximum polynomial degree (max_m)
 *    to include in the analysis range.
 * 2. Validates user input against defined limits (MAX_NODES, MAX_CONSIDERED_DEGREE) and necessary conditions (n > m for approximation).
 * 3. Generates a dense set of points for plotting the original function f(x) over the interval [a, b].
 * 4. Saves the original function plotting data to 'data/original_function_plot.dat'.
 * 5. Opens a single CSV file ('data/approximation_heatmap_errors.csv') to store error results for all (n, m) combinations.
 * 6. Iterates through a range of sample point counts 'n' (from MIN_HEATMAP_N up to max_n).
 * 7. For each 'n', generates 'n' uniformly distributed sample points (x_i, y_i = f(x_i)) within [a, b].
 * 8. Saves the sample points for the current 'n' to 'data/sample_points_n{n}.dat'.
 * 9. For each 'n', iterates through polynomial degrees 'm' (from 0 up to max_m).
 * 10. For each (n, m) pair:
 *    a. Checks if the combination is valid for least squares (m < n). If not, writes NAN to the heatmap CSV and continues.
 *    b. If valid, calculates the coefficients of the least-squares polynomial P_m(x) using `leastSquaresApprox`.
 *    c. If coefficient calculation is successful (status == 0):
 *       i. Evaluates P_m(x) on the dense plotting grid (`x_plot`) using `evaluatePolynomial`.
 *       ii. Saves the evaluated polynomial data to 'data/approximation_degree{m}_points{n}.dat'.
 *       iii. Calculates the maximum absolute error and MSE between f(x) and P_m(x) on the dense grid using `calculateError`.
 *       iv. Appends the (n, m) pair and the calculated errors to the heatmap CSV file using `appendErrorToHeatmapFile`.
 *    d. If coefficient calculation failed (status == -1), appends NAN for errors to the heatmap CSV.
 * 11. Closes the heatmap CSV file.
 * 12. Generates Gnuplot scripts for:
 *     - Heatmap of Max Absolute Error ('scripts/plot_approx_max_error_heatmap.gp').
 *     - Heatmap of Mean Squared Error ('scripts/plot_approx_mse_heatmap.gp').
 *     - All individual approximation plots ('scripts/plot_all_approximations.gp').
 * 13. Prints instructions on how to generate the plots using Gnuplot or the Makefile.
 */
#include "../include/common.h"        // Common definitions (constants like a, b, k, m, PI, MAX_NODES)
#include "../include/function.h"      // Function f(x) and its derivative df(x)
#include "../include/nodes.h"         // Node generation functions (uniformNodes used)
#include "../include/approximation.h" // Least squares approximation logic (leastSquaresApprox, evaluatePolynomial)
#include "../include/error.h"         // Error calculation (calculateError)
#include "../include/fileio.h"        // File I/O (saving data, generating Gnuplot scripts)
#include <stdio.h>                   // Standard Input/Output (printf, scanf, fprintf, FILE, fopen, fclose)
#include <stdlib.h>                  // Standard Library (exit)
#include <math.h>                    // For isnan() to check error results
#include <string.h>                  // For snprintf

// --- Configuration Constants ---
/**
 * @brief Maximum recommended polynomial degree for approximation.
 * Used to size static buffers and as a suggested upper limit for user input.
 */
#define MAX_CONSIDERED_DEGREE 50 // Adjusted name to reflect its use as max m

/**
 * @brief Minimum number of sample points required to start the heatmap analysis.
 * Must be at least 2 to allow m=1. For m=0, n=1 is sufficient, but analysis over m=0..max_m
 * requires n > max_m eventually, so starting slightly higher makes sense.
 */
#define MIN_HEATMAP_N 2

/**
 * @brief Number of points used for plotting smooth curves (original function and approximations)
 *        and for calculating errors against the true function.
 */
#define NUM_PLOT_POINTS 1000

/**
 * @brief Main entry point for the least-squares polynomial approximation heatmap data generation program.
 *
 * Orchestrates the process of getting user input for the range of n and m, generating
 * data points, performing approximations for all combinations within the range,
 * calculating errors, saving results to a single CSV file, and generating
 * plotting scripts.
 *
 * @return 0 on successful completion.
 * @return 1 if there is an error during input processing or initial setup.
 */
int main() {
    // --- Variable Declarations ---
    int max_n; // Maximum number of sample points (n) to analyze.
    int max_m; // Maximum polynomial degree (m) to analyze.

    // --- Get User Input for Range ---
    printf("--- Least Squares Polynomial Approximation Data Generator ---\n");
    printf("Analyzes error across a range of sample points (n) and polynomial degrees (m).\n");
    printf("Generates data files and Gnuplot scripts for heatmaps and individual plots.\n\n");

    printf("Enter the maximum number of sample points to analyze (n >= %d, n <= %d): ", MIN_HEATMAP_N, MAX_NODES);
    if (scanf("%d", &max_n) != 1 || max_n < MIN_HEATMAP_N || max_n > MAX_NODES) {
        fprintf(stderr, "Error: Invalid input for maximum sample points. Must be between %d and %d.\n", MIN_HEATMAP_N, MAX_NODES);
        return 1; // Exit on invalid input
    }

    printf("Enter the maximum polynomial degree to analyze (m >= 0, m <= %d): ", MAX_CONSIDERED_DEGREE);
     if (scanf("%d", &max_m) != 1 || max_m < 0 || max_m > MAX_CONSIDERED_DEGREE) {
        fprintf(stderr, "Error: Invalid input for maximum polynomial degree. Must be between 0 and %d.\n", MAX_CONSIDERED_DEGREE);
        return 1; // Exit on invalid input
    }

    // Check if max_n is sufficient to even allow the maximum degree max_m
    // Least squares for degree m requires n > m (at least m+1 points).
    if (max_n <= max_m) {
         fprintf(stderr, "Error: Maximum number of sample points (%d) must be strictly greater than maximum degree (%d) to allow testing the highest degree m.\n", max_n, max_m);
         return 1; // Exit if condition not met
    }


    // --- Generate Dense Grid for Plotting Original Function ---
    double x_plot[NUM_PLOT_POINTS];        // x-coordinates for smooth plotting
    double y_true_plot[NUM_PLOT_POINTS];   // f(x) values at x_plot points

    double plot_step = (b - a) / (NUM_PLOT_POINTS - 1.0); // Step size for plotting grid
    printf("\nGenerating data for original function f(x) on [%.2f, %.2f] for error comparison...\n", a, b);
    for (int i = 0; i < NUM_PLOT_POINTS; i++) {
        x_plot[i] = a + i * plot_step;
        y_true_plot[i] = f(x_plot[i]); // Calculate true function value
    }
    // Ensure the last point is exactly 'b' to avoid floating-point drift issues
    if (NUM_PLOT_POINTS > 1) x_plot[NUM_PLOT_POINTS - 1] = b;

    // Save the generated original function data for plotting
    saveDataToFile("original_function_plot.dat", x_plot, y_true_plot, NUM_PLOT_POINTS);
    printf("Saved original function plotting data to data/original_function_plot.dat\n");


    // --- Open CSV File for Heatmap Data ---
    const char *heatmap_filename = "data/approximation_heatmap_errors.csv";
    FILE *heatmap_file = fopen(heatmap_filename, "w"); // Open the file in write mode ("w")
    if (heatmap_file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s' for writing heatmap data.\n", heatmap_filename);
        return 1; // Exit on file opening failure
    }

    // Write header row for the heatmap CSV file
    fprintf(heatmap_file, "N,M,MaxAbsoluteError,MeanSquaredError\n");
    printf("\nStarting approximation analysis for n from %d to %d and m from 0 to %d...\n", MIN_HEATMAP_N, max_n, max_m);
    printf("=========================================================================\n");


    // --- Main Loops: Iterate Through Number of Points (n) and Degrees (m) ---
    // Outer loop iterates through the number of sample points (n)
    for (int n_val = MIN_HEATMAP_N; n_val <= max_n; n_val++) {
        printf("Processing n = %d points...\n", n_val);

        // Generate n_val uniformly distributed sample points (x_i) for the current n
        // These points remain the same for all degrees m for this specific n_val.
        double sample_x[MAX_NODES]; // Size MAX_NODES is safe because n_val <= max_n <= MAX_NODES
        double sample_y[MAX_NODES];
        uniformNodes(sample_x, n_val); // Generate n_val x-coordinates
        for (int i = 0; i < n_val; i++) {
            sample_y[i] = f(sample_x[i]); // Calculate corresponding y-coordinates f(x_i)
        }

        // Save the sample points for this specific n
        char sample_filename[100];
        snprintf(sample_filename, sizeof(sample_filename), "sample_points_n%d.dat", n_val);
        saveNodesToFile(sample_filename, sample_x, sample_y, n_val);
        // printf("  Saved sample points to data/%s\n", sample_filename); // Too verbose


        // Inner loop iterates through polynomial degrees (m)
        for (int m_deg = 0; m_deg <= max_m; m_deg++) {
            // printf("  Testing degree m = %d...\n", m_deg); // Too verbose

            // --- Check if (n, m) combination is valid for Least Squares ---
            // Least squares requires number of points (n) > polynomial degree (m), i.e., n >= m + 1.
            if (m_deg >= n_val) {
                // This combination (n_val, m_deg) is invalid for least squares.
                // Append NANs for errors to the heatmap CSV file for this entry.
                appendErrorToHeatmapFile(heatmap_file, n_val, m_deg, NAN, NAN);
                // printf("    Skipping calculation for n=%d, m=%d (m >= n).\n", n_val, m_deg); // Too verbose
                continue; // Skip to the next degree for this number of points
            }

            // If we reach here, the combination (n_val, m_deg) is valid (m_deg < n_val).
            // --- Perform Least Squares Approximation ---
            // coefficients buffer size needs to be max_m + 1 because m_deg goes up to max_m.
            // But specifically for leastSquaresApprox call, it only needs m_deg + 1.
            // Using MAX_CONSIDERED_DEGREE + 1 for the buffer is safe as max_m <= MAX_CONSIDERED_DEGREE.
            double coefficients[MAX_CONSIDERED_DEGREE + 1]; // Buffer for the m_deg + 1 coefficients
            double y_approx_plot[NUM_PLOT_POINTS];           // Buffer for evaluated polynomial values on plot grid

            int status = leastSquaresApprox(sample_x, sample_y, n_val, m_deg, coefficients);

            // --- Process Results ---
            ErrorResult errors; // Struct to hold calculated errors

            if (status == 0) { // Approximation successful
                // Evaluate the calculated polynomial on the dense plotting grid (x_plot)
                for (int j = 0; j < NUM_PLOT_POINTS; j++) {
                    y_approx_plot[j] = evaluatePolynomial(x_plot[j], coefficients, m_deg);
                }

                // Save the calculated polynomial curve data for this specific n and m
                char approx_filename[100];
                snprintf(approx_filename, sizeof(approx_filename), "approximation_degree%d_points%d.dat", m_deg, n_val);
                saveDataToFile(approx_filename, x_plot, y_approx_plot, NUM_PLOT_POINTS);
                // printf("  Saved approximation curve data to data/%s\n", approx_filename); // Too verbose

                // Calculate errors (Max Absolute and MSE) by comparing approximation to true function on plot grid
                errors = calculateError(y_true_plot, y_approx_plot, NUM_PLOT_POINTS);

                 // Print errors for the current (n, m) combination
                 // printf("    n=%d, m=%d: Max Error = %.5e, MSE = %.5e\n", n_val, m_deg, errors.max_error, errors.mean_squared_error); // Too verbose

            } else { // Approximation failed (e.g., singular matrix in leastSquaresApprox)
                // Set errors to NAN to indicate failure
                errors.max_error = NAN;
                errors.mean_squared_error = NAN;
                // An error message was already printed by leastSquaresApprox (can be noisy in loop).
                // Consider suppressing leastSquaresApprox error messages if this loop prints warnings.
                fprintf(stderr, "Warning: Approximation failed for n=%d, m=%d. Errors marked as NAN in CSV.\n", n_val, m_deg);
            }

            // Append the results (n, m, errors) to the heatmap CSV file regardless of success/failure
            appendErrorToHeatmapFile(heatmap_file, n_val, m_deg, errors.max_error, errors.mean_squared_error);

        } // --- End of loop over degrees m ---
         // Optional newline after each block of 'm' values for a fixed 'n'.
         // This can help Gnuplot's `splot` interpret blocks if needed,
         // and makes the CSV file more readable per block of N.
         if (n_val < max_n) {
             fprintf(heatmap_file, "\n");
         }
    } // --- End of loop over number of points n ---

    printf("=========================================================================\n");
    printf("Completed approximation analysis for n from %d to %d and m from 0 to %d.\n", MIN_HEATMAP_N, max_n, max_m);
    // --- Close the Heatmap CSV File ---
    fclose(heatmap_file);
    printf("Heatmap data saved to %s\n", heatmap_filename);

    // --- Generate Gnuplot Scripts ---
    printf("\nGenerating Gnuplot scripts...\n");
    // Generate script for Max Absolute Error heatmap
    generateApproxMaxErrorHeatmapScript(MIN_HEATMAP_N, max_n, max_m);
    // Generate script for Mean Squared Error heatmap
    generateApproxMseHeatmapScript(MIN_HEATMAP_N, max_n, max_m);
    // Generate script for ALL individual approximation plots
    generateAllIndividualApproxScripts(MIN_HEATMAP_N, max_n, max_m);


    // --- Final Instructions for User ---
    printf("\n=========================================================================\n");
    printf("Analysis and data generation complete.\n");
    printf("Data files saved in: data/\n");
    printf("Gnuplot scripts saved in: scripts/\n");
    printf("\nTo generate plots:\n");
    printf("1. Ensure Gnuplot is installed.\n");
    printf("2. Open a terminal in the project's root directory.\n");
    printf("3. Run the heatmap scripts (these generate 2 plots with reversed axes and log scale):\n");
    printf("   gnuplot scripts/plot_approx_max_error_heatmap.gp\n");
    printf("   gnuplot scripts/plot_approx_mse_heatmap.gp\n");
    printf("4. Run the individual plots script (this generates many plots):\n");
    printf("   gnuplot scripts/plot_all_approximations.gp\n");
    printf("5. Alternatively, use the Makefile target (runs all scripts):\n");
    printf("   make plots\n");
    printf("Generated plots (.png) will be saved in: plots/\n");
    printf("=========================================================================\n");

    return 0; // Indicate successful program execution
}