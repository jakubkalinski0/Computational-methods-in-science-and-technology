/**
 * @file main.c
 * @brief Main program driver for trigonometric approximation using direct summation formulas.
 *        Analyzes error across ranges of sample points (n) and maximum harmonic order (m).
 *        Generates data (CSV for heatmaps, DAT for individual plots) and a Gnuplot script.
 *        Heatmap visualization is handled by an external Python script.
 *        Enforces the condition m < n/2 for the direct method.
 */
#include "../include/common.h"        // Common definitions (constants, MAX_HARMONIC)
#include "../include/function.h"      // Function f(x)
#include "../include/nodes.h"         // Node generation functions (uniformNodes used)
#include "../include/approximation.h" // Direct trigonometric approximation functions
#include "../include/error.h"         // Error calculation (calculateError)
#include "../include/fileio.h"        // File I/O (saving data, generating Gnuplot scripts)
#include <stdio.h>                   // Standard Input/Output
#include <stdlib.h>                  // Standard Library
#include <math.h>                    // For isnan(), NAN
#include <string.h>                  // For snprintf

// --- Configuration Constants ---
// Minimum n required for m=1 (since m < n/2 -> 1 < n/2 -> n > 2)
#define MIN_HEATMAP_N 3
#define NUM_PLOT_POINTS 1000 // Number of points for plotting smooth curves

/**
 * @brief Main entry point for the trigonometric approximation (direct formulas) data generation program.
 */
int main() {
    // --- Variable Declarations ---
    int max_n; // Maximum number of sample points (n) to analyze.
    int max_m; // Maximum harmonic order (m) to analyze.

    // --- Get User Input for Range ---
    printf("--- Least Squares Trigonometric Approximation (Direct Formulas) ---\n");
    printf("Analyzes error across a range of sample points (n) and max harmonic order (m).\n");
    printf("Uses direct summation formulas (requires m < n/2 for validity).\n");
    printf("Generates data files (CSV for heatmaps, DAT for individual plots) and a Gnuplot script.\n");
    printf("Heatmap plots are generated separately by a Python script.\n\n");

    printf("Enter the maximum number of sample points to analyze (n >= %d, n <= %d): ", MIN_HEATMAP_N, MAX_NODES);
    if (scanf("%d", &max_n) != 1 || max_n < MIN_HEATMAP_N || max_n > MAX_NODES) {
        fprintf(stderr, "Error: Invalid input for maximum sample points (n). Must be between %d and %d.\n", MIN_HEATMAP_N, MAX_NODES);
        return 1; // Exit on invalid input
    }

    printf("Enter the maximum harmonic order (m >= 0, m <= %d): ", MAX_HARMONIC);
     if (scanf("%d", &max_m) != 1 || max_m < 0 || max_m > MAX_HARMONIC) {
        fprintf(stderr, "Error: Invalid input for maximum harmonic order (m). Must be between 0 and %d.\n", MAX_HARMONIC);
        return 1; // Exit on invalid input
    }

    // --- Generate Dense Grid for Plotting Original Function ---
    double x_plot[NUM_PLOT_POINTS];        // x-coordinates for smooth plotting
    double y_true_plot[NUM_PLOT_POINTS];   // f(x) values at x_plot points
    double plot_step = (L) / (NUM_PLOT_POINTS - 1.0); // Use interval length L
    printf("\nGenerating data for original function f(x) on [%.2f, %.2f] for error comparison...\n", a, b);
    for (int i = 0; i < NUM_PLOT_POINTS; i++) {
        x_plot[i] = a + i * plot_step;
        y_true_plot[i] = f(x_plot[i]);
    }
    if (NUM_PLOT_POINTS > 1) x_plot[NUM_PLOT_POINTS - 1] = b; // Ensure endpoint accuracy
    saveDataToFile("original_function_plot.dat", x_plot, y_true_plot, NUM_PLOT_POINTS);
    printf("Saved original function plotting data to data/original_function_plot.dat\n");


    // --- Open CSV File for Heatmap Data ---
    const char *heatmap_filename = "data/trig_approx_direct_heatmap_errors.csv"; // Updated filename
    FILE *heatmap_file = fopen(heatmap_filename, "w");
    if (heatmap_file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s' for writing heatmap data.\n", heatmap_filename);
        return 1; // Exit on file opening failure
    }
    fprintf(heatmap_file, "N,m,MaxAbsoluteError,MeanSquaredError\n"); // Updated header with 'm'
    printf("\nStarting trigonometric approximation analysis for n from %d to %d and m from 0 to %d (only considering m < n/2)...\n", MIN_HEATMAP_N, max_n, max_m);
    printf("=========================================================================================================\n");


    // --- Main Loops: Iterate Through Number of Points (n) and Max Harmonic (m) ---
    for (int n_val = MIN_HEATMAP_N; n_val <= max_n; n_val++) {
        printf("Processing n = %d points...\n", n_val);

        // Generate sample points for current n (using uniform nodes)
        double sample_x[MAX_NODES]; // Use MAX_NODES for buffer safety
        double sample_y[MAX_NODES];
        uniformNodes(sample_x, n_val); // Generate n uniform nodes in [a, b]
        for (int i = 0; i < n_val; i++) {
            sample_y[i] = f(sample_x[i]); // Calculate function values at nodes
        }

        // Save sample points for this n (filename unchanged)
        char sample_filename[100];
        snprintf(sample_filename, sizeof(sample_filename), "sample_points_n%d.dat", n_val);
        saveNodesToFile(sample_filename, sample_x, sample_y, n_val);

        // Inner loop over max harmonic order m
        for (int m_deg = 0; m_deg <= max_m; m_deg++) {

            // --- Apply the crucial condition m < n/2 for the direct method ---
            // Use floating point division for correctness with odd n
            if (m_deg >= (double)n_val / 2.0) {
                // Condition not met, skip calculation for this (n, m) pair
                // Record NAN in the CSV file to indicate this combination was skipped/invalid
                appendErrorToHeatmapFile(heatmap_file, n_val, m_deg, NAN, NAN);
                continue; // Move to the next value of m
            }

            // --- Perform Direct Trigonometric Coefficient Calculation ---
            int num_coeffs_required = 1 + 2 * m_deg;
            // Define buffer size based on the absolute maximum possible m
            double coefficients[1 + 2 * MAX_HARMONIC];
            // Ensure buffer is large enough (should always be true if max_m <= MAX_HARMONIC)
            if ((size_t)num_coeffs_required > sizeof(coefficients)/sizeof(coefficients[0])) {
                 fprintf(stderr, "FATAL ERROR: Coefficient buffer overflow detected for m=%d. Increase MAX_HARMONIC?\n", m_deg);
                 fclose(heatmap_file); // Close file before exiting
                 return 1; // Critical error
            }

            double y_approx_plot[NUM_PLOT_POINTS]; // Buffer for plotting approximated values

            // Call the function to calculate coefficients using direct summation
            int status = calculateTrigonometricCoeffsDirect(sample_x, sample_y, n_val, m_deg, coefficients);

            // Process Results
            ErrorResult errors;
            // Status should be 0 if m < n/2 check passed, but check anyway.
            if (status == 0) { // Calculation succeeded
                // Evaluate the resulting trigonometric sum on the dense plot grid
                for (int j = 0; j < NUM_PLOT_POINTS; j++) {
                    y_approx_plot[j] = evaluateTrigonometricSum(x_plot[j], coefficients, m_deg);
                }
                // Save the evaluated trigonometric sum data for plotting
                char approx_filename[100];
                // Use 'm' (m_deg) in the filename
                snprintf(approx_filename, sizeof(approx_filename), "trig_approx_m%d_points%d.dat", m_deg, n_val);
                saveDataToFile(approx_filename, x_plot, y_approx_plot, NUM_PLOT_POINTS);
                // Calculate approximation errors by comparing with true function values
                errors = calculateError(y_true_plot, y_approx_plot, NUM_PLOT_POINTS);
            } else { // Calculation failed (e.g., validation inside the function)
                // Should not happen if m < n/2 check is correct, but handle defensively
                errors.max_error = NAN;
                errors.mean_squared_error = NAN;
            }
            // Append errors (or NANs for skipped/failed cases) to the CSV file
            // Pass m_deg as the parameter 'm'
            appendErrorToHeatmapFile(heatmap_file, n_val, m_deg, errors.max_error, errors.mean_squared_error);

        } // End inner loop m (max harmonic)
    } // End outer loop n (number of points)

    printf("=========================================================================================================\n");
    printf("Completed trigonometric approximation analysis.\n");
    // --- Close the Heatmap CSV File ---
    fclose(heatmap_file);
    printf("Heatmap data saved to %s\n", heatmap_filename);

    // --- Generate Gnuplot Script for Individual Plots ---
    printf("\nGenerating Gnuplot script for individual trigonometric plots (m < n/2)...\n");
    // Pass max_m to the script generator
    generateAllIndividualTrigApproxScripts(MIN_HEATMAP_N, max_n, max_m);

    // --- Python script handles heatmaps ---
    printf("Heatmap generation is handled by plot_heatmaps.py.\n");

    // --- Final Instructions for User ---
    printf("\n=========================================================================================================\n");
    printf("Analysis and data generation complete.\n");
    printf("Data files saved in: data/\n");
    printf("Gnuplot script for individual plots saved in: scripts/\n");
    printf("\nTo generate plots:\n");
    printf("1. Ensure Python3 with pandas, matplotlib, seaborn, and numpy is installed.\n");
    printf("   (Activate your virtual environment if used: source .venv/bin/activate)\n");
    printf("2. Ensure Gnuplot is installed.\n");
    printf("3. Open a terminal in the project's root directory.\n");
    printf("4. **Crucially, modify 'src/plot_heatmaps.py':**\n"); // Emphasize Python script modification
    printf("   - Update `CSV_FILE` to read '%s'.\n", heatmap_filename);
    printf("   - Change the X-axis label and relevant titles to use 'Max Harmonic (m)'.\n");
    printf("   - Update the expected column name in `required_cols` to 'm'.\n");
    printf("   - Adjust output plot filenames if desired.\n");
    printf("5. Run the Python script for heatmaps:\n");
    printf("   python3 src/plot_heatmaps.py\n");
    printf("6. Run the Gnuplot script for individual plots:\n");
    printf("   gnuplot scripts/plot_all_trig_approximations.gp\n");
    printf("7. Alternatively, update the Makefile targets (`HEATMAP_DATA`, help messages etc.) and use:\n");
    printf("   make plots\n");
    printf("Generated plots (.png, .svg, etc.) will be saved in: plots/\n");
    printf("=========================================================================================================\n");

    return 0;
}