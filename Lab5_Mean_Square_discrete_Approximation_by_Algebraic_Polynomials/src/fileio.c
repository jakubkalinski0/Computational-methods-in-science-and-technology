/**
 * @file fileio.c
 * @brief Implementation of file input/output operations for polynomial approximation analysis.
 *
 * This file contains functions for:
 * - Saving (x, y) data points to files (e.g., original function, approximated polynomial).
 * - Saving the sample points used for the approximation.
 * - Saving calculated approximation errors (max absolute, MSE) vs. polynomial degree to a CSV file (optional, from old code).
 * - Appending errors to a heatmap CSV file.
 * - Generating Gnuplot scripts to visualize the approximation results (all individual plots, heatmap) and error trends (heatmap).
 * It assumes a standard directory structure ('data/', 'scripts/', 'plots/'). Gnuplot scripts
 * include commands to create these directories if they don't exist.
 */
#include "../include/fileio.h" // Function prototypes and common includes (common.h)
#include <stdio.h>             // Standard I/O functions (fopen, fprintf, fclose, FILE)
#include <stdlib.h>            // Standard library (e.g., for system() if needed, though avoided here)
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
 * `leastSquaresApprox` function.
 *
 * @param filename The base name of the file (e.g., "sample_points_n50.dat"). Path becomes "data/<filename>".
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
    // Check if the file pointer is valid
    if (file == NULL) {
        fprintf(stderr, "Error [appendErrorToHeatmapFile]: Invalid file pointer.\n");
        return;
    }

    // Write n, m, max_error, and mse_error to the file.
    // Use %.10e for scientific notation with good precision.
    // Check for NAN values explicitly as fprintf("%f", NAN) is implementation-defined;
    // standard requires "NAN" output for %a, but %.10e might vary. Writing "NAN" string is safer.
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
    fprintf(gnuplot_script, "# Generated by: generateAllIndividualApproxScripts\n\n");
    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1200,800 font 'Arial,12'\n"); // Output format
    fprintf(gnuplot_script, "set grid\n");                         // Enable grid lines
    fprintf(gnuplot_script, "set key top right outside spacing 1.1\n"); // Legend position and spacing
    fprintf(gnuplot_script, "set xlabel 'x'\n");                   // X-axis label
    fprintf(gnuplot_script, "set ylabel 'f(x), P_m(x)'\n");        // Y-axis label
    // Set x-range based on the global interval [a, b] from common.h
    fprintf(gnuplot_script, "set xrange [%.4f:%.4f]\n", a, b);
    // Set a fixed y-range for consistency; adjust if function values vary significantly.
    fprintf(gnuplot_script, "set yrange [-15:15]\n");              // Example range, adjust as needed
    // Use Gnuplot's system command to ensure output directories exist
    fprintf(gnuplot_script, "system 'mkdir -p plots data'\n\n");

    // --- Gnuplot Loops to Generate Plots for Each (n, m) ---
    // Iterate through n values
    fprintf(gnuplot_script, "do for [n=%d:%d] {\n", min_n, max_n);
    // Iterate through m values
    fprintf(gnuplot_script, "    do for [m=0:%d] {\n", max_m);

    // Check if m < n, which is required for the least squares calculation to be valid
    // Note: Gnuplot checks this as a string comparison, which works for integers.
    // We also need to check if the data file for this (n,m) exists, but it's simpler
    // to rely on the C code to only *create* the file if m < n and calculation succeeds.
    // Gnuplot will silently skip plotting a non-existent file.
    fprintf(gnuplot_script, "        if (m < n) {\n");

    // Define data file names for this specific n and m
    fprintf(gnuplot_script, "            sample_file = sprintf(\"data/sample_points_n%%d.dat\", n)\n");
    fprintf(gnuplot_script, "            approx_file = sprintf(\"data/approximation_degree%%d_points%%d.dat\", m, n)\n");

    // Define output PNG filename for this specific n and m
    fprintf(gnuplot_script, "            set output sprintf('plots/approximation_m%%d_n%%d.png', m, n)\n");

    // Set plot title dynamically based on n and m
    fprintf(gnuplot_script, "            set title sprintf(\"Least Squares Approximation (n=%%d points, degree m=%%d)\", n, m)\n");

    // Gnuplot plot command:
    // 1. Original function (dashed blue line) - this file is the same for all plots
    // 2. Approximating polynomial P_m(x) (solid red line) - use approx_file variable
    // 3. Sample points (black points) - use sample_file variable
    fprintf(gnuplot_script, "            plot 'data/original_function_plot.dat' with lines dashtype 2 lw 3 lc rgb 'blue' title 'Original function f(x)', \\\n");
    fprintf(gnuplot_script, "                 approx_file with lines lw 3 lc rgb 'red' title sprintf('Approximating P_{%%d}(x)', m), \\\n");
    fprintf(gnuplot_script, "                 sample_file with points pt 7 ps 1.5 lc rgb 'black' title 'Sample points (x_i, y_i)'\n"); // pt 7=circle, ps=point size

    fprintf(gnuplot_script, "        }\n"); // End if (m < n)
    fprintf(gnuplot_script, "    }\n"); // End do for m
    fprintf(gnuplot_script, "}\n"); // End do for n

    fclose(gnuplot_script); // Close the script file
    printf("\nGenerated Gnuplot script for all individual approximations: %s\n", script_path);
}


/**
 * @brief Generates Gnuplot script for plotting the Max Absolute Error heatmap.
 *
 * The plot shows Max Absolute Error as a function of 'm' (x-axis)
 * and 'n' (y-axis, reversed). The color scale (z-axis) is logarithmic
 * with a fixed minimum to better show variation in small errors.
 *
 * @param min_n Minimum number of sample points in the data.
 * @param max_n Maximum number of sample points in the data.
 * @param max_m Maximum polynomial degree in the data.
 */
void generateApproxMaxErrorHeatmapScript(int min_n, int max_n, int max_m) {
    char script_path[256];
    snprintf(script_path, sizeof(script_path), "scripts/plot_approx_max_error_heatmap.gp");

    FILE *gnuplot_script = fopen(script_path, "w");
    if (gnuplot_script == NULL) {
        fprintf(stderr,"Error [generateApproxMaxErrorHeatmapScript]: Cannot open file %s for writing.\n", script_path);
        return;
    }

    fprintf(gnuplot_script, "# Gnuplot script: Heatmap of Max Absolute Error vs. m and n\n"); // Updated comment
    fprintf(gnuplot_script, "# Generated by: generateApproxMaxErrorHeatmapScript\n\n");
    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1200,800 font 'Arial,12'\n");
    fprintf(gnuplot_script, "set output 'plots/approximation_max_error_heatmap.png'\n"); // Output file name
    fprintf(gnuplot_script, "set title 'Maximum Error - Approximation (Axes Reversed)'\n"); // Plot title

    fprintf(gnuplot_script, "set xlabel 'Approximation Degree (m)'\n"); // X-axis label
    fprintf(gnuplot_script, "set ylabel 'Number of points (n)'\n");      // Y-axis label

    // Set axis ranges
    // m from 0 to max_m
    fprintf(gnuplot_script, "set xrange [-0.5:%d]\n", max_m);
    // n from max_n down to min_n-1 (reversed Y axis)
    fprintf(gnuplot_script, "set yrange [%d:%d]\n", max_n, min_n - 1);

    fprintf(gnuplot_script, "set grid\n");

    // Set logarithmic scale for the color (error)
    fprintf(gnuplot_script, "set logscale cb\n");
    // Set the range for the color bar using automatic data range
    // [*:*] tells Gnuplot to determine the range automatically from the data.
    fprintf(gnuplot_script, "set cbrange [*:*]\n"); // From the minimum found error to the maximum found error
    fprintf(gnuplot_script, "set cblabel 'Maximum Error (Log Scale)'\n"); // Color bar label
    fprintf(gnuplot_script, "set format cb \"10^{%%L}\"\n"); // Format color bar labels in 10^x notation

    // Define a custom color palette (e.g., from black to yellow)
    fprintf(gnuplot_script, "set palette defined ( 0 \"black\", 0.25 \"blue\", 0.5 \"magenta\", 0.75 \"orange\", 1 \"yellow\" )\n");
    // Configure pm3d for drawing a heatmap. 'interpolate 10,10' smooths the colors.
    fprintf(gnuplot_script, "set pm3d map interpolate 10,10\n");

    // Set view as map (2D top-down projection)
    fprintf(gnuplot_script, "set view map\n");

    // Set data separator in CSV file to comma
    fprintf(gnuplot_script, "set datafile separator ','\n");

    // Use splot to draw from a 3D data file (or 2D data with color mapped from a column)
    // 'data/approximation_heatmap_errors.csv': specifies the data file
    // 'using 2:1:3': specifies which columns to use: X=column 2 (m), Y=column 1 (n), Z(color)=column 3 (MaxError)
    // 'with pm3d': tells Gnuplot to draw using the pm3d colors (creating the heatmap)
    // 'notitle': prevents Gnuplot from adding an automatic legend entry for this data
    // The '\' indicates that the command continues on the next line.
    fprintf(gnuplot_script, "splot 'data/approximation_heatmap_errors.csv' using 2:1:3 with pm3d notitle, \\\n");
    // Second plot element: Draws lines to represent a grid overlay.
    // '' : refers to the same data file as the previous part of the splot command.
    // 'using 2:1:(0)': uses columns 2 (m) and 1 (n) for X and Y, but sets Z coordinate to 0
    // 'with lines': connects points with lines
    // 'lc rgb 'white' lw 0.25': sets line color to white and line width to 0.25
    // 'notitle': prevents legend entry
    // This creates a subtle white grid aligned with the data points on the heatmap.
    fprintf(gnuplot_script, "      '' using 2:1:(0) with lines lc rgb 'white' lw 0.25 notitle\n");

    // Ensure directories exist (safer in C, but can also be done in Gnuplot)
    fprintf(gnuplot_script, "system 'mkdir -p plots data'\n");

    fclose(gnuplot_script);
    printf("Generated Gnuplot script for Max Error heatmap: %s\n", script_path);
}

/**
 * @brief Generates Gnuplot script for plotting the Mean Squared Error heatmap.
 *
 * The plot shows MSE as a function of 'm' (x-axis)
 * and 'n' (y-axis, reversed). The color scale (z-axis) is logarithmic
 * with a fixed minimum to better show variation in small errors.
 *
 * @param min_n Minimum number of sample points in the data.
 * @param max_n Maximum number of sample points in the data.
 * @param max_m Maximum polynomial degree in the data.
 */
void generateApproxMseHeatmapScript(int min_n, int max_n, int max_m) {
    char script_path[256];
    snprintf(script_path, sizeof(script_path), "scripts/plot_approx_mse_heatmap.gp");

    FILE *gnuplot_script = fopen(script_path, "w");
    if (gnuplot_script == NULL) {
        fprintf(stderr,"Error [generateApproxMseHeatmapScript]: Cannot open file %s for writing.\n", script_path);
        return;
    }

    fprintf(gnuplot_script, "# Gnuplot script: Heatmap of Mean Squared Error vs. m and n\n"); // Updated comment
    fprintf(gnuplot_script, "# Generated by: generateApproxMseHeatmapScript\n\n");
    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1200,800 font 'Arial,12'\n");
    fprintf(gnuplot_script, "set output 'plots/approximation_mse_heatmap.png'\n"); // Output file name
    fprintf(gnuplot_script, "set title 'Mean Squared Error (MSE) - Approximation (Axes Reversed)'\n"); // Plot title

    fprintf(gnuplot_script, "set xlabel 'Approximation Degree (m)'\n"); // X-axis label
    fprintf(gnuplot_script, "set ylabel 'Number of points (n)'\n");      // Y-axis label

    // Set axis ranges
    // m from 0 to max_m
    fprintf(gnuplot_script, "set xrange [-0.5:%d]\n", max_m);
    // n from max_n down to min_n-1 (reversed Y axis)
    fprintf(gnuplot_script, "set yrange [%d:%d]\n", max_n, min_n - 1);

    fprintf(gnuplot_script, "set grid\n");

    // Set logarithmic scale for the color (error)
    fprintf(gnuplot_script, "set logscale cb\n");
    // Set the range for the color bar using automatic data range
    // [*:*] tells Gnuplot to determine the range automatically from the data.
    fprintf(gnuplot_script, "set cbrange [*:*]\n"); // From the minimum found error to the maximum found error
    fprintf(gnuplot_script, "set cblabel 'Mean Squared Error (Log Scale)'\n"); // Color bar label
    fprintf(gnuplot_script, "set format cb \"10^{%%L}\"\n"); // Format color bar labels in 10^x notation

    // Define a custom color palette (e.g., from black to yellow)
    fprintf(gnuplot_script, "set palette defined ( 0 \"black\", 0.25 \"blue\", 0.5 \"magenta\", 0.75 \"orange\", 1 \"yellow\" )\n");
    // Configure pm3d for drawing a heatmap. 'interpolate 10,10' smooths the colors.
    fprintf(gnuplot_script, "set pm3d map interpolate 10,10\n");

    // Set view as map
    fprintf(gnuplot_script, "set view map\n");

    // Set data separator in CSV file
    fprintf(gnuplot_script, "set datafile separator ','\n");

    // Use splot to draw from a 3D data file (or 2D data with color mapped from a column)
    // 'data/approximation_heatmap_errors.csv': specifies the data file
    // 'using 2:1:3': specifies which columns to use: X=column 2 (m), Y=column 1 (n), Z(color)=column 3 (MaxError)
    // 'with pm3d': tells Gnuplot to draw using the pm3d colors (creating the heatmap)
    // 'notitle': prevents Gnuplot from adding an automatic legend entry for this data
    // The '\' indicates that the command continues on the next line.
    fprintf(gnuplot_script, "splot 'data/approximation_heatmap_errors.csv' using 2:1:4 with pm3d notitle, \\\n");
    // Second plot element: Draws lines to represent a grid overlay.
    // '' : refers to the same data file as the previous part of the splot command.
    // 'using 2:1:(0)': uses columns 2 (m) and 1 (n) for X and Y, but sets Z coordinate to 0
    // 'with lines': connects points with lines
    // 'lc rgb 'white' lw 0.25': sets line color to white and line width to 0.25
    // 'notitle': prevents legend entry
    // This creates a subtle white grid aligned with the data points on the heatmap.
    fprintf(gnuplot_script, "      '' using 2:1:(0) with lines lc rgb 'white' lw 0.25 notitle\n");

    // Ensure directories exist
    fprintf(gnuplot_script, "system 'mkdir -p plots data'\n");

    fclose(gnuplot_script);
    printf("Generated Gnuplot script for MSE heatmap: %s\n", script_path);
}