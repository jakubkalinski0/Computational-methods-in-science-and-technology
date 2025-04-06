#include "../include/utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For snprintf
#include <math.h> // For fabsl etc.

#define FILE_PATH_BUFFER_SIZE 256 // Define buffer size for file paths

// Helper function to handle file opening with error check
FILE* open_file(const char* path, const char* mode, const char* description) {
    FILE* file = fopen(path, mode);
    if (!file) {
        fprintf(stderr, "Error: Could not open %s file '%s' for %s.\n", description, path, (*mode == 'w') ? "writing" : "reading");
        perror("fopen"); // Print system error message
    }
    return file;
}


// --- Modified: save_results (Original combined file) ---
// Now takes dataDir argument
void save_results(const char* dataDir, const char* base_name, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n) {
    char file_path[FILE_PATH_BUFFER_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/%s.csv", dataDir, base_name);

    FILE* file = open_file(file_path, "w", "combined results CSV");
    if (!file) return;

    // Column Headings
    fprintf(file, "x,f1_float,f1_double,f1_long_double,f2_float,f2_double,f2_long_double,");
    fprintf(file, "f3_float,f3_double,f3_long_double,f4_float,f4_double,f4_long_double\n");

    // Data
    for (int i = 0; i < n; i++) {
        fprintf(file, "%.15e,%.15e,%.15e,%.15Le,%.15e,%.15e,%.15Le,%.15e,%.15e,%.15Le,%.15e,%.15e,%.15Le\n",
                (double)values_x[i], // Use higher precision format specifier
                (double)f1_float[i], f1_double[i], f1_long_double[i],
                (double)f2_float[i], f2_double[i], f2_long_double[i],
                (double)f3_float[i], f3_double[i], f3_long_double[i],
                (double)f4_float[i], f4_double[i], f4_long_double[i]);
    }

    fclose(file);
    printf("Combined results saved to file %s.\n", file_path);
}

// --- Modified: save_results_to_separate_files ---
// Takes dataDir, passes it to save_results
void save_results_to_separate_files(const char* dataDir, const char* base_name_sep, const char* base_name_orig, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n) {

    char file_path[FILE_PATH_BUFFER_SIZE];
    FILE* current_file = NULL;

    // File for function 1
    snprintf(file_path, sizeof(file_path), "%s/%s_f1.csv", dataDir, base_name_sep);
    current_file = open_file(file_path, "w", "separate results CSV (f1)");
    if (current_file) {
        fprintf(current_file, "x,float,double,long_double\n");
        for (int i = 0; i < n; i++) {
            fprintf(current_file, "%.15e,%.15e,%.15e,%.15Le\n", (double)values_x[i], (double)f1_float[i], f1_double[i], f1_long_double[i]);
        }
        fclose(current_file);
        printf("Results for function 1 saved to file %s.\n", file_path);
    }

    // File for function 2
    snprintf(file_path, sizeof(file_path), "%s/%s_f2.csv", dataDir, base_name_sep);
    current_file = open_file(file_path, "w", "separate results CSV (f2)");
     if (current_file) {
        fprintf(current_file, "x,float,double,long_double\n");
        for (int i = 0; i < n; i++) {
            fprintf(current_file, "%.15e,%.15e,%.15e,%.15Le\n", (double)values_x[i], (double)f2_float[i], f2_double[i], f2_long_double[i]);
        }
        fclose(current_file);
        printf("Results for function 2 saved to file %s.\n", file_path);
    }

     // File for function 3
    snprintf(file_path, sizeof(file_path), "%s/%s_f3.csv", dataDir, base_name_sep);
    current_file = open_file(file_path, "w", "separate results CSV (f3)");
     if (current_file) {
        fprintf(current_file, "x,float,double,long_double\n");
        for (int i = 0; i < n; i++) {
            fprintf(current_file, "%.15e,%.15e,%.15e,%.15Le\n", (double)values_x[i], (double)f3_float[i], f3_double[i], f3_long_double[i]);
        }
        fclose(current_file);
        printf("Results for function 3 saved to file %s.\n", file_path);
    }

    // File for function 4
    snprintf(file_path, sizeof(file_path), "%s/%s_f4.csv", dataDir, base_name_sep);
    current_file = open_file(file_path, "w", "separate results CSV (f4)");
     if (current_file) {
        fprintf(current_file, "x,float,double,long_double\n");
        for (int i = 0; i < n; i++) {
            fprintf(current_file, "%.15e,%.15e,%.15e,%.15Le\n", (double)values_x[i], (double)f4_float[i], f4_double[i], f4_long_double[i]);
        }
        fclose(current_file);
        printf("Results for function 4 saved to file %s.\n", file_path);
    }

    // Keep the original combined format for compatibility
    save_results(dataDir, base_name_orig, values_x,
                 f1_float, f1_double, f1_long_double,
                 f2_float, f2_double, f2_long_double,
                 f3_float, f3_double, f3_long_double,
                 f4_float, f4_double, f4_long_double,
                 n);
}


// --- Modified: generate_gnuplot_script_function_all_types ---
// Takes dataDir, plotsDir. Generates script in plotsDir, reads data from dataDir.
void generate_gnuplot_script_function_all_types(const char* dataDir, const char* plotsDir, const char* base_name_csv, int function) {
    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];
    char data_path[FILE_PATH_BUFFER_SIZE];

    snprintf(script_path, sizeof(script_path), "%s/chart_f%d_all_types.gp", plotsDir, function);
    snprintf(output_path, sizeof(output_path), "%s/chart_f%d_all_types.png", plotsDir, function);
    snprintf(data_path, sizeof(data_path), "%s/%s_f%d.csv", dataDir, base_name_csv, function);

    FILE* script = open_file(script_path, "w", "gnuplot script (f_all_types)");
    if (!script) return;

    fprintf(script, "# Gnuplot script generated by C program\n");
    fprintf(script, "# Data file: %s\n\n", data_path);

    fprintf(script, "set terminal pngcairo size 1200,800 enhanced font 'Verdana,10'\n"); // Better terminal
    fprintf(script, "set output '%s'\n", output_path); // Output path uses plotsDir
    fprintf(script, "set title 'Function %d: Comparison of all variable types'\n", function);
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'f%d(x)'\n", function);
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside bottom center\n"); // Better key placement
    fprintf(script, "set datafile separator \",\"\n");
    fprintf(script, "set logscale y\n"); // Keep logscale if desired

    fprintf(script, "plot '%s' using 1:2 title 'float' with points pt 7 ps 0.8, \\\n", data_path); // Read from dataDir
    fprintf(script, "     '%s' using 1:3 title 'double' with points pt 9 ps 0.8, \\\n", data_path);
    fprintf(script, "     '%s' using 1:4 title 'long double' with points pt 11 ps 0.8\n", data_path);

    fclose(script);
    printf("Gnuplot script created: %s\n", script_path);
}

// --- Modified: generate_multiplot_script ---
// Takes dataDir, plotsDir. Generates script in plotsDir, reads data from dataDir.
void generate_multiplot_script(const char* dataDir, const char* plotsDir, const char* base_name_csv, int num_functions, int num_types) {
    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];
    char data_path_template[FILE_PATH_BUFFER_SIZE]; // Template for data files

    snprintf(script_path, sizeof(script_path), "%s/chart_collage.gp", plotsDir);
    snprintf(output_path, sizeof(output_path), "%s/chart_collage.png", plotsDir);
    // Template used inside the loop
    snprintf(data_path_template, sizeof(data_path_template), "%s/%s_f%%d.csv", dataDir, base_name_csv);

    FILE* script = open_file(script_path, "w", "gnuplot script (collage)");
    if (!script) return;

    fprintf(script, "# Gnuplot script generated by C program\n");
    fprintf(script, "# Data files pattern: %s\n\n", data_path_template);

    fprintf(script, "set terminal pngcairo size 1600,1200 enhanced font 'Verdana,10'\n");
    fprintf(script, "set output '%s'\n", output_path);
    fprintf(script, "set datafile separator \",\"\n");
    fprintf(script, "set grid\n");
    // fprintf(script, "set logscale y\n"); // Often problematic in multiplot, enable if needed

    int rows = num_functions;
    int cols = num_types;
    fprintf(script, "set multiplot layout %d,%d title 'Comparison of functions and variable types' font ',14'\n", rows, cols);

    const char* type_names[] = {"float", "double", "long_double"};

    // Generate each plot in the collage
    for (int f = 1; f <= num_functions; f++) {
        for (int t = 0; t < num_types; t++) {
            char current_data_path[FILE_PATH_BUFFER_SIZE];
            snprintf(current_data_path, sizeof(current_data_path), data_path_template, f); // Create specific data path

            fprintf(script, "set title 'Function %d - %s'\n", f, type_names[t]);
            fprintf(script, "set xlabel 'x'\n");
            fprintf(script, "set ylabel 'f(x)'\n");
            // Plot using column t+2 (col 1=x, col 2=float, 3=double, 4=long double)
            fprintf(script, "plot '%s' using 1:%d notitle with points pt 7 ps 0.6\n",
                    current_data_path, t+2); // Read from dataDir
        }
    }

    fprintf(script, "unset multiplot\n");
    fclose(script);
    printf("Gnuplot script for collage created: %s\n", script_path);
}

// --- Modified: generate_gnuplot_script (Original single function) ---
// Takes dataDir, plotsDir. Generates script in plotsDir, reads combined data from dataDir.
void generate_gnuplot_script(const char* dataDir, const char* plotsDir, const char* base_name_csv, int function) {
    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];
    char data_path[FILE_PATH_BUFFER_SIZE];

    snprintf(script_path, sizeof(script_path), "%s/chart_f%d.gp", plotsDir, function);
    snprintf(output_path, sizeof(output_path), "%s/chart_f%d.png", plotsDir, function);
    snprintf(data_path, sizeof(data_path), "%s/%s.csv", dataDir, base_name_csv); // Uses combined results.csv

    FILE* script = open_file(script_path, "w", "gnuplot script (orig f)");
    if (!script) return;

    fprintf(script, "# Gnuplot script generated by C program\n");
    fprintf(script, "# Data file: %s\n\n", data_path);

    fprintf(script, "set terminal pngcairo size 1200,800 enhanced font 'Verdana,10'\n");
    fprintf(script, "set output '%s'\n", output_path);
    fprintf(script, "set title 'Comparison of f%d function results for different variable types (Original Format)'\n", function);
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'f(x)'\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside bottom center\n");
    fprintf(script, "set datafile separator \",\"\n");
    fprintf(script, "set logscale y\n"); // Keep logscale if desired

    // Columns for f1: 2,3,4; f2: 5,6,7; f3: 8,9,10; f4: 11,12,13
    int col_float = function * 3 - 1;
    int col_double = function * 3;
    int col_long_double = function * 3 + 1;

    fprintf(script, "plot '%s' using 1:%d title 'float' with points pt 7 ps 0.8, \\\n", data_path, col_float); // Read from dataDir
    fprintf(script, "     '%s' using 1:%d title 'double' with points pt 9 ps 0.8, \\\n", data_path, col_double);
    fprintf(script, "     '%s' using 1:%d title 'long double' with points pt 11 ps 0.8\n", data_path, col_long_double);

    fclose(script);
    printf("Gnuplot script created: %s\n", script_path);
}

// --- Modified: generate_gnuplot_script_type (Original single type) ---
// Takes dataDir, plotsDir. Generates script in plotsDir, reads combined data from dataDir.
void generate_gnuplot_script_type(const char* dataDir, const char* plotsDir, const char* base_name_csv, const char* type, int offset) {
    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];
    char data_path[FILE_PATH_BUFFER_SIZE];

    snprintf(script_path, sizeof(script_path), "%s/chart_%s.gp", plotsDir, type);
    snprintf(output_path, sizeof(output_path), "%s/chart_%s.png", plotsDir, type);
    snprintf(data_path, sizeof(data_path), "%s/%s.csv", dataDir, base_name_csv); // Uses combined results.csv

    FILE* script = open_file(script_path, "w", "gnuplot script (orig type)");
    if (!script) return;

    fprintf(script, "# Gnuplot script generated by C program\n");
    fprintf(script, "# Data file: %s\n\n", data_path);

    fprintf(script, "set terminal pngcairo size 1200,800 enhanced font 'Verdana,10'\n");
    fprintf(script, "set output '%s'\n", output_path);
    fprintf(script, "set title 'Comparison of all functions for type %s (Original Format)'\n", type);
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'f(x)'\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside bottom center\n");
    fprintf(script, "set datafile separator \",\"\n");
    fprintf(script, "set logscale y\n"); // Keep logscale if desired

    // Columns for float: 2, 5, 8, 11 (offset=0)
    // Columns for double: 3, 6, 9, 12 (offset=1)
    // Columns for long double: 4, 7, 10, 13 (offset=2)
    int col_f1 = 2 + offset;
    int col_f2 = 5 + offset;
    int col_f3 = 8 + offset;
    int col_f4 = 11 + offset;

    fprintf(script, "plot '%s' using 1:%d title 'f1_%s' with points pt 7 ps 0.8, \\\n", data_path, col_f1, type); // Read from dataDir
    fprintf(script, "     '%s' using 1:%d title 'f2_%s' with points pt 9 ps 0.8, \\\n", data_path, col_f2, type);
    fprintf(script, "     '%s' using 1:%d title 'f3_%s' with points pt 11 ps 0.8, \\\n", data_path, col_f3, type);
    fprintf(script, "     '%s' using 1:%d title 'f4_%s' with points pt 5 ps 0.8\n", data_path, col_f4, type);

    fclose(script);
    printf("Gnuplot script created: %s\n", script_path);
}

// --- Modified: generate_error_analysis ---
// Takes dataDir, plotsDir. Generates CSV in dataDir, script in plotsDir.
void generate_error_analysis(const char* dataDir, const char* plotsDir, const char* base_name_csv, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n) {

    char data_path[FILE_PATH_BUFFER_SIZE];
    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];

    snprintf(data_path, sizeof(data_path), "%s/%s_error_analysis.csv", dataDir, base_name_csv);
    snprintf(script_path, sizeof(script_path), "%s/chart_error_analysis.gp", plotsDir);
    snprintf(output_path, sizeof(output_path), "%s/chart_error_analysis.png", plotsDir);

    FILE* file = open_file(data_path, "w", "error analysis CSV");
    if (!file) return;

    // Column headings
    fprintf(file, "x,f1_float_err,f1_double_err,f2_float_err,f2_double_err,");
    fprintf(file, "f3_float_err,f3_double_err,f4_float_err,f4_double_err\n");

    // Calculate relative errors using long double as reference
    for (int i = 0; i < n; i++) {
        // Use volatile to potentially prevent excessive optimization issues if needed, though unlikely here
        volatile long double ref1 = f1_long_double[i];
        volatile long double ref2 = f2_long_double[i];
        volatile long double ref3 = f3_long_double[i];
        volatile long double ref4 = f4_long_double[i];

        // Calculate relative error, handle division by zero or near-zero reference
        double f1_float_error = (ref1 == 0.0L) ? 0.0 : fabsl(((long double)f1_float[i] - ref1) / ref1);
        double f1_double_error = (ref1 == 0.0L) ? 0.0 : fabsl(((long double)f1_double[i] - ref1) / ref1);

        double f2_float_error = (ref2 == 0.0L) ? 0.0 : fabsl(((long double)f2_float[i] - ref2) / ref2);
        double f2_double_error = (ref2 == 0.0L) ? 0.0 : fabsl(((long double)f2_double[i] - ref2) / ref2);

        double f3_float_error = (ref3 == 0.0L) ? 0.0 : fabsl(((long double)f3_float[i] - ref3) / ref3);
        double f3_double_error = (ref3 == 0.0L) ? 0.0 : fabsl(((long double)f3_double[i] - ref3) / ref3);

        // Function 4 might produce NAN for x=1, error is undefined there
        double f4_float_error = NAN, f4_double_error = NAN;
        if (!isnan(ref4) && ref4 != 0.0L) {
             f4_float_error = fabsl(((long double)f4_float[i] - ref4) / ref4);
             f4_double_error = fabsl(((long double)f4_double[i] - ref4) / ref4);
        } else if (isnan((double)f4_float[i]) && isnan(ref4)) {
             f4_float_error = 0.0; // NAN vs NAN could be considered 0 error? Or leave NAN
        }
         if (!isnan(ref4) && ref4 != 0.0L) {
             f4_double_error = fabsl(((long double)f4_double[i] - ref4) / ref4);
        } else if (isnan(f4_double[i]) && isnan(ref4)) {
             f4_double_error = 0.0;
        }


        fprintf(file, "%.15e,%.15e,%.15e,%.15e,%.15e,%.15e,%.15e,%.15e,%.15e\n",
                (double)values_x[i],
                f1_float_error, f1_double_error,
                f2_float_error, f2_double_error,
                f3_float_error, f3_double_error,
                f4_float_error, f4_double_error);
    }

    fclose(file);
    printf("Error analysis saved to file %s.\n", data_path);

    // Create gnuplot script for error analysis
    FILE* script = open_file(script_path, "w", "gnuplot script (error analysis)");
    if (!script) return;

    fprintf(script, "# Gnuplot script generated by C program\n");
    fprintf(script, "# Data file: %s\n\n", data_path);

    fprintf(script, "set terminal pngcairo size 1600,1200 enhanced font 'Verdana,10'\n");
    fprintf(script, "set output '%s'\n", output_path);
    fprintf(script, "set title 'Relative Error Analysis (compared to long double)' font ',14'\n");
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'Relative Error'\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside bottom center\n");
    fprintf(script, "set datafile separator \",\"\n");
    fprintf(script, "set logscale y\n"); // Logscale is essential for errors
    fprintf(script, "set format y \"%%.1e\"\n"); // Scientific notation for y-axis

    fprintf(script, "plot '%s' using 1:2 title 'f1\\_float' with lines lw 2, \\\n", data_path); // Read from dataDir
    fprintf(script, "     '%s' using 1:3 title 'f1\\_double' with lines lw 2, \\\n", data_path);
    fprintf(script, "     '%s' using 1:4 title 'f2\\_float' with lines lw 2, \\\n", data_path);
    fprintf(script, "     '%s' using 1:5 title 'f2\\_double' with lines lw 2, \\\n", data_path);
    fprintf(script, "     '%s' using 1:6 title 'f3\\_float' with lines lw 2, \\\n", data_path);
    fprintf(script, "     '%s' using 1:7 title 'f3\\_double' with lines lw 2, \\\n", data_path);
    fprintf(script, "     '%s' using 1:8 title 'f4\\_float' with lines lw 2, \\\n", data_path); // Added escaping for underscores in title
    fprintf(script, "     '%s' using 1:9 title 'f4\\_double' with lines lw 2\n", data_path);

    fclose(script);
    printf("Gnuplot script for error analysis created: %s\n", script_path);
}

// --- Modified: generate_individual_plots ---
// Takes dataDir, plotsDir. Generates scripts in plotsDir, reads data from dataDir.
void generate_individual_plots(const char* dataDir, const char* plotsDir, const char* base_name_csv, int num_functions) {
    const char* type_names[] = {"float", "double", "long_double"};
    int num_types = 3;

    char script_path[FILE_PATH_BUFFER_SIZE];
    char output_path[FILE_PATH_BUFFER_SIZE];
    char data_path[FILE_PATH_BUFFER_SIZE];

    // Generate plots for each function and each type
    for (int f = 1; f <= num_functions; f++) {
        // Data file for this function
        snprintf(data_path, sizeof(data_path), "%s/%s_f%d.csv", dataDir, base_name_csv, f);

        for (int t = 0; t < num_types; t++) {
            // Create script name and output file name
            snprintf(script_path, sizeof(script_path), "%s/chart_f%d_%s.gp", plotsDir, f, type_names[t]);
            snprintf(output_path, sizeof(output_path), "%s/chart_f%d_%s.png", plotsDir, f, type_names[t]);

            FILE* script = open_file(script_path, "w", "gnuplot script (individual f/type)");
            if (!script) continue; // Skip if cannot create script

            fprintf(script, "# Gnuplot script generated by C program\n");
            fprintf(script, "# Data file: %s\n\n", data_path);

            // Write gnuplot script
            fprintf(script, "set terminal pngcairo size 800,600 enhanced font 'Verdana,10'\n");
            fprintf(script, "set output '%s'\n", output_path); // Output to plotsDir
            fprintf(script, "set title 'Function %d with %s precision'\n", f, type_names[t]);
            fprintf(script, "set xlabel 'x'\n");
            fprintf(script, "set ylabel 'f%d(x)'\n", f);
            fprintf(script, "set grid\n");
            fprintf(script, "set key top right\n");
            fprintf(script, "set datafile separator \",\"\n");
            // fprintf(script, "set logscale y\n"); // Optional logscale for individual plots

            // Plot using appropriate column (t+2: col 1=x, 2=float, 3=double, 4=long_double)
            fprintf(script, "plot '%s' using 1:%d title 'f%d\\_%s' with linespoints lw 1.5 pt 7 ps 0.6\n", // Read from dataDir
                    data_path, t+2, f, type_names[t]);

            fclose(script);
            printf("Gnuplot script created: %s\n", script_path);
        }
    }
}