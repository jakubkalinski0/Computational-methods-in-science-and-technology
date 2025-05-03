/**
 * @file main.c
 * @brief Main program driver for performing and analyzing least-squares polynomial approximation
 *        across ranges of sample points (n) and polynomial degrees (m). Generates data (CSV for heatmaps,
 *        DAT for individual plots) and a Gnuplot script for individual plots.
 *        Heatmap visualization is handled by an external Python script (plot_heatmaps.py).
 */
#include "../include/common.h"        // Common definitions (constants like a, b, k, m, PI, MAX_NODES)
#include "../include/function.h"      // Function f(x) and its derivative df(x)
#include "../include/nodes.h"         // Node generation functions (uniformNodes used)
#include "../include/approximation.h" // Least squares approximation logic (leastSquaresApprox, evaluatePolynomial)
#include "../include/error.h"         // Error calculation (calculateError)
#include "../include/fileio.h"        // File I/O (saving data, generating Gnuplot scripts)
#include <stdio.h>                   // Standard Input/Output (printf, scanf, fprintf, FILE, fopen, fclose)
#include <stdlib.h>                  // Standard Library (exit)
#include <math.h>                    // For isnan() to check error results, NAN
#include <string.h>                  // For snprintf

// --- Configuration Constants ---
#define MAX_CONSIDERED_DEGREE 50
#define MIN_HEATMAP_N 2
#define NUM_PLOT_POINTS 1000

/**
 * @brief Main entry point for the least-squares polynomial approximation data generation program.
 */
int main() {
    // --- Variable Declarations ---
    int max_n; // Maximum number of sample points (n) to analyze.
    int max_m; // Maximum polynomial degree (m) to analyze.

    // --- Get User Input for Range ---
    printf("--- Least Squares Polynomial Approximation Data Generator ---\n");
    printf("Analyzes error across a range of sample points (n) and polynomial degrees (m).\n");
    printf("Generates data files (CSV for heatmaps, DAT for individual plots) and a Gnuplot script for individual plots.\n");
    printf("Heatmap plots are generated separately by a Python script (plot_heatmaps.py).\n\n"); // Updated info

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

    // Check if max_n is sufficient
    if (max_n <= max_m) {
         fprintf(stderr, "Error: Maximum number of sample points (%d) must be strictly greater than maximum degree (%d) to allow testing the highest degree m.\n", max_n, max_m);
         return 1; // Exit if condition not met
    }


    // --- Generate Dense Grid for Plotting Original Function ---
    double x_plot[NUM_PLOT_POINTS];        // x-coordinates for smooth plotting
    double y_true_plot[NUM_PLOT_POINTS];   // f(x) values at x_plot points
    double plot_step = (b - a) / (NUM_PLOT_POINTS - 1.0);
    printf("\nGenerating data for original function f(x) on [%.2f, %.2f] for error comparison...\n", a, b);
    for (int i = 0; i < NUM_PLOT_POINTS; i++) {
        x_plot[i] = a + i * plot_step;
        y_true_plot[i] = f(x_plot[i]);
    }
    if (NUM_PLOT_POINTS > 1) x_plot[NUM_PLOT_POINTS - 1] = b;
    saveDataToFile("original_function_plot.dat", x_plot, y_true_plot, NUM_PLOT_POINTS);
    printf("Saved original function plotting data to data/original_function_plot.dat\n");


    // --- Open CSV File for Heatmap Data ---
    const char *heatmap_filename = "data/approximation_heatmap_errors.csv";
    FILE *heatmap_file = fopen(heatmap_filename, "w");
    if (heatmap_file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s' for writing heatmap data.\n", heatmap_filename);
        return 1; // Exit on file opening failure
    }
    fprintf(heatmap_file, "N,M,MaxAbsoluteError,MeanSquaredError\n");
    printf("\nStarting approximation analysis for n from %d to %d and m from 0 to %d...\n", MIN_HEATMAP_N, max_n, max_m);
    printf("=========================================================================\n");


    // --- Main Loops: Iterate Through Number of Points (n) and Degrees (m) ---
    for (int n_val = MIN_HEATMAP_N; n_val <= max_n; n_val++) {
        printf("Processing n = %d points...\n", n_val);

        // Generate sample points for current n
        double sample_x[MAX_NODES]; // Use MAX_NODES for buffer safety
        double sample_y[MAX_NODES];
        uniformNodes(sample_x, n_val);
        for (int i = 0; i < n_val; i++) {
            sample_y[i] = f(sample_x[i]);
        }

        // Save sample points for this n
        char sample_filename[100];
        snprintf(sample_filename, sizeof(sample_filename), "sample_points_n%d.dat", n_val);
        saveNodesToFile(sample_filename, sample_x, sample_y, n_val);

        // Inner loop over degrees m
        for (int m_deg = 0; m_deg <= max_m; m_deg++) {
            // Check validity
            if (m_deg >= n_val) {
                appendErrorToHeatmapFile(heatmap_file, n_val, m_deg, NAN, NAN);
                continue; // Skip invalid combination
            }

            // Perform Least Squares
            double coefficients[MAX_CONSIDERED_DEGREE + 1]; // Safe buffer
            double y_approx_plot[NUM_PLOT_POINTS];
            int status = leastSquaresApprox(sample_x, sample_y, n_val, m_deg, coefficients);

            // Process Results
            ErrorResult errors;
            if (status == 0) { // Success
                // Evaluate polynomial on plot grid
                for (int j = 0; j < NUM_PLOT_POINTS; j++) {
                    y_approx_plot[j] = evaluatePolynomial(x_plot[j], coefficients, m_deg);
                }
                // Save evaluated polynomial data
                char approx_filename[100];
                snprintf(approx_filename, sizeof(approx_filename), "approximation_degree%d_points%d.dat", m_deg, n_val);
                saveDataToFile(approx_filename, x_plot, y_approx_plot, NUM_PLOT_POINTS);
                // Calculate errors
                errors = calculateError(y_true_plot, y_approx_plot, NUM_PLOT_POINTS);
            } else { // Failure
                errors.max_error = NAN;
                errors.mean_squared_error = NAN;
                 // fprintf(stderr, "Warning: Approximation failed for n=%d, m=%d. Errors marked as NAN in CSV.\n", n_val, m_deg); // Optional warning
            }
            // Append errors (or NANs) to CSV
            appendErrorToHeatmapFile(heatmap_file, n_val, m_deg, errors.max_error, errors.mean_squared_error);

        } // End loop m
        // Optional empty line between N blocks in CSV
        if (n_val < max_n) {
             fprintf(heatmap_file, "\n");
        }
    } // End loop n

    printf("=========================================================================\n");
    printf("Completed approximation analysis.\n");
    // --- Close the Heatmap CSV File ---
    fclose(heatmap_file);
    printf("Heatmap data saved to %s\n", heatmap_filename);

    // --- Generate Gnuplot Script for Individual Plots ONLY ---
    printf("\nGenerating Gnuplot script for individual plots...\n");
    generateAllIndividualApproxScripts(MIN_HEATMAP_N, max_n, max_m);

    // --- Removed calls to generate heatmap scripts ---
    // printf("Heatmap script generation is now handled by plot_heatmaps.py\n");

    // --- Final Instructions for User ---
    printf("\n=========================================================================\n");
    printf("Analysis and data generation complete.\n");
    printf("Data files saved in: data/\n");
    printf("Gnuplot script for individual plots saved in: scripts/\n");
    printf("\nTo generate plots:\n");
    printf("1. Ensure Python3 with pandas, matplotlib, seaborn, and numpy is installed.\n");
    printf("   (Activate your virtual environment if you created one: source .venv/bin/activate)\n"); // Added venv reminder
    printf("   (If not using venv: pip install pandas matplotlib seaborn numpy)\n");
    printf("2. Ensure Gnuplot is installed (for individual plots).\n");
    printf("3. Open a terminal in the project's root directory.\n");
    printf("4. Run the Python script to generate heatmap plots:\n");
    printf("   python3 plot_heatmaps.py\n");
    printf("5. Run the Gnuplot script to generate individual approximation plots:\n");
    printf("   gnuplot scripts/plot_all_approximations.gp\n");
    printf("6. Alternatively, use the Makefile target (builds C code, runs it, runs Python plotter, runs Gnuplot):\n");
    printf("   make plots\n");
    printf("Generated plots (.png) will be saved in: plots/\n");
    printf("=========================================================================\n");

    return 0;
}