/**
 * @file fileio.c
 * @brief Implementation of file I/O operations for root finding analysis.
 */
#include "../include/fileio.h"
#include "../include/function.h" // For f(x), a, b, N_param, M_param
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For snprintf
#include <math.h>   // For isnan

/**
 * @brief Opens and prepares the main CSV file for storing all root-finding results.
 */
FILE* openResultCsvFile(const char* filename) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "data/%s", filename);

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        fprintf(stderr, "Error [openResultCsvFile]: Could not open file: %s\n", filepath);
        return NULL;
    }

    // Write CSV Header
    fprintf(file, "Method,x0,x1,PrecisionRho,Root,Iterations,FinalError,Status\n");
    // Note: x1 is NAN for Newton

    return file;
}

/**
 * @brief Writes a double value or "NAN" string to the file.
 */
void fprintfDoubleOrNAN(FILE* file, double value, const char* suffix) {
    if (isnan(value)) {
        fprintf(file, "NAN%s", suffix);
    } else {
        fprintf(file, "%.16e%s", value, suffix); // High precision scientific notation
    }
}

/**
 * @brief Appends a single result row for Newton's method to the opened CSV file.
 */
void appendNewtonResultToCsv(FILE* file, double x0, double precision, RootResult result) {
    if (file == NULL) return;
    fprintf(file, "Newton,%.16e,NAN,%.1e,", x0, precision); // x1 is NAN for Newton
    fprintfDoubleOrNAN(file, result.root, ",");
    fprintf(file, "%d,", result.iterations);
    fprintfDoubleOrNAN(file, result.final_error, ",");
    fprintf(file, "%d\n", result.status);
}

/**
 * @brief Appends a single result row for the Secant method to the opened CSV file.
 */
void appendSecantResultToCsv(FILE* file, double x0, double x1, double precision, RootResult result) {
    if (file == NULL) return;
    fprintf(file, "Secant,%.16e,%.16e,%.1e,", x0, x1, precision);
    fprintfDoubleOrNAN(file, result.root, ",");
    fprintf(file, "%d,", result.iterations);
    fprintfDoubleOrNAN(file, result.final_error, ",");
    fprintf(file, "%d\n", result.status);
}


/**
 * @brief Generates a Gnuplot script to plot the function f(x) over the interval [a, b].
 */
void generateFunctionPlotScript(const char* script_filename, const char* plot_filename, int num_points) {
    char script_path[256];
    char plot_path[256];
    char data_path[256];

    snprintf(script_path, sizeof(script_path), "scripts/%s", script_filename);
    snprintf(plot_path, sizeof(plot_path), "plots/%s", plot_filename);
    snprintf(data_path, sizeof(data_path), "data/function_data.dat");

    // --- Generate Data for the Plot ---
    FILE *data_file = fopen(data_path, "w");
    if (data_file == NULL) {
        fprintf(stderr, "Error [generateFunctionPlotScript]: Cannot open data file %s for writing.\n", data_path);
        return;
    }
    double step = (b - a) / (double)(num_points - 1);
    for (int i = 0; i < num_points; ++i) {
        double x = a + i * step;
        if (i == num_points - 1) x = b; // Ensure endpoint
        fprintf(data_file, "%.10f %.10f\n", x, f(x));
    }
    fclose(data_file);
    printf("Generated function data for plotting: %s\n", data_path);

    // --- Generate Gnuplot Script ---
    FILE *gp_script = fopen(script_path, "w");
    if (gp_script == NULL) {
        fprintf(stderr, "Error [generateFunctionPlotScript]: Cannot open script file %s for writing.\n", script_path);
        return;
    }

    fprintf(gp_script, "# Gnuplot script: Plot function f(x)\n");
    fprintf(gp_script, "set terminal pngcairo enhanced size 800,600 font 'Arial,10'\n");
    fprintf(gp_script, "set output '%s'\n", plot_path);
    fprintf(gp_script, "set title 'Function f(x) = x^{%.0f} + x^{%.0f}'\n", N_param, M_param);
    fprintf(gp_script, "set xlabel 'x'\n");
    fprintf(gp_script, "set ylabel 'f(x)'\n");
    fprintf(gp_script, "set grid\n");
    fprintf(gp_script, "set zeroaxis lw 2\n");
    fprintf(gp_script, "set xrange [%.4f:%.4f]\n", a, b);
    // fprintf(gp_script, "set yrange [-1:2]\n"); // Adjust yrange based on function behavior in interval
    fprintf(gp_script, "plot '%s' using 1:2 with lines lw 2 title 'f(x)'\n", data_path);

    fclose(gp_script);
    printf("Generated Gnuplot script for function plot: %s\n", script_path);
}

// Removed generateIterationPlotScripts function as it's redundant.
// Python script (plot_results.py) is used for actual heatmap generation.