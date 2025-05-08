/**
 * @file main.c
 * @brief Main program driver for finding roots of f(x) = x^n + x^m using Newton's and Secant methods.
 *        Analyzes convergence and iteration counts across ranges of starting points
 *        and precision values (rho).
 *        Generates data (CSV for tables/plots) and basic Gnuplot scripts.
 *        Detailed visualization (heatmaps) is handled by an external Python script.
 *        Uses simplified loops with epsilon comparison for endpoint and float equality.
 */
#include "../include/common.h"     // Common definitions (constants a, b, N_param, M_param, MAX_ITERATIONS)
#include "../include/function.h"   // Function f(x), df(x)
#include "../include/root_finding.h" // Newton's and Secant methods
#include "../include/fileio.h"     // File I/O (saving results, generating Gnuplot scripts)
#include <stdio.h>                // Standard Input/Output
#include <stdlib.h>               // Standard Library
#include <math.h>                 // For fabs, pow, fmax
#include <string.h>               // For snprintf
#include <float.h>                // For DBL_EPSILON

#define NUM_PLOT_POINTS 500       // Points for plotting f(x)
#define START_POINT_STEP 0.1      // Step for iterating starting points
#define NUM_PRECISIONS 7          // Number of precision values to test

// Define the precision values (rho)
const double PRECISION_VALUES[NUM_PRECISIONS] = {
    1e-2, 1e-3, 1e-4, 1e-5, 1e-7, 1e-10, 1e-15
};

/**
 * @brief Main entry point for the root-finding analysis program.
 */
int main() {
    printf("--- Root Finding Analysis for f(x) = x^%.0f + x^%.0f on [%.2f, %.2f] ---\n", N_param, M_param, a, b);
    printf("Methods: Newton, Secant\n");
    printf("Stopping Criteria: |x_next - x_curr| < rho AND |f(x_next)| < rho\n");
    printf("Generates CSV data and triggers Python script for plots.\n\n");

    // --- Generate Plot of f(x) ---
    printf("Generating Gnuplot script to plot f(x)...\n");
    generateFunctionPlotScript("plot_function.gp", "function_plot.png", NUM_PLOT_POINTS);
    printf("Run 'gnuplot scripts/plot_function.gp' to generate the plot.\n\n");

    // --- Open CSV File for Results ---
    const char *results_filename = "root_finding_results.csv";
    FILE *csv_file = openResultCsvFile(results_filename);
    if (csv_file == NULL) {
        return 1; // Exit if file cannot be opened
    }
    printf("Opened %s for writing results.\n", results_filename);

    printf("\nStarting root-finding analysis...\n");
    printf("===================================================================\n");

    // Epsilon for floating point comparisons near loop boundaries
    // Use a small fraction of the step size to decide if we are close enough to b
    double epsilon = START_POINT_STEP * 0.01; // Small tolerance relative to step

    // --- Iterate Through Precision Values ---
    for (int p_idx = 0; p_idx < NUM_PRECISIONS; ++p_idx) {
        double current_precision = PRECISION_VALUES[p_idx];
        printf("Processing Precision rho = %.1e\n", current_precision);

        // --- Newton's Method ---
        printf("  Running Newton's Method...\n");
        // Iterate starting points x0 from a up to b (inclusive)
        for (double x0 = a; x0 <= b + epsilon; x0 += START_POINT_STEP) {
            double current_x0 = x0;
            // Clamp to b if we slightly overshoot due to float addition
            if (x0 > b && x0 < b + epsilon) {
                current_x0 = b;
            }
            // Ensure we don't go significantly past b
            if (current_x0 > b + epsilon) break;

            RootResult newton_res = newtonMethod(current_x0, current_precision, MAX_ITERATIONS);
            appendNewtonResultToCsv(csv_file, current_x0, current_precision, newton_res);

            // Break *after* processing b
            if (fabs(current_x0 - b) < DBL_EPSILON) break;
        }

        // --- Secant Method - Case 1: x1 = a (fixed), x0 iterates ---
        printf("  Running Secant Method (x1 = a = %.2f fixed)...\n", a);
        double x1_fixed_a = a;
        // Iterate x0 from a + step up to b (inclusive)
        for (double x0 = a + START_POINT_STEP; x0 <= b + epsilon; x0 += START_POINT_STEP) {
            double current_x0 = x0;
            if (x0 > b && x0 < b + epsilon) {
                current_x0 = b;
            }
            if (current_x0 > b + epsilon) break;

            // Avoid x0 == x1 using a relative epsilon check
            if (fabs(current_x0 - x1_fixed_a) < DBL_EPSILON * fmax(1.0, fmax(fabs(current_x0), fabs(x1_fixed_a)))) continue;

            RootResult secant_res_a = secantMethod(current_x0, x1_fixed_a, current_precision, MAX_ITERATIONS);
            appendSecantResultToCsv(csv_file, current_x0, x1_fixed_a, current_precision, secant_res_a);

            // Break *after* processing b
            if (fabs(current_x0 - b) < DBL_EPSILON) break;
        }

         // --- Secant Method - Case 2: x1 = b (fixed), x0 iterates ---
        printf("  Running Secant Method (x1 = b = %.2f fixed)...\n", b);
        double x1_fixed_b = b;
        // Iterate x0 from a up to b (inclusive)
         for (double x0 = a; x0 <= b + epsilon; x0 += START_POINT_STEP) {
             double current_x0 = x0;
             if (x0 > b && x0 < b + epsilon) {
                 current_x0 = b;
             }
             if (current_x0 > b + epsilon) break;

             // Avoid x0 == x1 using a relative epsilon check
             if (fabs(current_x0 - x1_fixed_b) < DBL_EPSILON * fmax(1.0, fmax(fabs(current_x0), fabs(x1_fixed_b)))) continue;

             RootResult secant_res_b = secantMethod(current_x0, x1_fixed_b, current_precision, MAX_ITERATIONS);
             appendSecantResultToCsv(csv_file, current_x0, x1_fixed_b, current_precision, secant_res_b);

             // Break *after* processing b
             if (fabs(current_x0 - b) < DBL_EPSILON) break;
         }
         printf("\n"); // Newline after each precision level

    } // End loop over precision values

    printf("===================================================================\n");
    printf("Completed root-finding analysis.\n");

    // --- Close the Results CSV File ---
    fclose(csv_file);
    printf("Results saved to data/%s\n", results_filename);

    printf("\nIteration heatmap plots will be generated by the Python script.\n");
    // Removed call to generateIterationPlotScripts

    // --- Final Instructions ---
    printf("\n===================================================================\n");
    printf("Analysis and data generation complete.\n");
    printf("Data file saved in: data/\n");
    printf("Gnuplot script for function plot saved in: scripts/\n");
    printf("\nTo generate plots:\n");
    printf("1. Ensure Python3 with pandas, matplotlib, seaborn, and numpy is installed.\n");
    printf("   (Activate your virtual environment if used: source .venv/bin/activate)\n");
    printf("2. Run the Python script for tables and heatmaps:\n");
    printf("   python3 src/plot_results.py\n");
    printf("3. Optionally, run Gnuplot for the function plot:\n");
    printf("   gnuplot scripts/plot_function.gp\n");
    printf("4. Alternatively, use the Makefile:\n");
    printf("   make plots   (runs C code, then Python script)\n");
    printf("   make gnuplot_func_plot (runs only the Gnuplot function plot)\n");
    printf("Generated plots (.png, .svg, etc.) will be saved in: plots/\n");
    printf("===================================================================\n");

    return 0;
}