#include <stdio.h>     // For standard I/O (printf, fprintf)
#include <stdlib.h>    // For memory allocation (malloc, free) and exit codes
#include <string.h>    // Potentially useful for string operations (not directly used here)
#include <math.h>      // For fabsf (absolute value of float) used in index check
#include "../include/functions.h" // Include function declarations
#include "../include/utilities.h" // Include utility function declarations (saving, plotting)

/**
 * @file main.c
 * @brief Main program to calculate and compare results of different mathematical functions
 *        using float, double, and long double precision.
 *        It generates data points, computes function values, saves results to CSV files,
 *        and generates Gnuplot scripts for visualization and error analysis.
 *
 * Usage: ./computer_arithmetic <data_directory> <scripts_directory> <plots_directory>
 * Example: ./bin/computer_arithmetic data scripts plots
 */

int main(int argc, char *argv[]) {
    // --- Argument Validation for THREE directories ---
    // Check if the correct number of command-line arguments is provided.
    // Expects: program name, data directory, scripts directory, plots directory (total 4).
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <data_directory> <scripts_directory> <plots_directory>\n", argv[0]);
        fprintf(stderr, "Example: %s data scripts plots\n", argv[0]);
        return 1; // Return error code 1 for incorrect usage
    }
    // Store the directory paths provided as arguments
    const char* dataDir = argv[1];       // Directory for output data files (.csv)
    const char* gpScriptsDir = argv[2];  // Directory for generated Gnuplot scripts (.gp)
    const char* plotImagesDir = argv[3]; // Directory for generated plot images (.png)

    // Print the directories being used for confirmation
    printf("Using Data Directory: %s\n", dataDir);
    printf("Using GP Scripts Directory: %s\n", gpScriptsDir);
    printf("Using Plot Images Directory: %s\n", plotImagesDir);
    // -------------------------------------------------

    // --- Constants for Calculation ---
    const int N = 101;         // Number of data points to generate
    const float xmin = 0.99f;  // Minimum x value for the range
    const float xmax = 1.01f;  // Maximum x value for the range (Chosen around x=1 where functions might be interesting)

    // --- Memory Allocation ---
    // Allocate memory on the heap for storing x values and the results of each function/type combination.
    float* values_x = (float*)malloc(N * sizeof(float)); // x-coordinates

    // Function 1 results
    float* f1_float = (float*)malloc(N * sizeof(float));
    double* f1_double = (double*)malloc(N * sizeof(double));
    long double* f1_long_double = (long double*)malloc(N * sizeof(long double));

    // Function 2 results
    float* f2_float = (float*)malloc(N * sizeof(float));
    double* f2_double = (double*)malloc(N * sizeof(double));
    long double* f2_long_double = (long double*)malloc(N * sizeof(long double));

    // Function 3 results
    float* f3_float = (float*)malloc(N * sizeof(float));
    double* f3_double = (double*)malloc(N * sizeof(double));
    long double* f3_long_double = (long double*)malloc(N * sizeof(long double));

    // Function 4 results
    float* f4_float = (float*)malloc(N * sizeof(float));
    double* f4_double = (double*)malloc(N * sizeof(double));
    long double* f4_long_double = (long double*)malloc(N * sizeof(long double));

    // --- Memory Allocation Check ---
    // Verify that all memory allocations were successful. If any `malloc` returned NULL, print an error and exit.
    if (!values_x || !f1_float || !f1_double || !f1_long_double ||
        !f2_float || !f2_double || !f2_long_double ||
        !f3_float || !f3_double || !f3_long_double ||
        !f4_float || !f4_double || !f4_long_double) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        // In a real application, free any successfully allocated memory before exiting.
        // For simplicity here, we just exit.
        return 1; // Return error code 1 for allocation failure
    }

    // --- Generating x values ---
    // Calculate the step size between consecutive x values.
    float step = (xmax - xmin) / (N - 1);
    // Generate N evenly spaced x values from xmin to xmax.
    for (int i = 0; i < N; i++) {
        values_x[i] = xmin + i * step;
    }

    // --- Calculating function values ---
    // Loop through each generated x value.
    for (int i = 0; i < N; i++) {
        // Get the current x value and cast it to the required precisions.
        float x_float = values_x[i];
        double x_double = (double)x_float; // Cast float to double
        long double x_long_double = (long double)x_float; // Cast float to long double

        // Calculate results for Function 1 using all precisions
        f1_float[i] = function1_float(x_float);
        f1_double[i] = function1_double(x_double);
        f1_long_double[i] = function1_long_double(x_long_double);

        // Calculate results for Function 2 using all precisions
        f2_float[i] = function2_float(x_float);
        f2_double[i] = function2_double(x_double);
        f2_long_double[i] = function2_long_double(x_long_double);

        // Calculate results for Function 3 using all precisions
        f3_float[i] = function3_float(x_float);
        f3_double[i] = function3_double(x_double);
        f3_long_double[i] = function3_long_double(x_long_double);

        // Calculate results for Function 4 using all precisions
        f4_float[i] = function4_float(x_float);
        f4_double[i] = function4_double(x_double);
        f4_long_double[i] = function4_long_double(x_long_double);
    }

    // --- Use utility functions to save data and generate scripts ---
    // Define base filenames for the output files.
    const char* base_name_sep = "separate_results"; // Base name for separate CSVs (e.g., separate_results_f1.csv)
    const char* base_name_orig = "results";         // Base name for the combined CSV (e.g., results.csv)

    printf("\n--- Generating Data Files and Plot Scripts ---\n");

    // Save computed data to CSV files. This creates both separate and combined files in `dataDir`.
    save_results_to_separate_files(dataDir, base_name_sep, base_name_orig, values_x,
                                   f1_float, f1_double, f1_long_double,
                                   f2_float, f2_double, f2_long_double,
                                   f3_float, f3_double, f3_long_double,
                                   f4_float, f4_double, f4_long_double,
                                   N);

    // Generate error analysis data (CSV in `dataDir`) and the corresponding plot script (`.gp` in `gpScriptsDir`).
    // The script will be configured to save the plot image to `plotImagesDir`.
    generate_error_analysis(dataDir, gpScriptsDir, plotImagesDir, base_name_sep, values_x,
                            f1_float, f1_double, f1_long_double,
                            f2_float, f2_double, f2_long_double,
                            f3_float, f3_double, f3_long_double,
                            f4_float, f4_double, f4_long_double,
                            N);

    // Generate Gnuplot scripts comparing all types for each function (using separate CSVs).
    // Scripts saved in `gpScriptsDir`, read data from `dataDir`, output plots to `plotImagesDir`.
    for (int i = 1; i <= 4; i++) {
        generate_gnuplot_script_function_all_types(dataDir, gpScriptsDir, plotImagesDir, base_name_sep, i);
    }

    // Generate the multiplot collage script (using separate CSVs).
    // Script saved in `gpScriptsDir`, reads data from `dataDir`, outputs plot to `plotImagesDir`.
    generate_multiplot_script(dataDir, gpScriptsDir, plotImagesDir, base_name_sep, 4, 3); // 4 functions, 3 types

    // Generate individual plot scripts for each function/type combination (using separate CSVs).
    // Scripts saved in `gpScriptsDir`, read data from `dataDir`, output plots to `plotImagesDir`.
    generate_individual_plots(dataDir, gpScriptsDir, plotImagesDir, base_name_sep, 4); // 4 functions

    // Generate original format scripts (using the combined CSV).
    // These are kept for reference or if the original data format is needed.
    // Scripts saved in `gpScriptsDir`, read data from `dataDir`, output plots to `plotImagesDir`.
    // Generate scripts comparing types for each function (original combined CSV).
    for (int i = 1; i <= 4; i++) {
        generate_gnuplot_script(dataDir, gpScriptsDir, plotImagesDir, base_name_orig, i);
    }
    // Generate scripts comparing functions for each type (original combined CSV).
    // Offsets correspond to column shifts in the combined CSV.
    generate_gnuplot_script_type(dataDir, gpScriptsDir, plotImagesDir, base_name_orig, "float", 0);
    generate_gnuplot_script_type(dataDir, gpScriptsDir, plotImagesDir, base_name_orig, "double", 1);
    generate_gnuplot_script_type(dataDir, gpScriptsDir, plotImagesDir, base_name_orig, "long_double", 2);

    printf("\n--- Data and Script Generation Complete ---\n");

    // --- Comparison of results (optional check) ---
    // Print the calculated values for the point closest to x=1.0 for comparison.
    printf("\nComparison of results between functions and variable types:\n");
    int idx_middle = N / 2; // Index of the middle element
    // Check if the middle element is indeed very close to 1.0 (within half a step)
    if (N % 2 == 1 && fabsf(values_x[idx_middle] - 1.0f) < step / 2.0f) {
         printf("\nValues for x = %.10f (index %d):\n", (double)values_x[idx_middle], idx_middle);
    } else {
        // If N is even or the middle point isn't exactly 1.0, just show the middle value anyway.
        idx_middle = N / 2; // Use the middle index regardless
        printf("\nNote: x=1.0 might not be exactly one of the calculated points.\n");
        printf("Showing results for the middle point x = %.10f (index %d):\n", (double)values_x[idx_middle], idx_middle);
    }

    // Print the results for the selected index (middle point)
    if(idx_middle >= 0 && idx_middle < N) { // Basic bounds check
        printf("Function 1 (float): %18.10e, (double): %18.10e, (long double): %22.10Le\n",
            (double)f1_float[idx_middle], f1_double[idx_middle], f1_long_double[idx_middle]);
        printf("Function 2 (float): %18.10e, (double): %18.10e, (long double): %22.10Le\n",
            (double)f2_float[idx_middle], f2_double[idx_middle], f2_long_double[idx_middle]);
        printf("Function 3 (float): %18.10e, (double): %18.10e, (long double): %22.10Le\n",
            (double)f3_float[idx_middle], f3_double[idx_middle], f3_long_double[idx_middle]);
        // Note: Function 4 might be NAN if x is exactly 1.0
        printf("Function 4 (float): %18.10e, (double): %18.10e, (long double): %22.10Le\n",
            (double)f4_float[idx_middle], f4_double[idx_middle], f4_long_double[idx_middle]);
    }

    // --- Freeing up memory ---
    // Release all dynamically allocated memory to prevent memory leaks.
    free(values_x);
    free(f1_float); free(f1_double); free(f1_long_double);
    free(f2_float); free(f2_double); free(f2_long_double);
    free(f3_float); free(f3_double); free(f3_long_double);
    free(f4_float); free(f4_double); free(f4_long_double);

    // Inform the user how to generate the plots using the Makefile
    printf("\nTo generate plots from the created scripts, run: make plots\n");

    return 0; // Indicate successful execution
}