#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Needed for strcmp if more complex arg parsing were added
#include "../include/functions.h"
#include "../include/utilities.h"

int main(int argc, char *argv[]) { // Accept command line arguments
    // --- Argument Validation ---
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <data_directory> <plots_directory>\n", argv[0]);
        fprintf(stderr, "Example: %s data plots\n", argv[0]);
        return 1;
    }
    const char* dataDir = argv[1];
    const char* plotsDir = argv[2];

    printf("Using Data Directory: %s\n", dataDir);
    printf("Using Plots Directory: %s\n", plotsDir);
    // --------------------------

    const int N = 101;  // Number of points
    const float xmin = 0.99f;
    const float xmax = 1.01f;

    // --- Memory Allocation (no changes) ---
    float* values_x = (float*)malloc(N * sizeof(float));
    float* f1_float = (float*)malloc(N * sizeof(float));
    double* f1_double = (double*)malloc(N * sizeof(double));
    long double* f1_long_double = (long double*)malloc(N * sizeof(long double));
    float* f2_float = (float*)malloc(N * sizeof(float));
    double* f2_double = (double*)malloc(N * sizeof(double));
    long double* f2_long_double = (long double*)malloc(N * sizeof(long double));
    float* f3_float = (float*)malloc(N * sizeof(float));
    double* f3_double = (double*)malloc(N * sizeof(double));
    long double* f3_long_double = (long double*)malloc(N * sizeof(long double));
    float* f4_float = (float*)malloc(N * sizeof(float));
    double* f4_double = (double*)malloc(N * sizeof(double));
    long double* f4_long_double = (long double*)malloc(N * sizeof(long double));

    // --- Memory Allocation Check (no changes) ---
    if (!values_x || !f1_float || !f1_double || !f1_long_double ||
        !f2_float || !f2_double || !f2_long_double ||
        !f3_float || !f3_double || !f3_long_double ||
        !f4_float || !f4_double || !f4_long_double) {
        fprintf(stderr, "Memory allocation error.\n");
        // Consider freeing already allocated memory before exiting
        return 1;
    }

    // --- Generating x values (no changes) ---
    float step = (xmax - xmin) / (N - 1);
    for (int i = 0; i < N; i++) {
        values_x[i] = xmin + i * step;
    }

    // --- Calculating function values (no changes) ---
    for (int i = 0; i < N; i++) {
        float x_float = values_x[i];
        double x_double = (double)x_float;
        long double x_long_double = (long double)x_float;

        f1_float[i] = function1_float(x_float);
        f1_double[i] = function1_double(x_double);
        f1_long_double[i] = function1_long_double(x_long_double);

        f2_float[i] = function2_float(x_float);
        f2_double[i] = function2_double(x_double);
        f2_long_double[i] = function2_long_double(x_long_double);

        f3_float[i] = function3_float(x_float);
        f3_double[i] = function3_double(x_double);
        f3_long_double[i] = function3_long_double(x_long_double);

        f4_float[i] = function4_float(x_float);
        f4_double[i] = function4_double(x_double);
        f4_long_double[i] = function4_long_double(x_long_double);
    }

    // --- Use utilities to save results and generate scripts, passing directories ---
    const char* base_name_sep = "separate_results"; // Basename for separate files
    const char* base_name_orig = "results"; // Basename for original combined file

    printf("\n--- Generating Data Files and Plot Scripts ---\n");

    // Save data to separate CSV files (will also call original save_results)
    save_results_to_separate_files(dataDir, base_name_sep, base_name_orig, values_x,
                 f1_float, f1_double, f1_long_double,
                 f2_float, f2_double, f2_long_double,
                 f3_float, f3_double, f3_long_double,
                 f4_float, f4_double, f4_long_double,
                 N);

    // Generate error analysis CSV and script
    generate_error_analysis(dataDir, plotsDir, base_name_sep, values_x,
                 f1_float, f1_double, f1_long_double,
                 f2_float, f2_double, f2_long_double,
                 f3_float, f3_double, f3_long_double,
                 f4_float, f4_double, f4_long_double,
                 N);

    // Generate scripts comparing all types for each function
    for (int i = 1; i <= 4; i++) {
        generate_gnuplot_script_function_all_types(dataDir, plotsDir, base_name_sep, i);
    }

    // Generate multiplot collage script
    generate_multiplot_script(dataDir, plotsDir, base_name_sep, 4, 3);

    // Generate individual plots script for each func/type combination
    generate_individual_plots(dataDir, plotsDir, base_name_sep, 4);

    // Generate original format scripts (using the combined results.csv)
    for (int i = 1; i <= 4; i++) {
        generate_gnuplot_script(dataDir, plotsDir, base_name_orig, i);
    }
    generate_gnuplot_script_type(dataDir, plotsDir, base_name_orig, "float", 0);
    generate_gnuplot_script_type(dataDir, plotsDir, base_name_orig, "double", 1);
    generate_gnuplot_script_type(dataDir, plotsDir, base_name_orig, "long_double", 2);

    printf("\n--- Data and Script Generation Complete ---\n");

    // --- Comparison of results (no changes) ---
    printf("\nComparison of results between functions and variable types:\n");
    int idx_middle = N / 2; // Index for x potentially near 1.0
    if (N % 2 == 1 && fabsf(values_x[idx_middle] - 1.0f) < step / 2.0f) {
         printf("\nValues for x = %.10f (index %d):\n", (double)values_x[idx_middle], idx_middle);
    } else {
        idx_middle = -1; // Indicate 1.0 is not exactly hit or range is different
        printf("\nNote: x=1.0 might not be exactly calculated. Showing results near the middle.\n");
        idx_middle = N / 2;
        printf("\nValues for x = %.10f (index %d):\n", (double)values_x[idx_middle], idx_middle);
    }

    if(idx_middle >= 0) {
        printf("Function 1 (float): %.10e, (double): %.10e, (long double): %.10Le\n",
            (double)f1_float[idx_middle], f1_double[idx_middle], f1_long_double[idx_middle]);
        printf("Function 2 (float): %.10e, (double): %.10e, (long double): %.10Le\n",
            (double)f2_float[idx_middle], f2_double[idx_middle], f2_long_double[idx_middle]);
        printf("Function 3 (float): %.10e, (double): %.10e, (long double): %.10Le\n",
            (double)f3_float[idx_middle], f3_double[idx_middle], f3_long_double[idx_middle]);
        printf("Function 4 (float): %.10e, (double): %.10e, (long double): %.10Le\n",
            (double)f4_float[idx_middle], f4_double[idx_middle], f4_long_double[idx_middle]);
    }

    // --- Freeing up memory (no changes) ---
    free(values_x);
    free(f1_float); free(f1_double); free(f1_long_double);
    free(f2_float); free(f2_double); free(f2_long_double);
    free(f3_float); free(f3_double); free(f3_long_double);
    free(f4_float); free(f4_double); free(f4_long_double);

    // --- Remove system calls to gnuplot ---
    printf("\nTo generate plots, run: make plots\n");

    return 0;
}