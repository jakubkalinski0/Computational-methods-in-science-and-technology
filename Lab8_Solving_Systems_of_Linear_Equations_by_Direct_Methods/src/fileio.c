/**
 * @file fileio.c
 * @brief Implementation of file input/output operations for spline interpolation analysis.
 *
 * This file contains functions for saving data points, interpolation nodes,
 * calculated errors (maximum absolute and mean squared error) to files,
 * and for generating Gnuplot scripts to visualize the spline interpolation
 * results and error trends. It handles directory creation and formatting
 * data for storage and plotting.
 */
#include "../include/fileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // For isnan, isinf
#include "../include/common.h" // For a, b constants used in Gnuplot scripts

/**
 * @brief Ensures that a specified directory path exists. If not, it attempts to create it.
 *
 * Uses the `mkdir -p` command, which creates parent directories as needed
 * and doesn't error if the directory already exists.
 * Prints a warning if the command execution fails.
 *
 * @param path The directory path to check/create (e.g., "data", "scripts/subdir").
 */
void ensure_directory_exists(const char* path) {
    char command[300];
    // Use sprintf to create the shell command "mkdir -p <path>"
    sprintf(command, "mkdir -p %s", path);
    // Execute the command using system()
    int ret = system(command);
    // Check the return status of the system command
    if (ret != 0) {
        // Print a warning if the command might have failed
        fprintf(stderr, "Warning: Could not execute command '%s'. Directory '%s' might not exist.\n", command, path);
    }
}

/**
 * @brief Saves a set of (x, y) data points to a file within the 'data/' directory.
 *
 * Ensures the 'data/' directory exists before attempting to write the file.
 * Handles NaN values in `y` by writing "nan" instead of a number.
 *
 * @param filename The base name of the file (e.g., "original_function.dat").
 * @param x Array of x-coordinates.
 * @param y Array of y-coordinates.
 * @param n The number of data points.
 */
void saveDataToFile(const char* filename, double x[], double y[], int n) {
    char filepath[256];
    // Construct the full path including the subdirectory
    sprintf(filepath, "data/%s", filename);

    FILE *file = fopen(filepath, "w"); // Open file for writing
    if (file == NULL) {
        // Print error message if file cannot be opened
        printf("Error opening file: %s\n", filepath);
        return;
    }

    // Write each (x, y) pair to the file, one pair per line
    for (int i = 0; i < n; i++) {
        fprintf(file, "%lf %lf\n", x[i], y[i]);
    }

    fclose(file); // Close the file
}

/**
 * @brief Saves the interpolation nodes (x, y) to a file in the 'data/' directory.
 *
 * @param filename The base name of the file (e.g., "uniform_nodes_n5.dat").
 * @param nodes Array of node x-coordinates.
 * @param values Array of node y-coordinates (function values at nodes).
 * @param n The number of nodes.
 */
void saveNodesToFile(const char* filename, double nodes[], double values[], int n) {
    char filepath[256];
    // Construct the full path including the subdirectory
    sprintf(filepath, "data/%s", filename);

    FILE *file = fopen(filepath, "w"); // Open file for writing
    if (file == NULL) {
        // Print error message if file cannot be opened
        printf("Error opening file: %s\n", filepath);
        return;
    }

    // Write each node (x, y) pair to the file, one pair per line
    for (int i = 0; i < n; i++) {
        fprintf(file, "%lf %lf\n", nodes[i], values[i]);
    }

    fclose(file); // Close the file
}


/**
 * @brief Saves spline interpolation errors (max absolute error and MSE) to a CSV file.
 *
 * Ensures the 'data/' directory exists. Creates a CSV file named `<filename_base>.csv`.
 * The file contains columns: Number of Nodes (n), Max Absolute Error, Mean Squared Error.
 * Starts saving data from n=2 nodes up to `maxNodes`.
 * Handles NaN and Infinity values by writing "nan" or "inf".
 *
 * @param filename_base The base name for the output CSV file (e.g., "spline_errors_uniform").
 * @param maxNodes The maximum number of nodes analyzed (results are stored for n=2 to maxNodes).
 * @param errors Array containing the maximum absolute errors for n=2 to `maxNodes`. Array index `i` corresponds to `n = i + 2`.
 * @param mse Array containing the mean squared errors for n=2 to `maxNodes`. Array index `i` corresponds to `n = i + 2`.
 */
void saveSplineErrorsToFile(const char* filename_base, int maxNodes, double errors[], double mse[]) {
    // Ensure the target directory exists
    ensure_directory_exists("data");
    char filepath[256];
    // Construct the full path: "data/<filename_base>.csv"
    sprintf(filepath, "data/%s.csv", filename_base);

    // Open the CSV file for writing
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        // Print error if file opening fails
        fprintf(stderr, "Error opening file for writing: %s\n", filepath);
        return;
    }

    // Write the CSV header
    fprintf(file, "NumNodes,MaxAbsoluteError,MeanSquaredError\n");
    // Iterate through the error arrays (indices 0 to maxNodes-2 correspond to n=2 to maxNodes)
    for (int i = 0; i <= maxNodes - 2; i++) {
        int n = i + 2; // Current number of nodes
        fprintf(file, "%d,", n); // Write node count

        // Write Max Absolute Error, handling NaN/Inf
        if (isnan(errors[i])) fprintf(file, "nan,");
        else if (isinf(errors[i])) fprintf(file, "inf,");
        else fprintf(file, "%.10e,", errors[i]); // Format as scientific notation

        // Write Mean Squared Error, handling NaN/Inf
        if (isnan(mse[i])) fprintf(file, "nan\n");
        else if (isinf(mse[i])) fprintf(file, "inf\n");
        else fprintf(file, "%.10e\n", mse[i]); // Format as scientific notation
    }

    fclose(file); // Close the file
}


/**
 * @brief Generates a Gnuplot script to plot a comparison of maximum spline interpolation errors.
 *
 * Creates 'scripts/plot_spline_errors.gp'. This script, when run, generates
 * 'plots/spline_interpolation_errors.png'.
 * The plot shows the maximum absolute error (log scale) vs. the number of nodes (n=2 to maxNodes)
 * for various spline types (cubic natural, cubic clamped, quadratic clamped, quadratic zero-start)
 * and potentially both uniform and Chebyshev node distributions (Chebyshev part is commented out).
 * Data is embedded directly into the Gnuplot script.
 *
 * @param maxNodes The maximum number of nodes analyzed (plots data for n=2 to maxNodes).
 * @param errors_cubic_natural_uniform Max errors for Cubic Natural Spline, Uniform nodes.
 * @param errors_cubic_clamped_uniform Max errors for Cubic Clamped Spline, Uniform nodes.
 * @param errors_quad_clamped_uniform Max errors for Quadratic Clamped Start Spline, Uniform nodes.
 * @param errors_quad_zero_start_uniform Max errors for Quadratic Zero Start Spline, Uniform nodes.
 * @param errors_cubic_natural_chebyshev (Commented out) Max errors for Cubic Natural Spline, Chebyshev nodes.
 * @param errors_cubic_clamped_chebyshev (Commented out) Max errors for Cubic Clamped Spline, Chebyshev nodes.
 * @param errors_quad_clamped_chebyshev (Commented out) Max errors for Quadratic Clamped Start Spline, Chebyshev nodes.
 * @param errors_quad_zero_start_chebyshev (Commented out) Max errors for Quadratic Zero Start Spline, Chebyshev nodes.
 */
void generateSplineErrorPlotScript(int maxNodes,
                                   double errors_cubic_natural_uniform[],
                                   double errors_cubic_clamped_uniform[],
                                   double errors_quad_clamped_uniform[],
                                   double errors_quad_zero_start_uniform[],
                                   double errors_cubic_natural_chebyshev[],
                                   double errors_cubic_clamped_chebyshev[],
                                   double errors_quad_clamped_chebyshev[],
                                   double errors_quad_zero_start_chebyshev[])
{
    // Ensure necessary directories exist
    ensure_directory_exists("scripts");
    ensure_directory_exists("plots");
    char script_path[256];
    // Construct the script file path
    sprintf(script_path, "scripts/plot_spline_errors.gp");

    // Open the Gnuplot script file for writing
    FILE *gnuplot_script = fopen(script_path, "w");
    if (gnuplot_script == NULL) {
        // Print error if file opening fails
        fprintf(stderr, "Cannot open file %s for writing\n", script_path);
        return;
    }

    // --- Gnuplot Script Commands ---
    fprintf(gnuplot_script, "# Gnuplot script to plot spline interpolation errors\n");
    fprintf(gnuplot_script, "# Generated by generateSplineErrorPlotScript()\n\n");

    // Set terminal and output file
    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1400,900 font 'Arial,11'\n");
    fprintf(gnuplot_script, "set output 'plots/spline_interpolation_errors.png'\n");

    // Set plot appearance and labels
    fprintf(gnuplot_script, "set title 'Comparison of Spline Interpolation Errors (Max Absolute Error vs. Nodes)'\n");
    fprintf(gnuplot_script, "set xlabel 'Number of Nodes (n)'\n");
    fprintf(gnuplot_script, "set ylabel 'Maximum Absolute Error'\n");
    fprintf(gnuplot_script, "set grid\n");
    fprintf(gnuplot_script, "set key top right outside spacing 1.1 title 'Spline Type (Node Type)'\n"); // Key outside, descriptive title

    // Set axes scales and format
    fprintf(gnuplot_script, "set logscale y\n");             // Logarithmic y-axis for errors
    fprintf(gnuplot_script, "set format y \"10^{%%L}\"\n");    // Scientific notation for y-axis labels
    fprintf(gnuplot_script, "set xrange [1.8:%d]\n", maxNodes); // X-axis range (starts slightly before n=2)
    // Optional: Set a minimum y-range if errors become very small
    // fprintf(gnuplot_script, "set yrange [1e-10:*]\n");

    // Define the plot command using embedded data ('-')
    // Plot uniform node results first
    fprintf(gnuplot_script, "plot '-' using 1:2 with linespoints pt 7 lc rgb 'blue' title 'Cubic Nat (Uniform)', \\\n");   // pt 7: circle
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 7 lc rgb 'red' title 'Cubic Clamp (Uniform)', \\\n");
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 6 lc rgb 'green' title 'Quad Clamp (Uniform)', \\\n"); // pt 6: square
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 6 lc rgb 'purple' title 'Quad ZeroSt (Uniform)'"); // Add more lines if Chebyshev is uncommented

    // Uncomment the following lines and the corresponding data embedding below to include Chebyshev results
    /*
    fprintf(gnuplot_script, ", \\\n"); // Continue plot command if adding Chebyshev
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 5 lc rgb 'cyan' title 'Cubic Nat (Chebyshev)', \\\n"); // pt 5: triangle
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 5 lc rgb 'orange' title 'Cubic Clamp (Chebyshev)', \\\n");
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 4 lc rgb 'dark-green' title 'Quad Clamp (Chebyshev)', \\\n"); // pt 4: inverted triangle
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 4 lc rgb 'magenta' title 'Quad ZeroSt (Chebyshev)'\n");
    */
    // If Chebyshev is uncommented, remove the semicolon from the last uniform line above and add one here.
    // If only uniform is plotted, end the plot command here.
     fprintf(gnuplot_script, "\n\n"); // End plot command if only uniform is plotted

    // --- Embed Data for Each Plot Series ---
    // Data for Uniform nodes (4 series)
    fprintf(gnuplot_script, "# Data for Cubic Natural (Uniform)\n");
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_cubic_natural_uniform[i]) && !isinf(errors_cubic_natural_uniform[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_cubic_natural_uniform[i]);
    fprintf(gnuplot_script, "e\n"); // Gnuplot end-of-data marker

    fprintf(gnuplot_script, "# Data for Cubic Clamped (Uniform)\n");
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_cubic_clamped_uniform[i]) && !isinf(errors_cubic_clamped_uniform[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_cubic_clamped_uniform[i]);
    fprintf(gnuplot_script, "e\n");

    fprintf(gnuplot_script, "# Data for Quadratic Clamped (Uniform)\n");
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_quad_clamped_uniform[i]) && !isinf(errors_quad_clamped_uniform[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_quad_clamped_uniform[i]);
    fprintf(gnuplot_script, "e\n");

    fprintf(gnuplot_script, "# Data for Quadratic Zero Start (Uniform)\n");
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_quad_zero_start_uniform[i]) && !isinf(errors_quad_zero_start_uniform[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_quad_zero_start_uniform[i]);
    fprintf(gnuplot_script, "e\n");


    // Uncomment the following blocks to embed data for Chebyshev nodes
    /*
    fprintf(gnuplot_script, "# Data for Cubic Natural (Chebyshev)\n");
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_cubic_natural_chebyshev[i]) && !isinf(errors_cubic_natural_chebyshev[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_cubic_natural_chebyshev[i]);
    fprintf(gnuplot_script, "e\n");

    fprintf(gnuplot_script, "# Data for Cubic Clamped (Chebyshev)\n");
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_cubic_clamped_chebyshev[i]) && !isinf(errors_cubic_clamped_chebyshev[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_cubic_clamped_chebyshev[i]);
    fprintf(gnuplot_script, "e\n");

    fprintf(gnuplot_script, "# Data for Quadratic Clamped (Chebyshev)\n");
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_quad_clamped_chebyshev[i]) && !isinf(errors_quad_clamped_chebyshev[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_quad_clamped_chebyshev[i]);
    fprintf(gnuplot_script, "e\n");

    fprintf(gnuplot_script, "# Data for Quadratic Zero Start (Chebyshev)\n");
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_quad_zero_start_chebyshev[i]) && !isinf(errors_quad_zero_start_chebyshev[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_quad_zero_start_chebyshev[i]);
    fprintf(gnuplot_script, "e\n");
    */

    // --- End of Gnuplot Script ---
    fclose(gnuplot_script); // Close the script file
    printf("Generated Gnuplot error comparison script: %s\n", script_path); // Confirmation message
}

/**
 * @brief Generates a Gnuplot script to plot individual spline interpolation results.
 *
 * Creates 'scripts/plot_spline_interpolation.gp'. This script, when run, generates
 * multiple PNG files in the 'plots/' directory. Each PNG file shows the original function,
 * the spline interpolation for a specific type (e.g., cubic natural), node distribution
 * (e.g., uniform), and number of nodes (n), along with the interpolation nodes.
 * Loops through n=2 to `maxNodes`. Includes commands for uniform nodes; commands for
 * Chebyshev nodes are present but commented out.
 *
 * @param maxNodes The maximum number of nodes for which plots should be generated (plots for n=2 to maxNodes).
 */
void generateSplineGnuplotScript(int maxNodes) {
    // Ensure necessary directories exist
    ensure_directory_exists("scripts");
    ensure_directory_exists("plots");
    char script_path[256];
    // Construct the script file path
    sprintf(script_path, "scripts/plot_spline_interpolation.gp");

    // Open the Gnuplot script file for writing
    FILE *gnuplot_script = fopen(script_path, "w");
    if (gnuplot_script == NULL) {
        // Print error if file opening fails
        fprintf(stderr, "Cannot open file %s for writing\n", script_path);
        return;
    }

    // --- Gnuplot Script Header and Common Settings ---
    fprintf(gnuplot_script, "# Gnuplot script to plot individual spline interpolations\n");
    fprintf(gnuplot_script, "# Generated by generateSplineGnuplotScript()\n\n");

    // Set terminal type and default plot appearance
    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1200,800 font 'Arial,12'\n");
    fprintf(gnuplot_script, "set grid\n");
    fprintf(gnuplot_script, "set key top left outside\n"); // Position legend outside plot area
    fprintf(gnuplot_script, "set xlabel 'x'\n");
    fprintf(gnuplot_script, "set ylabel 'f(x)'\n");
    // Set x-range based on global constants 'a' and 'b'
    fprintf(gnuplot_script, "set xrange [%.4f:%.4f]\n", a, b);
    // Set a fixed y-range for consistency across plots (adjust if needed)
    fprintf(gnuplot_script, "set yrange [-15:15]\n");

    fprintf(gnuplot_script, "\n# --- Individual Spline Interpolation Plots (n=2 to %d) ---\n", maxNodes);

    // --- Loop through number of nodes (n=2 to maxNodes) ---
    for (int n = 2; n <= maxNodes; n++) {

        // --- Plots for Uniform Nodes ---
        fprintf(gnuplot_script, "\n# --- n=%d, Uniform Nodes ---\n", n);

        // Plot: Cubic Natural Spline (Uniform Nodes)
        fprintf(gnuplot_script, "set output 'plots/cubic_natural_uniform_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Cubic Spline (n=%d, Natural BC, Uniform Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original f(x)', \\\n"); // Original function
        fprintf(gnuplot_script, "     'data/cubic_natural_uniform_n%d.dat' with lines lw 2 lc 'blue' title 'Cubic Spline (Natural)', \\\n", n); // Interpolated spline
        fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n); // Interpolation nodes (pt 7: circle)

        // Plot: Cubic Clamped Spline (Uniform Nodes)
        fprintf(gnuplot_script, "set output 'plots/cubic_clamped_uniform_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Cubic Spline (n=%d, Clamped BC, Uniform Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original f(x)', \\\n");
        fprintf(gnuplot_script, "     'data/cubic_clamped_uniform_n%d.dat' with lines lw 2 lc 'red' title 'Cubic Spline (Clamped)', \\\n", n);
        fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n);

        // Plot: Quadratic Clamped Start Spline (Uniform Nodes)
        fprintf(gnuplot_script, "set output 'plots/quadratic_clamped_uniform_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Quadratic Spline (n=%d, Clamped Start BC, Uniform Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original f(x)', \\\n");
        fprintf(gnuplot_script, "     'data/quadratic_clamped_uniform_n%d.dat' with lines lw 2 lc 'green' title 'Quadratic Spline (Clamped Start)', \\\n", n);
        fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n);

        // Plot: Quadratic Zero Start Derivative Spline (Uniform Nodes)
        fprintf(gnuplot_script, "set output 'plots/quadratic_zero_start_uniform_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Quadratic Spline (n=%d, Zero Start Derivative BC, Uniform Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original f(x)', \\\n");
        fprintf(gnuplot_script, "     'data/quadratic_zero_start_uniform_n%d.dat' with lines lw 2 lc 'purple' title 'Quadratic Spline (Zero Start Deriv.)', \\\n", n);
        fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n);


        // --- Plots for Chebyshev Nodes (Commented Out) ---
        // Uncomment the following section to generate plots for Chebyshev nodes as well.
        /*
        fprintf(gnuplot_script, "\n# --- n=%d, Chebyshev Nodes ---\n", n);

        // Plot: Cubic Natural Spline (Chebyshev Nodes)
        fprintf(gnuplot_script, "set output 'plots/cubic_natural_chebyshev_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Cubic Spline (n=%d, Natural BC, Chebyshev Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original f(x)', \\\n");
        fprintf(gnuplot_script, "     'data/cubic_natural_chebyshev_n%d.dat' with lines lw 2 lc 'cyan' title 'Cubic Spline (Natural)', \\\n", n);
        fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 5 ps 1.5 lc 'black' title 'Nodes'\n", n); // pt 5: triangle

        // Plot: Cubic Clamped Spline (Chebyshev Nodes)
        fprintf(gnuplot_script, "set output 'plots/cubic_clamped_chebyshev_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Cubic Spline (n=%d, Clamped BC, Chebyshev Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original f(x)', \\\n");
        fprintf(gnuplot_script, "     'data/cubic_clamped_chebyshev_n%d.dat' with lines lw 2 lc 'orange' title 'Cubic Spline (Clamped)', \\\n", n);
        fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 5 ps 1.5 lc 'black' title 'Nodes'\n", n);

        // Plot: Quadratic Clamped Start Spline (Chebyshev Nodes)
        fprintf(gnuplot_script, "set output 'plots/quadratic_clamped_chebyshev_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Quadratic Spline (n=%d, Clamped Start BC, Chebyshev Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original f(x)', \\\n");
        fprintf(gnuplot_script, "     'data/quadratic_clamped_chebyshev_n%d.dat' with lines lw 2 lc 'dark-green' title 'Quadratic Spline (Clamped Start)', \\\n", n);
        fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 5 ps 1.5 lc 'black' title 'Nodes'\n", n);

        // Plot: Quadratic Zero Start Derivative Spline (Chebyshev Nodes)
        fprintf(gnuplot_script, "set output 'plots/quadratic_zero_start_chebyshev_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Quadratic Spline (n=%d, Zero Start Derivative BC, Chebyshev Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original f(x)', \\\n");
        fprintf(gnuplot_script, "     'data/quadratic_zero_start_chebyshev_n%d.dat' with lines lw 2 lc 'magenta' title 'Quadratic Spline (Zero Start Deriv.)', \\\n", n);
        fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 5 ps 1.5 lc 'black' title 'Nodes'\n", n);
        */

    } // End loop over n

    // --- End of Gnuplot Script ---
    fclose(gnuplot_script); // Close the script file
    printf("Generated Gnuplot individual interpolation script: %s\n", script_path); // Confirmation message
}