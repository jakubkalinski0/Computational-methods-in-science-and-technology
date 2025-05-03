/**
 * @file fileio.c
 * @brief Implementation of file input/output operations for polynomial approximation analysis.
 *
 * This file contains functions for:
 * - Saving (x, y) data points to files (e.g., original function, approximated polynomial).
 * - Saving the sample points used for the approximation.
 * - Saving calculated approximation errors (max absolute, MSE) vs. polynomial degree to a CSV file (optional, from old code).
 * - Appending errors to a heatmap CSV file.
 * - Generating a Gnuplot script to visualize the individual approximation results.
 * Heatmap plotting is handled by an external Python script (plot_heatmaps.py).
 * It assumes a standard directory structure ('data/', 'scripts/', 'plots/').
 */
#include "../include/fileio.h" // Function prototypes and common includes (common.h)
#include <stdio.h>             // Standard I/O functions (fopen, fprintf, fclose, FILE)
#include <stdlib.h>            // Standard library
#include <string.h>            // For string manipulation (like snprintf)
#include <math.h>              // For NAN, isnan

// Global constants 'a' and 'b' from common.h (via fileio.h) are used in Gnuplot script generation.

/**
 * @brief Saves a set of (x, y) data points to a specified file within the 'data/' directory.
 *
 * Constructs the full file path by prepending "data/" to the provided `filename`.
 * Opens the file for writing and writes each (x, y) pair on a separate line,
 * formatted as "%lf %lf\n". Handles file opening errors by printing a message
 * to stderr and returning.
 *
 * @param filename The base name of the file (e.g., "original_function_plot.dat", "approximation_degree5_points50.dat"). Path becomes "data/<filename>".
 * @param x Array of x-coordinates.
 * @param y Array of y-coordinates.
 * @param n The number of data points to save.
 */
void saveDataToFile(const char* filename, double x[], double y[], int n) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "data/%s", filename);

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        fprintf(stderr, "Error [saveDataToFile]: Could not open file: %s\n", filepath);
        return;
    }

    for (int i = 0; i < n; i++) {
        fprintf(file, "%lf %lf\n", x[i], y[i]);
    }

    fclose(file);
}

/**
 * @brief Saves the sample points (x, y) used for approximation to a file in the 'data/' directory.
 *
 * Functionally identical to `saveDataToFile`, but conceptually used for the specific
 * purpose of saving the discrete (x_i, y_i) points used as input to the
 * `leastSquaresApprox` function.
 *
 * @param filename The base name of the file (e.g., "sample_points_n50.dat"). Path becomes "data/<filename>".
 * @param points_x Array of sample point x-coordinates.
 * @param points_y Array of sample point y-coordinates.
 * @param n The number of sample points.
 */
void saveNodesToFile(const char* filename, double points_x[], double points_y[], int n) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "data/%s", filename);

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        fprintf(stderr, "Error [saveNodesToFile]: Could not open file: %s\n", filepath);
        return;
    }

    for (int i = 0; i < n; i++) {
        fprintf(file, "%lf %lf\n", points_x[i], points_y[i]);
    }

    fclose(file);
}


/**
 * @brief Saves approximation errors (maximum absolute and MSE) versus polynomial degree to a CSV file.
 * This function is from the original project's single-n analysis. It might not be needed
 * if the heatmap CSV is the only error output. Kept for compatibility if desired.
 *
 * @param filename The full or relative path name for the output CSV file.
 * @param maxDegree The highest degree included in the error arrays (index `maxDegree`).
 * @param max_errors Array storing max absolute errors, indexed 0 to `maxDegree`.
 * @param mse_errors Array storing mean squared errors, indexed 0 to `maxDegree`.
 * @return 0 on success, -1 if the file could not be opened for writing.
 */
int saveApproximationErrorsToFile(const char* filename, int maxDegree, double max_errors[], double mse_errors[]) {
    // This function is likely deprecated by the heatmap CSV.
    // Implement if needed, otherwise, it can be removed or left as a stub.
     fprintf(stderr, "Warning: saveApproximationErrorsToFile is deprecated in heatmap mode.\n");
     return -1; // Indicate not implemented or not used
}


/**
 * @brief Saves approximation errors (N, M, Max Absolute, MSE) to a CSV file for heatmap plotting.
 *
 * This function is designed to write a single row of data for a specific (n, m) combination
 * to an already opened file stream. The file must be opened and the header written by the caller.
 *
 * @param file The already opened FILE pointer for the heatmap CSV file.
 * @param n The number of sample points.
 * @param m The polynomial degree.
 * @param max_error The calculated maximum absolute error for this (n, m).
 * @param mse_error The calculated mean squared error for this (n, m).
 */
void appendErrorToHeatmapFile(FILE* file, int n, int m, double max_error, double mse_error) {
    if (file == NULL) {
        fprintf(stderr, "Error [appendErrorToHeatmapFile]: Invalid file pointer.\n");
        return;
    }
    // Write n, m, max_error, and mse_error. Use %.10e for scientific notation.
    // Write "NAN" string if value is NaN.
    if (isnan(max_error)) {
        if (isnan(mse_error)) {
             fprintf(file, "%d,%d,NAN,NAN\n", n, m);
        } else {
             fprintf(file, "%d,%d,NAN,%.10e\n", n, m, mse_error);
        }
    } else {
        if (isnan(mse_error)) {
             fprintf(file, "%d,%d,%.10e,NAN\n", n, m, max_error);
        } else {
             fprintf(file, "%d,%d,%.10e,%.10e\n", n, m, max_error, mse_error);
        }
    }
}


/**
 * @brief Generates a single Gnuplot script ('scripts/plot_all_approximations.gp')
 *        to visualize individual approximation results for ALL (n, m) combinations.
 *
 * Creates a Gnuplot script that, when executed, generates a series of PNG plots,
 * one for each valid (n, m) combination (where m < n) within the analyzed ranges.
 * Each plot compares:
 * 1. The original function `f(x)` (read from 'data/original_function_plot.dat').
 * 2. The approximating polynomial `P_m(x)` (read from 'data/approximation_degree{m}_points{n}.dat').
 * 3. The discrete sample points (x_i, y_i) used for the approximation (read from 'data/sample_points_n{n}.dat').
 * Output plots (.png) are saved in the 'plots/' directory with names like 'plots/approximation_m{m}_n{n}.png'.
 * The script uses Gnuplot's `do for` loops to iterate through n and m,
 * and includes commands to ensure the 'plots/' and 'data/' directories exist.
 *
 * @param min_n Minimum number of sample points included in the data.
 * @param max_n Maximum number of sample points included in the data.
 * @param max_m Maximum polynomial degree included in the data.
 */
void generateAllIndividualApproxScripts(int min_n, int max_n, int max_m) {
    char script_path[256];
    snprintf(script_path, sizeof(script_path), "scripts/plot_all_approximations.gp");

    FILE *gnuplot_script = fopen(script_path, "w");
    if (gnuplot_script == NULL) {
        fprintf(stderr,"Error [generateAllIndividualApproxScripts]: Cannot open file %s for writing.\n", script_path);
        return;
    }

    // --- Common Gnuplot Settings ---
    fprintf(gnuplot_script, "# Gnuplot script: Plot individual approximation results for all (n, m) combinations\n");
    fprintf(gnuplot_script, "# Generated by: C program (main.c -> generateAllIndividualApproxScripts)\n\n"); // Clarify generator
    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1200,800 font 'Arial,12'\n");
    fprintf(gnuplot_script, "set grid\n");
    fprintf(gnuplot_script, "set key top right outside spacing 1.1\n");
    fprintf(gnuplot_script, "set xlabel 'x'\n");
    fprintf(gnuplot_script, "set ylabel 'f(x), P_m(x)'\n");
    fprintf(gnuplot_script, "set xrange [%.4f:%.4f]\n", a, b);
    fprintf(gnuplot_script, "set yrange [-15:15]\n"); // Adjust if needed
    fprintf(gnuplot_script, "system 'mkdir -p plots data'\n\n");

    // --- Gnuplot Loops ---
    fprintf(gnuplot_script, "do for [n=%d:%d] {\n", min_n, max_n);
    fprintf(gnuplot_script, "    do for [m=0:%d] {\n", max_m);
    fprintf(gnuplot_script, "        if (m < n) {\n");
    fprintf(gnuplot_script, "            sample_file = sprintf(\"data/sample_points_n%%d.dat\", n)\n");
    fprintf(gnuplot_script, "            approx_file = sprintf(\"data/approximation_degree%%d_points%%d.dat\", m, n)\n");
    fprintf(gnuplot_script, "            set output sprintf('plots/approximation_m%%d_n%%d.png', m, n)\n");
    fprintf(gnuplot_script, "            set title sprintf(\"Least Squares Approximation (n=%%d points, degree m=%%d)\", n, m)\n");
    fprintf(gnuplot_script, "            plot 'data/original_function_plot.dat' with lines dashtype 2 lw 3 lc rgb 'blue' title 'Original function f(x)', \\\n");
    fprintf(gnuplot_script, "                 approx_file with lines lw 3 lc rgb 'red' title sprintf('Approximating P_{%%d}(x)', m), \\\n");
    fprintf(gnuplot_script, "                 sample_file with points pt 7 ps 1.5 lc rgb 'black' title 'Sample points (x_i, y_i)'\n");
    fprintf(gnuplot_script, "        }\n"); // End if (m < n)
    fprintf(gnuplot_script, "    }\n"); // End do for m
    fprintf(gnuplot_script, "}\n"); // End do for n

    fclose(gnuplot_script);
    printf("Generated Gnuplot script for all individual approximations: %s\n", script_path);
}