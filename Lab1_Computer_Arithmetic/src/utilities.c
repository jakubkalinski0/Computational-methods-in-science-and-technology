#include "../include/utilities.h"
#include <stdio.h>  // For file operations (fopen, fprintf, fclose, snprintf, perror)
#include <stdlib.h> // For general utilities (e.g., exit - though not used directly here)
#include <string.h> // For string manipulation (snprintf)
#include <math.h>   // For fabsl, isnan (requires C99/C11 or later for isnan)

/**
 * @file utilities.c
 * @brief Implementation of utility functions for data saving and Gnuplot script generation.
 *        Handles file I/O, path construction, and formatting for CSV and Gnuplot files.
 */

/**
 * @brief Helper function to safely open a file.
 *        Checks if the file was opened successfully and prints an error message to stderr if not.
 * @param path The path to the file.
 * @param mode The mode to open the file in (e.g., "w" for write, "r" for read).
 * @param description A description of the file type (e.g., "CSV data") used in error messages.
 * @return A file pointer (FILE*) to the opened file, or NULL if an error occurred.
 */
FILE* open_file(const char* path, const char* mode, const char* description) {
    FILE* file = fopen(path, mode);
    if (!file) {
        // Print a descriptive error message to standard error
        fprintf(stderr, "Error: Could not open %s file '%s' for %s.\n",
                description, path, (*mode == 'w') ? "writing" : "reading");
        perror("fopen details"); // Print system-specific error details (e.g., "Permission denied")
    }
    return file;
}


/**
 * @brief Saves all computed results into a single, wide CSV file.
 *        This is the original saving format. The file is created in `dataDir`.
 * @param dataDir Directory path for saving the CSV file.
 * @param base_name Base name for the CSV file (e.g., "results" -> "results.csv").
 * @param values_x Array of x-coordinates.
 * @param f1_float...f4_long_double Arrays containing the computed function values.
 * @param n Number of data points.
 */
void save_results(const char* dataDir, const char* base_name, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n) {
    char file_path[FILE_PATH_BUFFER_SIZE];
    // Construct the full file path
    snprintf(file_path, sizeof(file_path), "%s/%s.csv", dataDir, base_name);

    // Open the file for writing
    FILE* file = open_file(file_path, "w", "combined results CSV");
    if (!file) return; // Exit if file could not be opened

    // Write the header row
    fprintf(file, "x,f1_float,f1_double,f1_long_double,f2_float,f2_double,f2_long_double,");
    fprintf(file, "f3_float,f3_double,f3_long_double,f4_float,f4_double,f4_long_double\n");

    // Write the data rows
    for (int i = 0; i < n; i++) {
        // Use %.15e for float/double and %.15Le for long double for consistent precision in output
        fprintf(file, "%.15e,%.15e,%.15e,%.15Le,%.15e,%.15e,%.15Le,%.15e,%.15e,%.15Le,%.15e,%.15e,%.15Le\n",
                (double)values_x[i], // Cast x to double for consistent format specifier
                (double)f1_float[i], f1_double[i], f1_long_double[i],
                (double)f2_float[i], f2_double[i], f2_long_double[i],
                (double)f3_float[i], f3_double[i], f3_long_double[i],
                (double)f4_float[i], f4_double[i], f4_long_double[i]);
    }

    // Close the file
    fclose(file);
    printf("Combined results saved to file %s.\n", file_path);
}

/**
 * @brief Saves results into separate CSV files (one per function) in `dataDir`.
 *        Each file has columns: x, float, double, long_double.
 *        Also calls `save_results` to create the combined file for backward compatibility
 *        or alternative plotting needs.
 * @param dataDir Directory path for saving the CSV files.
 * @param base_name_sep Base name for separate files (e.g., "sep_results" -> "sep_results_f1.csv").
 * @param base_name_orig Base name for the combined file (e.g., "results" -> "results.csv").
 * @param values_x Array of x-coordinates.
 * @param f1_float...f4_long_double Arrays containing the computed function values.
 * @param n Number of data points.
 */
void save_results_to_separate_files(const char* dataDir, const char* base_name_sep, const char* base_name_orig, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n) {

    char file_path[FILE_PATH_BUFFER_SIZE];
    FILE* current_file = NULL;
    // Arrays of pointers to the actual result arrays for easier iteration
    const float* f_float_arr[] = {f1_float, f2_float, f3_float, f4_float};
    const double* f_double_arr[] = {f1_double, f2_double, f3_double, f4_double};
    const long double* f_ldouble_arr[] = {f1_long_double, f2_long_double, f3_long_double, f4_long_double};

    // Loop through each function (1 to 4)
    for(int f=0; f<4; ++f) {
        // Construct file path for the current function's separate CSV
        snprintf(file_path, sizeof(file_path), "%s/%s_f%d.csv", dataDir, base_name_sep, f+1);
        // Open the file
        current_file = open_file(file_path, "w", "separate results CSV");
        if (current_file) {
            // Write header
            fprintf(current_file, "x,float,double,long_double\n");
            // Get pointers to the correct data arrays for this function (f+1)
            const float* current_f_float = f_float_arr[f];
            const double* current_f_double = f_double_arr[f];
            const long double* current_f_ldouble = f_ldouble_arr[f];
            // Write data rows
            for (int i = 0; i < n; i++) {
                fprintf(current_file, "%.15e,%.15e,%.15e,%.15Le\n",
                        (double)values_x[i],          // x value
                        (double)current_f_float[i],   // float result
                        current_f_double[i],          // double result
                        current_f_ldouble[i]);        // long double result
            }
            // Close the current file
            fclose(current_file);
            printf("Results for function %d saved to file %s.\n", f+1, file_path);
        }
    }

    // Keep the original combined format for compatibility or other uses
    // This function call only needs the dataDir for outputting its combined CSV.
    save_results(dataDir, base_name_orig, values_x,
                 f1_float, f1_double, f1_long_double,
                 f2_float, f2_double, f2_long_double,
                 f3_float, f3_double, f3_long_double,
                 f4_float, f4_double, f4_long_double,
                 n);
}


/**
 * @brief Generates a Gnuplot script (.gp) in `gpScriptsDir` to plot results
 *        for a single function, comparing all precision types.
 *        The script reads data from the corresponding separate CSV file in `dataDir`.
 *        The generated plot image (.png) path is set to `plotImagesDir` within the script.
 * @param dataDir Directory containing the input CSV data files (e.g., "separate_results_f1.csv").
 * @param gpScriptsDir Directory to save the generated Gnuplot script (.gp).
 * @param plotImagesDir Directory path for the output PNG image (used in 'set output').
 * @param base_name_csv Base name of the separate data files (e.g., "separate_results").
 * @param function The function number (1-4) for which to generate the plot script.
 */
void generate_gnuplot_script_function_all_types(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, int function) {
    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];
    char data_path[FILE_PATH_BUFFER_SIZE];

    // Path to the .gp script file being generated
    snprintf(script_path, sizeof(script_path), "%s/chart_f%d_all_types.gp", gpScriptsDir, function);
    // Path to the .png image file to be generated by gnuplot
    snprintf(output_path, sizeof(output_path), "%s/chart_f%d_all_types.png", plotImagesDir, function);
    // Path to the specific .csv data file to be read by gnuplot
    snprintf(data_path, sizeof(data_path), "%s/%s_f%d.csv", dataDir, base_name_csv, function);

    FILE* script = open_file(script_path, "w", "gnuplot script (f_all_types)");
    if (!script) return; // Exit if script file cannot be opened

    // Write Gnuplot commands to the script file
    fprintf(script, "# Gnuplot script generated by C program\n");
    fprintf(script, "# Plots data for Function %d from: %s\n\n", function, data_path);

    fprintf(script, "set terminal pngcairo size 1200,800 enhanced font 'Verdana,10'\n"); // Set output format and options
    fprintf(script, "set output '%s'\n", output_path); // Set the output PNG file path
    fprintf(script, "set title 'Function %d: Comparison of all variable types'\n", function); // Plot title
    fprintf(script, "set xlabel 'x'\n"); // X-axis label
    fprintf(script, "set ylabel 'f%d(x)'\n", function); // Y-axis label
    fprintf(script, "set grid\n"); // Enable grid lines
    fprintf(script, "set key outside bottom center\n"); // Position the legend
    fprintf(script, "set datafile separator \",\"\n"); // Specify CSV separator
    fprintf(script, "set logscale y\n"); // Use logarithmic scale for y-axis (often useful for these functions)

    // Plot command: plots data from the specified CSV file
    // 'using 1:2' means use column 1 (x) for x-axis, column 2 (float) for y-axis
    // 'using 1:3' uses column 3 (double)
    // 'using 1:4' uses column 4 (long double)
    fprintf(script, "plot '%s' using 1:2 title 'float' with points pt 7 ps 0.8, \\\n", data_path); // Plot float data
    fprintf(script, "     '%s' using 1:3 title 'double' with points pt 9 ps 0.8, \\\n", data_path); // Plot double data
    fprintf(script, "     '%s' using 1:4 title 'long double' with points pt 11 ps 0.8\n", data_path); // Plot long double data

    fclose(script);
    printf("Gnuplot script created: %s\n", script_path);
}

/**
 * @brief Generates a Gnuplot script (.gp) in `gpScriptsDir` for a multiplot collage.
 *        Creates a grid of plots (num_functions rows, num_types columns).
 *        Reads data from the separate CSV files in `dataDir`.
 *        The generated collage image (.png) path is set to `plotImagesDir` within the script.
 * @param dataDir Directory containing the input CSV data files.
 * @param gpScriptsDir Directory to save the generated Gnuplot script (.gp).
 * @param plotImagesDir Directory path for the output PNG image (used in 'set output').
 * @param base_name_csv Base name of the separate data files (e.g., "separate_results").
 * @param num_functions Number of functions (rows in the collage).
 * @param num_types Number of types (columns in the collage).
 */
void generate_multiplot_script(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, int num_functions, int num_types) {
    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];
    char data_path_template[FILE_PATH_BUFFER_SIZE]; // Template for data file names

    // Construct paths for the script and output image
    snprintf(script_path, sizeof(script_path), "%s/chart_collage.gp", gpScriptsDir);
    snprintf(output_path, sizeof(output_path), "%s/chart_collage.png", plotImagesDir);
    // Create a template for data file paths, using %d for the function number
    snprintf(data_path_template, sizeof(data_path_template), "%s/%s_f%%d.csv", dataDir, base_name_csv);

    FILE* script = open_file(script_path, "w", "gnuplot script (collage)");
    if (!script) return;

    fprintf(script, "# Gnuplot script generated by C program\n");
    fprintf(script, "# Creates a collage plot from data files matching pattern: %s\n\n", data_path_template);

    fprintf(script, "set terminal pngcairo size 1600,1200 enhanced font 'Verdana,10'\n"); // Larger terminal for collage
    fprintf(script, "set output '%s'\n", output_path); // Output file for the collage
    fprintf(script, "set datafile separator \",\"\n"); // CSV separator
    fprintf(script, "set grid\n"); // Enable grid for subplots

    int rows = num_functions;
    int cols = num_types;
    // Set up multiplot layout
    fprintf(script, "set multiplot layout %d,%d title 'Comparison of functions and variable types' font ',14'\n", rows, cols);

    // Array of type names for titles and column selection
    const char* type_names[] = {"float", "double", "long_double"};

    // Nested loops: outer for functions (rows), inner for types (columns)
    for (int f = 1; f <= num_functions; f++) { // Loop through functions (1 to 4)
        for (int t = 0; t < num_types; t++) { // Loop through types (0=float, 1=double, 2=long double)
            char current_data_path[FILE_PATH_BUFFER_SIZE];
            // Construct the specific data file path for the current function f
            snprintf(current_data_path, sizeof(current_data_path), data_path_template, f);

            // Set title and labels for the current subplot
            fprintf(script, "set title 'Function %d - %s'\n", f, type_names[t]);
            fprintf(script, "set xlabel 'x'\n");
            fprintf(script, "set ylabel 'f(x)'\n");
            // Plot command for the subplot
            // Column t+2 corresponds to the type: 2=float, 3=double, 4=long_double in separate CSVs
            fprintf(script, "plot '%s' using 1:%d notitle with points pt 7 ps 0.6\n",
                    current_data_path, t+2); // Use t+2 for column index
        }
    }

    // End multiplot mode
    fprintf(script, "unset multiplot\n");
    fclose(script);
    printf("Gnuplot script for collage created: %s\n", script_path);
}

/**
 * @brief Generates a Gnuplot script (.gp) in `gpScriptsDir` for a single function,
 *        comparing all types. This is the *original* version that reads data
 *        from the combined CSV file in `dataDir`.
 *        The generated plot image (.png) path is set to `plotImagesDir` within the script.
 * @param dataDir Directory containing the combined input CSV data file (e.g., "results.csv").
 * @param gpScriptsDir Directory to save the generated Gnuplot script (.gp).
 * @param plotImagesDir Directory path for the output PNG image (used in 'set output').
 * @param base_name_csv Base name of the combined data file (e.g., "results").
 * @param function The function number (1-4) for which to generate the plot script.
 */
void generate_gnuplot_script(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, int function) {
    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];
    char data_path[FILE_PATH_BUFFER_SIZE];

    // Construct paths
    snprintf(script_path, sizeof(script_path), "%s/chart_f%d.gp", gpScriptsDir, function); // Script path
    snprintf(output_path, sizeof(output_path), "%s/chart_f%d.png", plotImagesDir, function); // Output image path
    snprintf(data_path, sizeof(data_path), "%s/%s.csv", dataDir, base_name_csv); // Data path (combined CSV)

    FILE* script = open_file(script_path, "w", "gnuplot script (orig f)");
    if (!script) return;

    fprintf(script, "# Gnuplot script generated by C program\n");
    fprintf(script, "# Plots data for Function %d from combined file: %s\n\n", function, data_path);

    fprintf(script, "set terminal pngcairo size 1200,800 enhanced font 'Verdana,10'\n");
    fprintf(script, "set output '%s'\n", output_path); // Use plotImagesDir for output PNG
    fprintf(script, "set title 'Comparison of f%d function results for different variable types (Original Format)'\n", function);
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'f(x)'\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside bottom center\n");
    fprintf(script, "set datafile separator \",\"\n");
    fprintf(script, "set logscale y\n");

    // Calculate column indices in the combined CSV file based on the function number
    // Col 1: x
    // Cols 2,3,4: f1 (float, double, long double)
    // Cols 5,6,7: f2 (...)
    // Cols 8,9,10: f3 (...)
    // Cols 11,12,13: f4 (...)
    int col_float = 1 + (function - 1) * 3 + 1; // e.g., f1: 1+0+1=2, f2: 1+3+1=5
    int col_double = col_float + 1;
    int col_long_double = col_float + 2;

    // Plot command using calculated column indices
    fprintf(script, "plot '%s' using 1:%d title 'float' with points pt 7 ps 0.8, \\\n", data_path, col_float);
    fprintf(script, "     '%s' using 1:%d title 'double' with points pt 9 ps 0.8, \\\n", data_path, col_double);
    fprintf(script, "     '%s' using 1:%d title 'long double' with points pt 11 ps 0.8\n", data_path, col_long_double);

    fclose(script);
    printf("Gnuplot script created: %s\n", script_path);
}

/**
 * @brief Generates a Gnuplot script (.gp) in `gpScriptsDir` comparing all functions
 *        for a single specified precision type. This is the *original* version that
 *        reads data from the combined CSV file in `dataDir`.
 *        The generated plot image (.png) path is set to `plotImagesDir` within the script.
 * @param dataDir Directory containing the combined input CSV data file (e.g., "results.csv").
 * @param gpScriptsDir Directory to save the generated Gnuplot script (.gp).
 * @param plotImagesDir Directory path for the output PNG image (used in 'set output').
 * @param base_name_csv Base name of the combined data file (e.g., "results").
 * @param type Name of the type ("float", "double", "long_double") used for title and legend.
 * @param offset 0 for float (cols 2, 5, 8, 11), 1 for double (cols 3, 6, 9, 12), 2 for long double (cols 4, 7, 10, 13).
 */
void generate_gnuplot_script_type(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, const char* type, int offset) {
    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];
    char data_path[FILE_PATH_BUFFER_SIZE];

    // Construct paths
    snprintf(script_path, sizeof(script_path), "%s/chart_%s.gp", gpScriptsDir, type); // Script path uses type name
    snprintf(output_path, sizeof(output_path), "%s/chart_%s.png", plotImagesDir, type); // Output image path uses type name
    snprintf(data_path, sizeof(data_path), "%s/%s.csv", dataDir, base_name_csv); // Data path (combined CSV)

    FILE* script = open_file(script_path, "w", "gnuplot script (orig type)");
    if (!script) return;

    fprintf(script, "# Gnuplot script generated by C program\n");
    fprintf(script, "# Plots comparison of all functions for type %s from combined file: %s\n\n", type, data_path);

    fprintf(script, "set terminal pngcairo size 1200,800 enhanced font 'Verdana,10'\n");
    fprintf(script, "set output '%s'\n", output_path); // Use plotImagesDir for output PNG
    fprintf(script, "set title 'Comparison of all functions for type %s (Original Format)'\n", type);
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'f(x)'\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside bottom center\n");
    fprintf(script, "set datafile separator \",\"\n");
    fprintf(script, "set logscale y\n");

    // Calculate column indices based on the type offset
    // Base columns for float are 2, 5, 8, 11. Add offset for double/long double.
    int col_f1 = 2 + offset;
    int col_f2 = 5 + offset;
    int col_f3 = 8 + offset;
    int col_f4 = 11 + offset;

    // Plot command using calculated column indices for the specific type
    // Use backslash before underscore in titles for Gnuplot enhanced text interpretation
    fprintf(script, "plot '%s' using 1:%d title 'f1\\_%s' with points pt 7 ps 0.8, \\\n", data_path, col_f1, type);
    fprintf(script, "     '%s' using 1:%d title 'f2\\_%s' with points pt 9 ps 0.8, \\\n", data_path, col_f2, type);
    fprintf(script, "     '%s' using 1:%d title 'f3\\_%s' with points pt 11 ps 0.8, \\\n", data_path, col_f3, type);
    fprintf(script, "     '%s' using 1:%d title 'f4\\_%s' with points pt 5 ps 0.8\n", data_path, col_f4, type);

    fclose(script);
    printf("Gnuplot script created: %s\n", script_path);
}

/**
 * @brief Calculates relative errors for float and double results compared to long double,
 *        saves them to a CSV file in `dataDir`, and generates a Gnuplot script (.gp)
 *        in `gpScriptsDir` to plot these errors.
 *        The generated plot image (.png) path is set to `plotImagesDir` within the script.
 * @param dataDir Directory to save the error analysis CSV file.
 * @param gpScriptsDir Directory to save the generated Gnuplot script (.gp).
 * @param plotImagesDir Directory path for the output PNG image (used in 'set output').
 * @param base_name_csv Base name for the error CSV and script (e.g., "separate_results").
 * @param values_x Array of x-coordinates.
 * @param f1_float...f4_long_double Arrays containing the computed function values.
 * @param n Number of data points.
 */
void generate_error_analysis(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, float* values_x,
                             float* f1_float, double* f1_double, long double* f1_long_double,
                             float* f2_float, double* f2_double, long double* f2_long_double,
                             float* f3_float, double* f3_double, long double* f3_long_double,
                             float* f4_float, double* f4_double, long double* f4_long_double,
                             int n) {

    char data_path[FILE_PATH_BUFFER_SIZE];
    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];

    // Construct paths for the error data CSV, Gnuplot script, and output PNG
    snprintf(data_path, sizeof(data_path), "%s/%s_error_analysis.csv", dataDir, base_name_csv);
    snprintf(script_path, sizeof(script_path), "%s/chart_error_analysis.gp", gpScriptsDir);
    snprintf(output_path, sizeof(output_path), "%s/chart_error_analysis.png", plotImagesDir);

    // --- Calculate and Save Error Data ---
    FILE* file = open_file(data_path, "w", "error analysis CSV");
    if (!file) return; // Exit if cannot open error CSV file

    // Write header for the error CSV file
    fprintf(file, "x,f1_float_err,f1_double_err,f2_float_err,f2_double_err,");
    fprintf(file, "f3_float_err,f3_double_err,f4_float_err,f4_double_err\n");

    // Loop through data points to calculate relative errors
    for (int i = 0; i < n; i++) {
        // Use long double results as the reference values
        // Mark as volatile to potentially prevent unwanted optimizations if issues arise, though likely not strictly necessary here.
        volatile long double ref1 = f1_long_double[i];
        volatile long double ref2 = f2_long_double[i];
        volatile long double ref3 = f3_long_double[i];
        volatile long double ref4 = f4_long_double[i];

        // Calculate relative error: |(value - reference) / reference|
        // Handle division by zero: if reference is 0, error is 0 (assuming value is also 0) or undefined (here, treated as 0).
        double f1_float_error = (ref1 == 0.0L) ? 0.0 : fabsl(((long double)f1_float[i] - ref1) / ref1);
        double f1_double_error = (ref1 == 0.0L) ? 0.0 : fabsl(((long double)f1_double[i] - ref1) / ref1);
        double f2_float_error = (ref2 == 0.0L) ? 0.0 : fabsl(((long double)f2_float[i] - ref2) / ref2);
        double f2_double_error = (ref2 == 0.0L) ? 0.0 : fabsl(((long double)f2_double[i] - ref2) / ref2);
        double f3_float_error = (ref3 == 0.0L) ? 0.0 : fabsl(((long double)f3_float[i] - ref3) / ref3);
        double f3_double_error = (ref3 == 0.0L) ? 0.0 : fabsl(((long double)f3_double[i] - ref3) / ref3);

        // Handle NAN for function 4:
        // Initialize errors to NAN. Calculate only if ref4 is a non-zero number.
        // If ref4 is NAN and the corresponding value is also NAN, consider error 0.
        double f4_float_error = NAN, f4_double_error = NAN;

        // Check if reference value is a valid, non-zero number
        if (!isnan(ref4) && ref4 != 0.0L) {
             f4_float_error = fabsl(((long double)f4_float[i] - ref4) / ref4);
             f4_double_error = fabsl(((long double)f4_double[i] - ref4) / ref4);
        } else if (isnan(ref4) && isnan(f4_float[i])) { // Both reference and float value are NAN
             f4_float_error = 0.0; // Define error as 0 if both are NAN
        } // else: f4_float_error remains NAN if ref is NAN but float isn't, or vice versa.

        if (isnan(ref4) && isnan(f4_double[i])) { // Both reference and double value are NAN
             f4_double_error = 0.0; // Define error as 0 if both are NAN
        } // else: f4_double_error remains NAN

        // Write the calculated errors to the CSV file
        fprintf(file, "%.15e,%.15e,%.15e,%.15e,%.15e,%.15e,%.15e,%.15e,%.15e\n",
                (double)values_x[i],
                f1_float_error, f1_double_error,
                f2_float_error, f2_double_error,
                f3_float_error, f3_double_error,
                f4_float_error, f4_double_error); // Write NAN if applicable
    }
    fclose(file); // Close the error CSV file
    printf("Error analysis saved to file %s.\n", data_path);

    // --- Generate Gnuplot Script for Error Analysis ---
    FILE* script = open_file(script_path, "w", "gnuplot script (error analysis)");
    if (!script) return; // Exit if cannot open script file

    fprintf(script, "# Gnuplot script generated by C program\n");
    fprintf(script, "# Plots relative error analysis from data file: %s\n\n", data_path);

    fprintf(script, "set terminal pngcairo size 1600,1200 enhanced font 'Verdana,10'\n");
    fprintf(script, "set output '%s'\n", output_path); // Set output PNG path
    fprintf(script, "set title 'Relative Error Analysis (compared to long double)' font ',14'\n");
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'Relative Error'\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside bottom center\n"); // Legend position
    fprintf(script, "set datafile separator \",\"\n"); // CSV separator
    fprintf(script, "set logscale y\n"); // Use log scale for y-axis to see small errors
    fprintf(script, "set format y \"%%.1e\"\n"); // Format y-axis labels in scientific notation

    // Plot command: plot each error column against x (column 1) from the error CSV
    // Uses lines (lw 2) for better visibility of error trends.
    fprintf(script, "plot '%s' using 1:2 title 'f1\\_float err' with lines lw 2, \\\n", data_path); // Col 2: f1 float error
    fprintf(script, "     '%s' using 1:3 title 'f1\\_double err' with lines lw 2, \\\n", data_path); // Col 3: f1 double error
    fprintf(script, "     '%s' using 1:4 title 'f2\\_float err' with lines lw 2, \\\n", data_path); // Col 4: f2 float error
    fprintf(script, "     '%s' using 1:5 title 'f2\\_double err' with lines lw 2, \\\n", data_path); // Col 5: f2 double error
    fprintf(script, "     '%s' using 1:6 title 'f3\\_float err' with lines lw 2, \\\n", data_path); // Col 6: f3 float error
    fprintf(script, "     '%s' using 1:7 title 'f3\\_double err' with lines lw 2, \\\n", data_path); // Col 7: f3 double error
    fprintf(script, "     '%s' using 1:8 title 'f4\\_float err' with lines lw 2, \\\n", data_path); // Col 8: f4 float error (may plot NANs as gaps)
    fprintf(script, "     '%s' using 1:9 title 'f4\\_double err' with lines lw 2\n", data_path);   // Col 9: f4 double error (may plot NANs as gaps)

    fclose(script); // Close the Gnuplot script file
    printf("Gnuplot script for error analysis created: %s\n", script_path);
}

/**
 * @brief Generates individual Gnuplot scripts (.gp) in `gpScriptsDir`, one for each
 *        combination of function (1 to num_functions) and precision type.
 *        Each script reads data from the corresponding separate CSV file in `dataDir`
 *        and plots only the data for that specific function/type combination.
 *        The generated plot image (.png) path is set to `plotImagesDir` within the script.
 * @param dataDir Directory containing the separate input CSV data files.
 * @param gpScriptsDir Directory to save the generated Gnuplot scripts (.gp).
 * @param plotImagesDir Directory path for the output PNG images (used in 'set output').
 * @param base_name_csv Base name of the separate data files (e.g., "separate_results").
 * @param num_functions The total number of functions (e.g., 4).
 */
void generate_individual_plots(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, int num_functions) {
    const char* type_names[] = {"float", "double", "long_double"}; // Names for file naming and titles
    int num_types = 3; // Number of precision types

    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];
    char data_path[FILE_PATH_BUFFER_SIZE];

    // Loop through each function
    for (int f = 1; f <= num_functions; f++) {
        // Construct the path to the data file for the current function f
        snprintf(data_path, sizeof(data_path), "%s/%s_f%d.csv", dataDir, base_name_csv, f);

        // Loop through each precision type
        for (int t = 0; t < num_types; t++) {
            // Construct the path for the Gnuplot script file (e.g., "scripts/chart_f1_float.gp")
            snprintf(script_path, sizeof(script_path), "%s/chart_f%d_%s.gp", gpScriptsDir, f, type_names[t]);
            // Construct the path for the output PNG image file (e.g., "plots/chart_f1_float.png")
            snprintf(output_path, sizeof(output_path), "%s/chart_f%d_%s.png", plotImagesDir, f, type_names[t]);

            // Open the Gnuplot script file for writing
            FILE* script = open_file(script_path, "w", "gnuplot script (individual f/type)");
            if (!script) continue; // Skip if file cannot be opened

            // Write Gnuplot commands to the script file
            fprintf(script, "# Gnuplot script generated by C program\n");
            fprintf(script, "# Plots Function %d (%s) from data file: %s\n\n", f, type_names[t], data_path);

            fprintf(script, "set terminal pngcairo size 800,600 enhanced font 'Verdana,10'\n"); // Standard size for individual plots
            fprintf(script, "set output '%s'\n", output_path); // Set output PNG file
            fprintf(script, "set title 'Function %d with %s precision'\n", f, type_names[t]); // Set plot title
            fprintf(script, "set xlabel 'x'\n"); // X-axis label
            fprintf(script, "set ylabel 'f%d(x)'\n", f); // Y-axis label
            fprintf(script, "set grid\n"); // Enable grid
            fprintf(script, "set key top right\n"); // Legend position
            fprintf(script, "set datafile separator \",\"\n"); // CSV separator

            // Plot command: Plots column 1 (x) vs column t+2 (specific type data)
            // t+2 gives column index: 2 for float (t=0), 3 for double (t=1), 4 for long double (t=2)
            fprintf(script, "plot '%s' using 1:%d title 'f%d\\_%s' with linespoints lw 1.5 pt 7 ps 0.6\n",
                    data_path, t+2, f, type_names[t]); // Plot data with lines and points

            fclose(script); // Close the script file
            printf("Gnuplot script created: %s\n", script_path);
        }
    }
}