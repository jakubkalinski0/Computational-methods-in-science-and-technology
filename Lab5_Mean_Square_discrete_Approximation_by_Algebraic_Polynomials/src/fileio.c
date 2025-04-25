/**
 * @file fileio.c
 * @brief Implementation of file input/output operations for polynomial approximation analysis.
 *
 * This file contains functions for:
 * - Saving (x, y) data points to files (e.g., original function, approximated polynomial).
 * - Saving the sample points used for the approximation.
 * - Saving calculated approximation errors (max absolute, MSE) vs. polynomial degree to a CSV file.
 * - Generating Gnuplot scripts to visualize the approximation results and error trends.
 * It assumes a standard directory structure ('data/', 'scripts/', 'plots/'). Gnuplot scripts
 * include commands to create these directories if they don't exist.
 */
#include "../include/fileio.h" // Function prototypes and common includes (common.h)
#include <stdio.h>             // Standard I/O functions (fopen, fprintf, fclose)
#include <stdlib.h>            // Standard library (e.g., for system() if needed, though avoided here)
#include <string.h>            // For string manipulation (like sprintf)
// Global constants 'a' and 'b' from common.h (via fileio.h) are used in Gnuplot script generation.

/**
 * @brief Saves a set of (x, y) data points to a file within the 'data/' directory.
 *
 * Constructs the full file path by prepending "data/" to the provided `filename`.
 * Opens the file for writing and writes each (x, y) pair on a separate line,
 * formatted as "%lf %lf\n". Handles file opening errors by printing a message
 * to stderr and returning.
 *
 * @param filename The base name of the file (e.g., "original_function.dat"). Path becomes "data/<filename>".
 * @param x Array of x-coordinates.
 * @param y Array of y-coordinates.
 * @param n The number of data points to save.
 */
void saveDataToFile(const char* filename, double x[], double y[], int n) {
    char filepath[256]; // Buffer for the full file path
    // Construct the full path with the "data/" subdirectory prefix.
    // Using snprintf is safer against buffer overflows than sprintf.
    snprintf(filepath, sizeof(filepath), "data/%s", filename);

    FILE *file = fopen(filepath, "w"); // Open the file in write mode ("w")
    if (file == NULL) {
        // Print an error message to standard error if the file could not be opened.
        fprintf(stderr, "Error [saveDataToFile]: Could not open file: %s\n", filepath);
        return; // Exit the function if file opening failed
    }

    // Write each data point (x[i], y[i]) to the file, one point per line.
    for (int i = 0; i < n; i++) {
        fprintf(file, "%lf %lf\n", x[i], y[i]); // Format: x_value<space>y_value<newline>
    }

    fclose(file); // Close the file stream
}

/**
 * @brief Saves the sample points (x, y) used for approximation to a file in the 'data/' directory.
 *
 * Functionally identical to `saveDataToFile`, but conceptually used for the specific
 * purpose of saving the discrete (x_i, y_i) points used as input to the
 * `leastSquaresApprox` function. The name `saveNodesToFile` is retained for compatibility
 * but refers to "sample points" in the context of approximation.
 *
 * @param filename The base name of the file (e.g., "sample_points.dat"). Path becomes "data/<filename>".
 * @param points_x Array of sample point x-coordinates.
 * @param points_y Array of sample point y-coordinates.
 * @param n The number of sample points.
 */
void saveNodesToFile(const char* filename, double points_x[], double points_y[], int n) {
    char filepath[256];
    // Construct the full path, ensuring it's within the 'data/' directory.
    snprintf(filepath, sizeof(filepath), "data/%s", filename);

    FILE *file = fopen(filepath, "w"); // Open file for writing
    if (file == NULL) {
        fprintf(stderr, "Error [saveNodesToFile]: Could not open file: %s\n", filepath);
        return;
    }

    // Write each sample point (x, y) pair to the file.
    for (int i = 0; i < n; i++) {
        fprintf(file, "%lf %lf\n", points_x[i], points_y[i]);
    }

    fclose(file); // Close the file
}


/**
 * @brief Saves approximation errors (max absolute and MSE) vs. degree to a CSV file.
 *
 * Creates or overwrites the specified CSV file. The filename provided should ideally
 * include the path (e.g., "data/approximation_errors_vs_degree.csv").
 * Writes a header row "Degree,MaxAbsoluteError,MeanSquaredError".
 * Then, for each degree `m` from 0 to `maxDegree`, writes a row containing
 * `m`, the corresponding `max_errors[m]`, and `mse_errors[m]`.
 * Errors are formatted using scientific notation (`%.10e`) for precision and readability.
 *
 * @param filename The full or relative path name for the output CSV file.
 * @param maxDegree The highest degree included in the error arrays (index `maxDegree`).
 * @param max_errors Array storing max absolute errors, indexed 0 to `maxDegree`.
 * @param mse_errors Array storing mean squared errors, indexed 0 to `maxDegree`.
 * @return 0 on success, -1 if the file could not be opened for writing.
 */
int saveApproximationErrorsToFile(const char* filename, int maxDegree, double max_errors[], double mse_errors[]) {
    // Open the specified file for writing.
    FILE *err_file = fopen(filename, "w");
    if (err_file == NULL) {
        // Use stderr for system-level error messages.
        fprintf(stderr, "Error [saveApproximationErrorsToFile]: Cannot open file '%s' for writing approximation errors.\n", filename);
        return -1; // Return error code
    }

    // Write the header row for the CSV file. Using English headers for broad compatibility (e.g., with Gnuplot).
    fprintf(err_file, "Degree,MaxAbsoluteError,MeanSquaredError\n");

    // Write the error data for each polynomial degree from 0 up to maxDegree.
    for (int m_deg = 0; m_deg <= maxDegree; m_deg++) {
         // Write degree, max absolute error, mean squared error.
         // Format with %.10e for good precision with potentially small or large error values.
         // Handle potential NaN values gracefully if they exist in the arrays (Gnuplot often skips these).
         fprintf(err_file, "%d,%.10e,%.10e\n", m_deg, max_errors[m_deg], mse_errors[m_deg]);
    }

    // Close the file stream.
    fclose(err_file);

    // Optional: Confirmation message (often handled in the calling function, e.g., main).
    // printf("Approximation errors vs. degree saved to %s\n", filename);

    return 0; // Indicate success
}


/**
 * @brief Generates a Gnuplot script ('scripts/plot_approximations.gp') to plot individual approximation results.
 *
 * Creates the Gnuplot script file. When executed, this script generates multiple PNG images,
 * one for each polynomial degree `m` from 0 to `maxDegree`. Each plot compares the
 * original function, the approximating polynomial P_m(x), and the sample points used.
 * Assumes data files (`original_function.dat`, `approximation_degree*.dat`, `sample_points.dat`)
 * are located in the 'data/' directory. Output plots are saved to the 'plots/' directory.
 * Includes Gnuplot `system` command to ensure 'data' and 'plots' directories exist.
 *
 * @param maxDegree The maximum polynomial degree `m` for which plots are generated.
 * @param numSamplePoints The number of sample points `n` used, included in output filenames and plot titles.
 */
void generateApproxGnuplotScript(int maxDegree, int numSamplePoints) {
    char script_path[256];
    // Define the path for the Gnuplot script file within the 'scripts/' directory.
    snprintf(script_path, sizeof(script_path), "scripts/plot_approximations.gp");

    FILE *gnuplot_script = fopen(script_path, "w"); // Open script file for writing
    if (gnuplot_script == NULL) {
        fprintf(stderr,"Error [generateApproxGnuplotScript]: Cannot open file %s for writing.\n", script_path);
        return;
    }

    // --- Common Gnuplot Settings ---
    fprintf(gnuplot_script, "# Gnuplot script: Plot individual approximation results\n");
    fprintf(gnuplot_script, "# Generated by: generateApproxGnuplotScript\n\n");
    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1200,800 font 'Arial,12'\n"); // Output format
    fprintf(gnuplot_script, "set grid\n");                         // Enable grid lines
    fprintf(gnuplot_script, "set key top right outside spacing 1.1\n"); // Legend position and spacing
    fprintf(gnuplot_script, "set xlabel 'x'\n");                   // X-axis label
    fprintf(gnuplot_script, "set ylabel 'f(x), P_m(x)'\n");        // Y-axis label
    // Set x-range based on the global interval [a, b] from common.h
    fprintf(gnuplot_script, "set xrange [%.4f:%.4f]\n", a, b);
    // Set a fixed y-range for consistency; adjust if function values vary significantly.
    fprintf(gnuplot_script, "set yrange [-15:15]\n");              // Example range, adjust as needed
    // Use Gnuplot's system command to ensure output directories exist (safer than C system call sometimes)
    fprintf(gnuplot_script, "system 'mkdir -p plots data'\n\n");

    // --- Loop Through Degrees 'm' to Generate Plot Commands ---
    for (int m = 0; m <= maxDegree; m++) {
        fprintf(gnuplot_script, "# --- Plot for Degree m = %d ---\n", m);
        // Define output PNG filename for this degree
        fprintf(gnuplot_script, "set output 'plots/approximation_m%d_n%d.png'\n", m, numSamplePoints);
        // Set plot title dynamically based on degree and number of points
        fprintf(gnuplot_script, "set title sprintf(\"Least Squares Approximation (n=%d points, degree m=%d)\", %d, %d)\n", numSamplePoints, m, numSamplePoints, m);

        // Gnuplot plot command:
        // 1. Original function (dashed blue line)
        // 2. Approximating polynomial P_m(x) (solid red line)
        // 3. Sample points (black points)
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 2 lw 3 lc rgb 'blue' title 'Original function f(x)', \\\n");
        fprintf(gnuplot_script, "     'data/approximation_degree%d_points%d.dat' with lines lw 3 lc rgb 'red' title sprintf('Approximating P_{%d}(x)', %d), \\\n", m, numSamplePoints, m, m);
        fprintf(gnuplot_script, "     'data/sample_points.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Sample points (x_i, y_i)'\n\n"); // pt 7=circle, ps=point size
    }
    // --- End of Loop ---

    fclose(gnuplot_script); // Close the script file
    // Confirmation message printed to standard output
    printf("\nGenerated Gnuplot script for approximations: %s\n", script_path);
}

/**
 * @brief Generates a Gnuplot script ('scripts/plot_approx_errors.gp') to plot approximation errors vs. degree.
 *
 * Creates the Gnuplot script file. When executed, this script generates a single PNG image
 * ('plots/approximation_errors.png') showing the Maximum Absolute Error and Mean Squared Error (MSE)
 * as a function of the polynomial degree `m` (from 0 to `maxDegree`).
 * It reads the error data from the CSV file specified (typically 'data/approximation_errors_vs_degree.csv').
 * Uses a logarithmic y-axis to better visualize error trends over potentially large ranges.
 * Includes Gnuplot `system` command to ensure 'data' and 'plots' directories exist.
 *
 * @param maxDegree The maximum polynomial degree `m` to include on the x-axis of the plot.
 */
void generateApproxErrorPlotScript(int maxDegree) {
    char script_path[256];
    // Define the path for the Gnuplot script file within the 'scripts/' directory.
    snprintf(script_path, sizeof(script_path), "scripts/plot_approx_errors.gp");

    FILE *gnuplot_script = fopen(script_path, "w"); // Open script file for writing
    if (gnuplot_script == NULL) {
        fprintf(stderr, "Error [generateApproxErrorPlotScript]: Cannot open file %s for writing.\n", script_path);
        return;
    }

    // --- Gnuplot Script Commands ---
    fprintf(gnuplot_script, "# Gnuplot script: Plot approximation errors vs. polynomial degree\n");
    fprintf(gnuplot_script, "# Generated by: generateApproxErrorPlotScript\n\n");
    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1200,800 font 'Arial,12'\n");
    fprintf(gnuplot_script, "set output 'plots/approximation_errors.png'\n"); // Output PNG filename
    fprintf(gnuplot_script, "set title 'Approximation Errors vs. Polynomial Degree (m)'\n"); // Plot title
    fprintf(gnuplot_script, "set xlabel 'Polynomial Degree (m)'\n"); // X-axis label
    fprintf(gnuplot_script, "set ylabel 'Error (Log Scale)'\n");      // Y-axis label
    fprintf(gnuplot_script, "set grid\n");                         // Enable grid lines
    fprintf(gnuplot_script, "set key top right\n");                // Legend position
    fprintf(gnuplot_script, "set logscale y\n");                   // Use logarithmic scale for the Y axis (errors)
    // Optional: Set specific formatting for the y-axis labels if needed
    fprintf(gnuplot_script, "set format y \"10^{%%L}\"\n");          // Format y-axis labels as powers of 10

    // Set the x-axis range based on the maximum degree analyzed.
    fprintf(gnuplot_script, "set xrange [0:%d]\n", maxDegree);
    // Optional: Set a minimum y-range to avoid plotting extremely small errors or focusing the plot.
    fprintf(gnuplot_script, "set yrange [1e-10:*]\n");             // Example: show errors from 1e-10 upwards

    // Ensure required directories exist using Gnuplot's system command.
    fprintf(gnuplot_script, "system 'mkdir -p plots data'\n\n");

    // Define the input data file (CSV generated by saveApproximationErrorsToFile)
    const char data_file[] = "data/approximation_errors_vs_degree.csv";

    // Gnuplot plot command:
    // Read data from the CSV file.
    fprintf(gnuplot_script, "# Plot errors reading from CSV file\n");
    fprintf(gnuplot_script, "set datafile separator ','\n");       // Set the column separator to comma for CSV
    // Plot Column 1 (Degree) vs Column 2 (MaxAbsError) and Column 1 vs Column 3 (MSE)
    fprintf(gnuplot_script, "plot '%s' using 1:2 with linespoints pt 7 lc rgb 'red' title 'Maximum Absolute Error', \\\n", data_file); // pt 7: circle
    fprintf(gnuplot_script, "     '%s' using 1:3 with linespoints pt 6 lc rgb 'blue' title 'Mean Squared Error (MSE)'\n", data_file);     // pt 6: square

    fclose(gnuplot_script); // Close the script file
    // Confirmation message printed to standard output
    printf("\nGenerated Gnuplot script for errors: %s\n", script_path);
}