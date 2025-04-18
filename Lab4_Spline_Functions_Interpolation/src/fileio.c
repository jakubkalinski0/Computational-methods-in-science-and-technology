/**
 * @file fileio.c
 * @brief Implementation of file input/output operations for spline interpolation.
 */
#include "../include/fileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // For isnan, isinf

// ensure_directory_exists, saveDataToFile, saveNodesToFile - bez zmian

void ensure_directory_exists(const char* path) {
    char command[300];
    sprintf(command, "mkdir -p %s", path);
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Warning: Could not execute command '%s'\n", command);
    }
}

void saveDataToFile(const char* filename, double x[], double y[], int n) {
    ensure_directory_exists("data");
    char filepath[256];
    sprintf(filepath, "data/%s", filename);
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing: %s\n", filepath);
        return;
    }
    for (int i = 0; i < n; i++) {
        if (isnan(y[i])) fprintf(file, "%lf nan\n", x[i]);
        else fprintf(file, "%lf %lf\n", x[i], y[i]);
    }
    fclose(file);
}

void saveNodesToFile(const char* filename, double nodes[], double values[], int n) {
    ensure_directory_exists("data");
    char filepath[256];
    sprintf(filepath, "data/%s", filename);
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing: %s\n", filepath);
        return;
    }
    for (int i = 0; i < n; i++) {
         if (isnan(nodes[i]) || isnan(values[i])) {
              fprintf(stderr, "Warning: NaN detected in node data for %s at index %d\n", filename, i);
         } else {
              fprintf(file, "%lf %lf\n", nodes[i], values[i]);
         }
    }
    fclose(file);
}


/**
 * @brief Saves spline interpolation errors (max and MSE) to a CSV file.
 */
void saveSplineErrorsToFile(const char* filename_base, int maxNodes, double errors[], double mse[]) {
    ensure_directory_exists("data");
    char filepath[256];
    sprintf(filepath, "data/%s.csv", filename_base);

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing: %s\n", filepath);
        return;
    }

    fprintf(file, "NumNodes,MaxAbsoluteError,MeanSquaredError\n");
    for (int i = 0; i <= maxNodes - 2; i++) {
        int n = i + 2;
        fprintf(file, "%d,", n);
        if (isnan(errors[i])) fprintf(file, "nan,");
        else if (isinf(errors[i])) fprintf(file, "inf,");
        else fprintf(file, "%.10e,", errors[i]);

        if (isnan(mse[i])) fprintf(file, "nan\n");
        else if (isinf(mse[i])) fprintf(file, "inf\n");
        else fprintf(file, "%.10e\n", mse[i]);
    }
    fclose(file);
    // Removed print statement from here, main will report completion
}


/**
 * @brief Generates Gnuplot script to plot comparison of maximum spline interpolation errors for different node types.
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
    ensure_directory_exists("scripts");
    ensure_directory_exists("plots");
    char script_path[256];
    sprintf(script_path, "scripts/plot_spline_errors.gp");

    FILE *gnuplot_script = fopen(script_path, "w");
    if (gnuplot_script == NULL) {
        fprintf(stderr, "Cannot open file %s for writing\n", script_path);
        return;
    }

    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1400,900 font 'Arial,11'\n"); // Slightly larger canvas
    fprintf(gnuplot_script, "set output 'plots/spline_interpolation_errors.png'\n");
    fprintf(gnuplot_script, "set title 'Comparison of Spline Interpolation Errors (Max Absolute Error vs. Nodes)'\n");
    fprintf(gnuplot_script, "set xlabel 'Number of Nodes (n)'\n");
    fprintf(gnuplot_script, "set ylabel 'Maximum Absolute Error'\n");
    fprintf(gnuplot_script, "set grid\n");
    fprintf(gnuplot_script, "set key top right outside spacing 1.1\n"); // Key outside for clarity
    fprintf(gnuplot_script, "set logscale y\n");
    fprintf(gnuplot_script, "set format y \"10^{%%L}\"\n"); // Scientific notation on y-axis
    fprintf(gnuplot_script, "set xrange [1.8:%d]\n", maxNodes);
    // fprintf(gnuplot_script, "set yrange [1e-10:*]\n"); // Adjust if needed


    // Plot command with 8 entries
    fprintf(gnuplot_script, "plot '-' using 1:2 with linespoints pt 7 lc rgb 'blue' title 'Cubic Nat (Uniform)', \\\n"); // points as circles for Uniform
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 7 lc rgb 'red' title 'Cubic Clamp (Uniform)', \\\n");
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 6 lc rgb 'green' title 'Quad Clamp (Uniform)', \\\n");
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 6 lc rgb 'purple' title 'Quad ZeroSt (Uniform)', \\\n");
//    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 5 lc rgb 'cyan' title 'Cubic Nat (Chebyshev)', \\\n"); // points as squares for Chebyshev
//    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 5 lc rgb 'orange' title 'Cubic Clamp (Chebyshev)', \\\n");
//    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 4 lc rgb 'dark-green' title 'Quad Clamp (Chebyshev)', \\\n");
//    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints pt 4 lc rgb 'magenta' title 'Quad ZeroSt (Chebyshev)'\n");


    // Embed data for Uniform nodes (4 plots)
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_cubic_natural_uniform[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_cubic_natural_uniform[i]);
    fprintf(gnuplot_script, "e\n");
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_cubic_clamped_uniform[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_cubic_clamped_uniform[i]);
    fprintf(gnuplot_script, "e\n");
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_quad_clamped_uniform[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_quad_clamped_uniform[i]);
    fprintf(gnuplot_script, "e\n");
    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_quad_zero_start_uniform[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_quad_zero_start_uniform[i]);
    fprintf(gnuplot_script, "e\n");

//    // Embed data for Chebyshev nodes (4 plots)
//    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_cubic_natural_chebyshev[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_cubic_natural_chebyshev[i]);
//    fprintf(gnuplot_script, "e\n");
//    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_cubic_clamped_chebyshev[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_cubic_clamped_chebyshev[i]);
//    fprintf(gnuplot_script, "e\n");
//    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_quad_clamped_chebyshev[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_quad_clamped_chebyshev[i]);
//    fprintf(gnuplot_script, "e\n");
//    for (int i = 0; i <= maxNodes - 2; i++) if (!isnan(errors_quad_zero_start_chebyshev[i])) fprintf(gnuplot_script, "%d %e\n", i + 2, errors_quad_zero_start_chebyshev[i]);
//    fprintf(gnuplot_script, "e\n");

    fclose(gnuplot_script);
    printf("Generated Gnuplot script: %s\n", script_path);
}


/**
 * @brief Generates Gnuplot script ('scripts/plot_spline_interpolation.gp') to plot individual results.
 */
void generateSplineGnuplotScript(int maxNodes) {
    ensure_directory_exists("scripts");
    ensure_directory_exists("plots");
    char script_path[256];
    sprintf(script_path, "scripts/plot_spline_interpolation.gp");

    FILE *gnuplot_script = fopen(script_path, "w");
    if (gnuplot_script == NULL) {
        fprintf(stderr, "Cannot open file %s for writing\n", script_path);
        return;
    }

    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1200,800 font 'Arial,12'\n");
    fprintf(gnuplot_script, "set grid\n");
    fprintf(gnuplot_script, "set key top left outside\n");
    fprintf(gnuplot_script, "set xlabel 'x'\n");
    fprintf(gnuplot_script, "set ylabel 'f(x)'\n");
    fprintf(gnuplot_script, "set xrange [%.4f:%.4f]\n", a, b);
     fprintf(gnuplot_script, "set yrange [-15:15]\n");

    fprintf(gnuplot_script, "\n# --- Individual Spline Interpolation Plots (n=2 to %d) ---\n", maxNodes);

    for (int n = 2; n <= maxNodes; n++) {

        // --- Plots for Uniform Nodes ---
        fprintf(gnuplot_script, "\n# --- n=%d, Uniform Nodes ---\n", n);
        // Cubic Natural (Uniform)
        fprintf(gnuplot_script, "set output 'plots/cubic_natural_uniform_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Cubic Spline (n=%d, Natural BC, Uniform Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original', \\\n");
        fprintf(gnuplot_script, "     'data/cubic_natural_uniform_n%d.dat' with lines lw 2 lc 'blue' title 'Cubic Spline', \\\n", n);
        fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n);
        // Cubic Clamped (Uniform)
        fprintf(gnuplot_script, "set output 'plots/cubic_clamped_uniform_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Cubic Spline (n=%d, Clamped BC, Uniform Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original', \\\n");
        fprintf(gnuplot_script, "     'data/cubic_clamped_uniform_n%d.dat' with lines lw 2 lc 'red' title 'Cubic Spline', \\\n", n);
        fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n);
        // Quadratic Clamped (Uniform)
        fprintf(gnuplot_script, "set output 'plots/quadratic_clamped_uniform_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Quadratic Spline (n=%d, Clamped Start BC, Uniform Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original', \\\n");
        fprintf(gnuplot_script, "     'data/quadratic_clamped_uniform_n%d.dat' with lines lw 2 lc 'green' title 'Quadratic Spline', \\\n", n);
        fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n);
        // Quadratic Zero Start (Uniform)
        fprintf(gnuplot_script, "set output 'plots/quadratic_zero_start_uniform_n%d.png'\n", n);
        fprintf(gnuplot_script, "set title \"Quadratic Spline (n=%d, Zero Start BC, Uniform Nodes)\"\n", n);
        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original', \\\n");
        fprintf(gnuplot_script, "     'data/quadratic_zero_start_uniform_n%d.dat' with lines lw 2 lc 'purple' title 'Quadratic Spline', \\\n", n);
        fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n);


//        // --- Plots for Chebyshev Nodes ---
//        fprintf(gnuplot_script, "\n# --- n=%d, Chebyshev Nodes ---\n", n);
//        // Cubic Natural (Chebyshev)
//        fprintf(gnuplot_script, "set output 'plots/cubic_natural_chebyshev_n%d.png'\n", n);
//        fprintf(gnuplot_script, "set title \"Cubic Spline (n=%d, Natural BC, Chebyshev Nodes)\"\n", n);
//        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original', \\\n");
//        fprintf(gnuplot_script, "     'data/cubic_natural_chebyshev_n%d.dat' with lines lw 2 lc 'cyan' title 'Cubic Spline', \\\n", n);
//        fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n); // Different point type
//        // Cubic Clamped (Chebyshev)
//        fprintf(gnuplot_script, "set output 'plots/cubic_clamped_chebyshev_n%d.png'\n", n);
//        fprintf(gnuplot_script, "set title \"Cubic Spline (n=%d, Clamped BC, Chebyshev Nodes)\"\n", n);
//        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original', \\\n");
//        fprintf(gnuplot_script, "     'data/cubic_clamped_chebyshev_n%d.dat' with lines lw 2 lc 'orange' title 'Cubic Spline', \\\n", n);
//        fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n);
//        // Quadratic Clamped (Chebyshev)
//        fprintf(gnuplot_script, "set output 'plots/quadratic_clamped_chebyshev_n%d.png'\n", n);
//        fprintf(gnuplot_script, "set title \"Quadratic Spline (n=%d, Clamped Start BC, Chebyshev Nodes)\"\n", n);
//        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original', \\\n");
//        fprintf(gnuplot_script, "     'data/quadratic_clamped_chebyshev_n%d.dat' with lines lw 2 lc 'dark-green' title 'Quadratic Spline', \\\n", n);
//        fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n);
//        // Quadratic Zero Start (Chebyshev)
//        fprintf(gnuplot_script, "set output 'plots/quadratic_zero_start_chebyshev_n%d.png'\n", n);
//        fprintf(gnuplot_script, "set title \"Quadratic Spline (n=%d, Zero Start BC, Chebyshev Nodes)\"\n", n);
//        fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines lw 2 lc 'black' title 'Original', \\\n");
//        fprintf(gnuplot_script, "     'data/quadratic_zero_start_chebyshev_n%d.dat' with lines lw 2 lc 'magenta' title 'Quadratic Spline', \\\n", n);
//        fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 7 ps 1.5 lc 'black' title 'Nodes'\n", n);

    }
    fclose(gnuplot_script);
    printf("Generated Gnuplot script: %s\n", script_path);
}