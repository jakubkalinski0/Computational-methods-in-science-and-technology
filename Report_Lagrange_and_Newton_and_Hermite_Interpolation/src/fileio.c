/**
 * @file fileio.c
 * @brief Implementation of file input/output operations.
 */
#include "../include/fileio.h"
#include <stdio.h>
#include <stdlib.h> // Not strictly needed here, but often useful with file I/O

/**
 * @brief Saves a set of (x, y) data points to a file in the 'data/' directory.
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
 * @brief Generates a Gnuplot script to plot the comparison of maximum interpolation errors.
 *
 * Creates 'scripts/plot_errors.gp' to visualize max error vs. number of nodes.
 * Output plot is 'plots/interpolation_errors.png'.
 *
 * @param maxNodes The maximum number of nodes analyzed.
 * @param lagrange_uniform_errors Array of max errors (Lagrange, Uniform).
 * @param lagrange_chebyshev_errors Array of max errors (Lagrange, Chebyshev).
 * @param newton_uniform_errors Array of max errors (Newton, Uniform).
 * @param newton_chebyshev_errors Array of max errors (Newton, Chebyshev).
 * @param hermite_uniform_errors Array of max errors (Hermite, Uniform).
 * @param hermite_chebyshev_errors Array of max errors (Hermite, Chebyshev).
 */
void generateErrorPlotScript(int maxNodes,
                           double lagrange_uniform_errors[],
                           double lagrange_chebyshev_errors[],
                           double newton_uniform_errors[],
                           double newton_chebyshev_errors[],
                           double hermite_uniform_errors[],
                           double hermite_chebyshev_errors[]) {
    char script_path[256];
    // Define the path for the Gnuplot script
    sprintf(script_path, "scripts/plot_errors.gp"); // Path relative to execution directory

    FILE *gnuplot_script = fopen(script_path, "w"); // Open script file for writing
    if (gnuplot_script == NULL) {
        printf("Cannot open file %s for writing\n", script_path);
        return;
    }

    // --- Gnuplot Commands ---
    // Set output format and appearance
    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1200,800 font 'Arial,12'\n");
    // Set output file name
    fprintf(gnuplot_script, "set output 'plots/interpolation_errors.png'\n");
    // Set plot title
    fprintf(gnuplot_script, "set title 'Porównanie błędów interpolacji (maksymalny błąd bezwzględny)'\n"); // English title
    // Set axis labels
    fprintf(gnuplot_script, "set xlabel 'Liczba węzłów (n)'\n"); // English label
    fprintf(gnuplot_script, "set ylabel 'Maksymalny błąd bezwzględny'\n"); // English label
    // Enable grid
    fprintf(gnuplot_script, "set grid\n");
    // Position the legend
    fprintf(gnuplot_script, "set key below\n");
    // Use logarithmic scale for the y-axis (errors often span orders of magnitude)
    fprintf(gnuplot_script, "set logscale y\n");
    // Optional: Set y-axis range if needed (adjust based on expected error values)
    // fprintf(gnuplot_script, "set yrange [1e-16:1e2]\n");

    // Ensure the 'plots' directory exists (Gnuplot specific system command execution)
    fprintf(gnuplot_script, "system 'mkdir -p plots'\n"); // Use system for portability

    // Define the plot command: plot data directly embedded in the script ('-')
    fprintf(gnuplot_script, "plot '-' using 1:2 with linespoints pt 7 lc rgb 'purple' title 'Lagrange (węzły równoodległe)', \\\n"); // English legend
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 4 lc rgb 'magenta' title 'Lagrange (węzły Czebyszewa)', \\\n"); // English legend
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 7 lc rgb 'blue' title 'Newton (węzły równoodległe)', \\\n"); // English legend
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 4 lc rgb 'green' title 'Newton (węzły Czebyszewa)', \\\n"); // English legend
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 7 lc rgb 'orange' title 'Hermite (węzły równoodległe)', \\\n"); // English legend
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 4 lc rgb 'red' title 'Hermite (węzły Czebyszewa)'\n"); // English legend

    // Embed data for Lagrange Uniform errors
    for (int i = 0; i < maxNodes; i++) {
        fprintf(gnuplot_script, "%d %e\n", i+1, lagrange_uniform_errors[i]); // Node count (1 to maxNodes), error
    }
    fprintf(gnuplot_script, "e\n"); // End of data marker for Gnuplot

    // Embed data for Lagrange Chebyshev errors
    for (int i = 0; i < maxNodes; i++) {
        fprintf(gnuplot_script, "%d %e\n", i+1, lagrange_chebyshev_errors[i]);
    }
    fprintf(gnuplot_script, "e\n");

    // Embed data for Newton Uniform errors
    for (int i = 0; i < maxNodes; i++) {
        fprintf(gnuplot_script, "%d %e\n", i+1, newton_uniform_errors[i]);
    }
    fprintf(gnuplot_script, "e\n");

    // Embed data for Newton Chebyshev errors
    for (int i = 0; i < maxNodes; i++) {
        fprintf(gnuplot_script, "%d %e\n", i+1, newton_chebyshev_errors[i]);
    }
    fprintf(gnuplot_script, "e\n");

    // Embed data for Hermite Uniform errors
    for (int i = 0; i < maxNodes; i++) {
        fprintf(gnuplot_script, "%d %e\n", i+1, hermite_uniform_errors[i]); // Node count (1 to maxNodes), error
    }
    fprintf(gnuplot_script, "e\n"); // End of data marker for Gnuplot

    // Embed data for Hermite Chebyshev errors
    for (int i = 0; i < maxNodes; i++) {
        fprintf(gnuplot_script, "%d %e\n", i+1, hermite_chebyshev_errors[i]);
    }
    fprintf(gnuplot_script, "e\n");
    // --- End of Gnuplot Commands ---

    fclose(gnuplot_script); // Close the script file
    printf("\nGenerated Gnuplot script: %s\n", script_path); // Confirmation message
}


/**
 * @brief Saves Lagrange/Uniform interpolation errors (max and MSE) to 'data/lagrange_uniform_errors.csv'.
 * @param maxNodes The maximum number of nodes used.
 * @param errors Array containing the maximum absolute errors for each node count (1 to maxNodes).
 * @param mse Array containing the mean squared errors for each node count (1 to maxNodes).
 */
void saveLagrangeUniformErrorsToFile(int maxNodes, double errors[], double mse[]) {
    FILE *file = fopen("data/lagrange_uniform_errors.csv", "w");
    if (file != NULL) {
        fprintf(file, "NumNodes,MaxAbsoluteError,MeanSquaredError\n"); // CSV Header (English)
        for (int i = 0; i < maxNodes; i++) {
            // Write node count (n), max error, and MSE
            fprintf(file, "%d,%.10e,%.10e\n", i+1, errors[i], mse[i]);
        }
        fclose(file);
    } else {
         printf("Error opening file: data/lagrange_uniform_errors.csv\n");
    }
}

/**
 * @brief Saves Lagrange/Chebyshev interpolation errors (max and MSE) to 'data/lagrange_chebyshev_errors.csv'.
 * @param maxNodes The maximum number of nodes used.
 * @param errors Array containing the maximum absolute errors for each node count (1 to maxNodes).
 * @param mse Array containing the mean squared errors for each node count (1 to maxNodes).
 */
void saveLagrangeChebyshevErrorsToFile(int maxNodes, double errors[], double mse[]) {
    FILE *file = fopen("data/lagrange_chebyshev_errors.csv", "w");
    if (file != NULL) {
        fprintf(file, "NumNodes,MaxAbsoluteError,MeanSquaredError\n"); // CSV Header (English)
        for (int i = 0; i < maxNodes; i++) {
            fprintf(file, "%d,%.10e,%.10e\n", i+1, errors[i], mse[i]);
        }
        fclose(file);
    } else {
         printf("Error opening file: data/lagrange_chebyshev_errors.csv\n");
    }
}

/**
 * @brief Saves Newton/Uniform interpolation errors (max and MSE) to 'data/newton_uniform_errors.csv'.
 * @param maxNodes The maximum number of nodes used.
 * @param errors Array containing the maximum absolute errors for each node count (1 to maxNodes).
 * @param mse Array containing the mean squared errors for each node count (1 to maxNodes).
 */
void saveNewtonUniformErrorsToFile(int maxNodes, double errors[], double mse[]) {
    FILE *file = fopen("data/newton_uniform_errors.csv", "w");
    if (file != NULL) {
        fprintf(file, "NumNodes,MaxAbsoluteError,MeanSquaredError\n"); // CSV Header (English)
        for (int i = 0; i < maxNodes; i++) {
            fprintf(file, "%d,%.10e,%.10e\n", i+1, errors[i], mse[i]);
        }
        fclose(file);
    } else {
         printf("Error opening file: data/newton_uniform_errors.csv\n");
    }
}

/**
 * @brief Saves Newton/Chebyshev interpolation errors (max and MSE) to 'data/newton_chebyshev_errors.csv'.
 * @param maxNodes The maximum number of nodes used.
 * @param errors Array containing the maximum absolute errors for each node count (1 to maxNodes).
 * @param mse Array containing the mean squared errors for each node count (1 to maxNodes).
 */
void saveNewtonChebyshevErrorsToFile(int maxNodes, double errors[], double mse[]) {
    FILE *file = fopen("data/newton_chebyshev_errors.csv", "w");
    if (file != NULL) {
        fprintf(file, "NumNodes,MaxAbsoluteError,MeanSquaredError\n"); // CSV Header (English)
        for (int i = 0; i < maxNodes; i++) {
            fprintf(file, "%d,%.10e,%.10e\n", i+1, errors[i], mse[i]);
        }
        fclose(file);
    } else {
         printf("Error opening file: data/newton_chebyshev_errors.csv\n");
    }
}

/**
 * @brief Saves Hermite/Uniform interpolation errors (max and MSE) to 'data/hermite_uniform_errors.csv'.
 * @param maxNodes The maximum number of nodes used.
 * @param errors Array containing the maximum absolute errors for each node count (1 to maxNodes).
 * @param mse Array containing the mean squared errors for each node count (1 to maxNodes).
 */void saveHermiteUniformErrorsToFile(int maxNodes, double errors[], double mse[]) {
    FILE *file = fopen("data/hermite_uniform_errors.csv", "w");
    if (file != NULL) {
        fprintf(file, "NumNodes,MaxAbsoluteError,MeanSquaredError\n"); // CSV Header (English)
        for (int i = 0; i < maxNodes; i++) {
            // Write node count (n), max error, and MSE
            fprintf(file, "%d,%.10e,%.10e\n", i+1, errors[i], mse[i]);
        }
        fclose(file);
    } else {
        printf("Error opening file: data/hermite_uniform_errors.csv\n");
    }
}

/**
 * @brief Saves Hermite/Chebyshev interpolation errors (max and MSE) to 'data/hermite_chebyshev_errors.csv'.
 * @param maxNodes The maximum number of nodes used.
 * @param errors Array containing the maximum absolute errors for each node count (1 to maxNodes).
 * @param mse Array containing the mean squared errors for each node count (1 to maxNodes).
 */
void saveHermiteChebyshevErrorsToFile(int maxNodes, double errors[], double mse[]) {
    FILE *file = fopen("data/hermite_chebyshev_errors.csv", "w");
    if (file != NULL) {
        fprintf(file, "NumNodes,MaxAbsoluteError,MeanSquaredError\n"); // CSV Header (English)
        for (int i = 0; i < maxNodes; i++) {
            // Write node count (n), max error, and MSE
            fprintf(file, "%d,%.10e,%.10e\n", i+1, errors[i], mse[i]);
        }
        fclose(file);
    } else {
        printf("Error opening file: data/hermite_uniform_errors.csv\n");
    }
}

/**
 * @brief Generates a Gnuplot script ('scripts/plot_interpolation.gp') to plot individual interpolation results.
 *
 * Creates plots for each combination of method, node type, and node count (n=1 to maxNodes).
 * Each plot compares the original function, the interpolated function, and shows the nodes.
 * Output plots are saved in the 'plots/' directory.
 *
 * @param maxNodes The maximum number of nodes for which plots should be generated.
 */
void generateGnuplotScript(int maxNodes) {

    char script_path[256];
    // Define the path for the Gnuplot script
    sprintf(script_path, "scripts/plot_interpolation.gp");

    FILE *gnuplot_script = fopen(script_path, "w"); // Open script file for writing
    if (gnuplot_script == NULL) {
        printf("Cannot open file %s for writing\n", script_path);
        return;
    }

    if (gnuplot_script != NULL) {
        // --- Common Gnuplot Settings for all plots in this script ---
        fprintf(gnuplot_script, "set terminal pngcairo size 1200,800\n"); // Output format
        fprintf(gnuplot_script, "set grid\n");                       // Enable grid
        fprintf(gnuplot_script, "set key below\n");                // Place legend outside plot area
        fprintf(gnuplot_script, "set xlabel 'x'\n");                 // X-axis label
        fprintf(gnuplot_script, "set ylabel 'f(x)'\n");              // Y-axis label
        // Set axis ranges based on the function's behavior in the interval [a, b]
        fprintf(gnuplot_script, "set xrange [%.2f:%.2f]\n", a, b);    // Use interval bounds
        // Adjust yrange manually if needed for better visualization
        fprintf(gnuplot_script, "set yrange [-15:15]\n"); // Example range, adjust as necessary
        // Ensure output directories exist (Gnuplot specific system command)
        fprintf(gnuplot_script, "system 'mkdir -p plots data'\n"); // Use system for portability

        // --- Generate individual plots for each node count 'n' ---
        fprintf(gnuplot_script, "# Plots of interpolated functions with nodes\n");

        // Loop through number of nodes from 1 to maxNodes
        for (int n = 1; n <= maxNodes; n++) {

            // --- Plot 1: Lagrange Interpolation with Uniform Nodes ---
            fprintf(gnuplot_script, "set output 'plots/lagrange_uniform_with_nodes_n%d.png'\n", n); // Output filename
            // Set plot title (using C string formatting)
            fprintf(gnuplot_script, "set title \"Lagrange Interpolation (n=%d, Uniform Nodes)\"\n", n); // English title
            // Plot command: original function, interpolated function, nodes
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 3 lw 3 lc rgb 'black' title 'Original Function',\\\n"); // English legend
            fprintf(gnuplot_script, "     'data/lagrange_uniform_n%d.dat' with lines dashtype 2 lw 3 lc rgb 'purple' title 'Lagrange Interpolation',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Interpolation Nodes'\n", n); // English legend

            // --- Plot 2: Lagrange Interpolation with Chebyshev Nodes ---
            fprintf(gnuplot_script, "set output 'plots/lagrange_chebyshev_with_nodes_n%d.png'\n", n);
            fprintf(gnuplot_script, "set title \"Lagrange Interpolation (n=%d, Chebyshev Nodes)\"\n", n); // English title
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 3 lw 3 lc rgb 'black' title 'Original Function',\\\n"); // English legend
            fprintf(gnuplot_script, "     'data/lagrange_chebyshev_n%d.dat' with lines dashtype 2 lw 3 lc rgb 'magenta' title 'Lagrange Interpolation',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Interpolation Nodes'\n", n); // English legend

            // --- Plot 3: Newton Interpolation with Uniform Nodes ---
            fprintf(gnuplot_script, "set output 'plots/newton_uniform_with_nodes_n%d.png'\n", n);
            fprintf(gnuplot_script, "set title \"Newton Interpolation (n=%d, Uniform Nodes)\"\n", n); // English title
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 3 lw 3 lc rgb 'black' title 'Original Function',\\\n"); // English legend
            fprintf(gnuplot_script, "     'data/newton_uniform_n%d.dat' with lines dashtype 4 lw 3 lc rgb 'blue' title 'Newton Interpolation',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Interpolation Nodes'\n", n); // English legend

            // --- Plot 4: Newton Interpolation with Chebyshev Nodes ---
            fprintf(gnuplot_script, "set output 'plots/newton_chebyshev_with_nodes_n%d.png'\n", n);
            fprintf(gnuplot_script, "set title \"Newton Interpolation (n=%d, Chebyshev Nodes)\"\n", n); // English title
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 3 lw 3 lc rgb 'black' title 'Original Function',\\\n"); // English legend
            fprintf(gnuplot_script, "     'data/newton_chebyshev_n%d.dat' with lines dashtype 4 lw 3 lc rgb 'green' title 'Newton Interpolation',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Interpolation Nodes'\n", n); // English legend

            // --- Plot 5: Hermite Interpolation with Uniform Nodes ---
            fprintf(gnuplot_script, "set output 'plots/hermite_uniform_with_nodes_n%d.png'\n", n); // Output filename
            fprintf(gnuplot_script, "set title \"Hermite Interpolation (n=%d, Uniform Nodes)\"\n", n); // English title
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 3 lw 3 lc rgb 'black' title 'Original Function',\\\n"); // English legend
            fprintf(gnuplot_script, "     'data/hermite_uniform_n%d.dat' with lines dashtype 5 lw 3 lc rgb 'orange' title 'Hermite Interpolation',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Interpolation Nodes'\n", n); // English legend

            // --- Plot 6: Hermite Interpolation with Chebyshev Nodes ---
            fprintf(gnuplot_script, "set output 'plots/hermite_chebyshev_with_nodes_n%d.png'\n", n); // Output filename
            fprintf(gnuplot_script, "set title \"Hermite Interpolation (n=%d, Chebyshev Nodes)\"\n", n); // English title
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 3 lw 3 lc rgb 'black' title 'Original Function',\\\n"); // English legend
            fprintf(gnuplot_script, "     'data/hermite_chebyshev_n%d.dat' with lines dashtype 5 lw 3 lc rgb 'red' title 'Hermite Interpolation',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Interpolation Nodes'\n", n); // English legend

            // --- Plot 7: All Interpolations with Uniform Nodes ---
            fprintf(gnuplot_script, "set output 'plots/all_uniform_with_nodes_n%d.png'\n", n); // Output filename
            fprintf(gnuplot_script, "set title \"Wszystkie interpolacje (n=%d, węzły równoodległe)\"\n", n); // English title
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 3 lw 3 lc rgb 'black' title 'Oryginalna funkcja',\\\n"); // English legend
            fprintf(gnuplot_script, "     'data/lagrange_uniform_n%d.dat' with lines dashtype 2 lw 3 lc rgb 'purple' title 'Interpolacja Lagrange`a',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/newton_uniform_n%d.dat' with lines dashtype 4 lw 3 lc rgb 'blue' title 'Interpolacja Newton`a',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/hermite_uniform_n%d.dat' with lines dashtype 5 lw 3 lc rgb 'orange' title 'Interpolacja Hermite`a',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Węzły'\n", n); // English legend

            // --- Plot 8: All Interpolations with Chebyshev Nodes ---
            fprintf(gnuplot_script, "set output 'plots/all_chebyshev_with_nodes_n%d.png'\n", n); // Output filename
            fprintf(gnuplot_script, "set title \"Wszystkie interpolacje (n=%d, węzły Czebyszewa)\"\n", n); // English title
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 3 lw 3 lc rgb 'black' title 'Oryginalna funkcja',\\\n"); // English legend
            fprintf(gnuplot_script, "     'data/lagrange_chebyshev_n%d.dat' with lines dashtype 2 lw 3 lc rgb 'magenta' title 'Interpolacja Lagrange`a',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/newton_chebyshev_n%d.dat' with lines dashtype 4 lw 3 lc rgb 'green' title 'Interpolacja Newton`a',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/hermite_chebyshev_n%d.dat' with lines dashtype 5 lw 3 lc rgb 'red' title 'Interpolacja Hermite`a',\\\n", n); // English legend
            fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Węzły'\n", n); // English legend
        }
        // --- End of loop ---

        fclose(gnuplot_script); // Close the script file
        printf("\nGenerated Gnuplot script: %s\n", script_path); // Confirmation message
    }
}