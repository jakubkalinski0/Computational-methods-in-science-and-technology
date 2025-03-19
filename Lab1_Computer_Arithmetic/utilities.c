/* utilities.c */
#include "utilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>

// Helper function to save results to separate CSV files
void save_results_to_separate_files(const char* base_name, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n) {

    char file_name[100];

    // File for function 1
    sprintf(file_name, "%s_f1.csv", base_name);
    FILE* file1 = fopen(file_name, "w");
    if (file1) {
        fprintf(file1, "x,float,double,long_double\n");
        for (int i = 0; i < n; i++) {
            fprintf(file1, "%f,%e,%e,%Le\n",
                    (double)values_x[i],
                    (double)f1_float[i], f1_double[i], f1_long_double[i]);
        }
        fclose(file1);
        printf("Results for function 1 saved to file %s.\n", file_name);
    } else {
        printf("Could not open file %s for writing.\n", file_name);
    }

    // File for function 2
    sprintf(file_name, "%s_f2.csv", base_name);
    FILE* file2 = fopen(file_name, "w");
    if (file2) {
        fprintf(file2, "x,float,double,long_double\n");
        for (int i = 0; i < n; i++) {
            fprintf(file2, "%f,%e,%e,%Le\n",
                    (double)values_x[i],
                    (double)f2_float[i], f2_double[i], f2_long_double[i]);
        }
        fclose(file2);
        printf("Results for function 2 saved to file %s.\n", file_name);
    } else {
        printf("Could not open file %s for writing.\n", file_name);
    }

    // File for function 3
    sprintf(file_name, "%s_f3.csv", base_name);
    FILE* file3 = fopen(file_name, "w");
    if (file3) {
        fprintf(file3, "x,float,double,long_double\n");
        for (int i = 0; i < n; i++) {
            fprintf(file3, "%f,%e,%e,%Le\n",
                    (double)values_x[i],
                    (double)f3_float[i], f3_double[i], f3_long_double[i]);
        }
        fclose(file3);
        printf("Results for function 3 saved to file %s.\n", file_name);
    } else {
        printf("Could not open file %s for writing.\n", file_name);
    }

    // File for function 4
    sprintf(file_name, "%s_f4.csv", base_name);
    FILE* file4 = fopen(file_name, "w");
    if (file4) {
        fprintf(file4, "x,float,double,long_double\n");
        for (int i = 0; i < n; i++) {
            fprintf(file4, "%f,%e,%e,%Le\n",
                    (double)values_x[i],
                    (double)f4_float[i], f4_double[i], f4_long_double[i]);
        }
        fclose(file4);
        printf("Results for function 4 saved to file %s.\n", file_name);
    } else {
        printf("Could not open file %s for writing.\n", file_name);
    }

    // Keep the original format for compatibility
    save_results("results.csv", values_x,
                 f1_float, f1_double, f1_long_double,
                 f2_float, f2_double, f2_long_double,
                 f3_float, f3_double, f3_long_double,
                 f4_float, f4_double, f4_long_double,
                 n);
}

// Helper function for saving results to CSV file
void save_results(const char* file_name, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n) {
    FILE* file = fopen(file_name, "w");
    if (!file) {
        printf("Could not open file %s for writing.\n", file_name);
        return;
    }

    // Column Headings
    fprintf(file, "x,f1_float,f1_double,f1_long_double,f2_float,f2_double,f2_long_double,");
    fprintf(file, "f3_float,f3_double,f3_long_double,f4_float,f4_double,f4_long_double\n");

    // Data
    for (int i = 0; i < n; i++) {
        fprintf(file, "%f,%e,%e,%Le,%e,%e,%Le,%e,%e,%Le,%e,%e,%Le\n",
                (double)values_x[i],
                (double)f1_float[i], f1_double[i], f1_long_double[i],
                (double)f2_float[i], f2_double[i], f2_long_double[i],
                (double)f3_float[i], f3_double[i], f3_long_double[i],
                (double)f4_float[i], f4_double[i], f4_long_double[i]);
    }

    fclose(file);
    printf("Results saved to file %s.\n", file_name);
}

// Function to generate a script for each function comparing all types
void generate_gnuplot_script_function_all_types(const char* csv_file_name, int function) {
    char script_name[100];
    char output_name[100];
    sprintf(script_name, "chart_f%d_all_types.gp", function);
    sprintf(output_name, "chart_f%d_all_types.png", function);

    FILE* script = fopen(script_name, "w");
    if (!script) {
        printf("Could not create gnuplot script.\n");
        return;
    }

    fprintf(script, "set terminal png size 1200,800\n");
    fprintf(script, "set output '%s'\n", output_name);
    fprintf(script, "set title 'Function %d: Comparison of all variable types'\n", function);
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'f%d(x)'\n", function);
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside\n");
    fprintf(script, "set datafile separator \",\"\n");
    fprintf(script, "set logscale y\n");

    fprintf(script, "plot '%s_f%d.csv' using 1:2 title 'float' with points pt 7 ps 1, ",
            csv_file_name, function);
    fprintf(script, "'%s_f%d.csv' using 1:3 title 'double' with points pt 9 ps 1, ",
            csv_file_name, function);
    fprintf(script, "'%s_f%d.csv' using 1:4 title 'long double' with points pt 11 ps 1\n",
            csv_file_name, function);

    fclose(script);
    printf("Gnuplot script created: %s\n", script_name);
}

// Function to generate a script for a collage of plots
void generate_multiplot_script(const char* base_name, int num_functions, int num_types) {
    FILE* script = fopen("chart_collage.gp", "w");
    if (!script) {
        printf("Could not create gnuplot script for collage.\n");
        return;
    }

    fprintf(script, "set terminal png size 1600,1200\n");
    fprintf(script, "set output 'chart_collage.png'\n");
    fprintf(script, "set datafile separator \",\"\n");
    fprintf(script, "set grid\n");
    // fprintf(script, "set logscale y\n");

    // Grid layout for plots
    int rows = num_functions;
    int cols = num_types;
    fprintf(script, "set multiplot layout %d,%d title 'Comparison of functions and variable types'\n", rows, cols);

    const char* type_names[] = {"float", "double", "long_double"};

    // Generate each plot in the collage
    for (int f = 1; f <= num_functions; f++) {
        for (int t = 0; t < num_types; t++) {
            char csv_file[100];
            sprintf(csv_file, "%s_f%d.csv", base_name, f);

            fprintf(script, "set title 'Function %d - %s'\n", f, type_names[t]);
            fprintf(script, "set xlabel 'x'\n");
            fprintf(script, "set ylabel 'f(x)'\n");
            fprintf(script, "plot '%s' using 1:%d title '%s' with points pointtype 7 pointsize 1\n",
                    csv_file, t+2, type_names[t]);
        }
    }

    fprintf(script, "unset multiplot\n");
    fclose(script);
    printf("Gnuplot script for collage created: chart_collage.gp\n");
}

// Helper function to generate a script for gnuplot for one function
void generate_gnuplot_script(const char* csv_file_name, const char* file_name_png, int function) {
    char script_name[100];
    sprintf(script_name, "chart_f%d.gp", function);

    FILE* script = fopen(script_name, "w");
    if (!script) {
        printf("Could not create gnuplot script.\n");
        return;
    }

    fprintf(script, "set terminal png size 1200,800\n");
    fprintf(script, "set output '%s'\n", file_name_png);
    fprintf(script, "set title 'Comparison of f%d function results for different variable types'\n", function);
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'f(x)'\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside\n");
    fprintf(script, "set datafile separator \",\"\n");
    // fprintf(script, "set logscale y\n");

    fprintf(script, "plot '%s' using 1:%d title 'float' with points pointtype 7 pointsize 1, ", csv_file_name, function * 3 - 1);
    fprintf(script, "'%s' using 1:%d title 'double' with points pointtype 7 pointsize 1, ", csv_file_name, function * 3);
    fprintf(script, "'%s' using 1:%d title 'long double' with points pointtype 7 pointsize 1\n", csv_file_name, function * 3 + 1);

    fclose(script);
    printf("gnuplot script created: %s\n", script_name);
}

// Function to generate scripts for comparing all functions for a given type
void generate_gnuplot_script_type(const char* csv_file_name, const char* file_name_png, const char* type, int offset) {
    char script_name[100];
    sprintf(script_name, "chart_%s.gp", type);

    FILE* script = fopen(script_name, "w");
    if (!script) {
        printf("Could not create gnuplot script.\n");
        return;
    }

    fprintf(script, "set terminal png size 1200,800\n");
    fprintf(script, "set output '%s'\n", file_name_png);
    fprintf(script, "set title 'Comparison of all functions for type %s'\n", type);
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'f(x)'\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside\n");
    fprintf(script, "set datafile separator \",\"\n");
    fprintf(script, "set logscale y\n");

    fprintf(script, "plot '%s' using 1:%d title 'f1_%s' with points pointtype 7 pointsize 1, ", csv_file_name, 2 + offset, type);
    fprintf(script, "'%s' using 1:%d title 'f2_%s' with points pointtype 7 pointsize 1, ", csv_file_name, 5 + offset, type);
    fprintf(script, "'%s' using 1:%d title 'f3_%s' with points pointtype 7 pointsize 1, ", csv_file_name, 8 + offset, type);
    fprintf(script, "'%s' using 1:%d title 'f4_%s' with points pointtype 7 pointsize 1\n", csv_file_name, 11 + offset, type);

    fclose(script);
    printf("Gnuplot script created: %s\n", script_name);
}

// Function to create detailed error analysis
void generate_error_analysis(const char* base_name, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n) {

    char file_name[100];
    sprintf(file_name, "%s_error_analysis.csv", base_name);

    FILE* file = fopen(file_name, "w");
    if (!file) {
        printf("Could not open file %s for writing.\n", file_name);
        return;
    }

    // Column headings
    fprintf(file, "x,f1_float_error,f1_double_error,f2_float_error,f2_double_error,");
    fprintf(file, "f3_float_error,f3_double_error,f4_float_error,f4_double_error\n");

    // Calculate relative errors using long double as reference
    for (int i = 0; i < n; i++) {
        double f1_float_error = fabs(((double)f1_float[i] - (double)f1_long_double[i]) / (double)f1_long_double[i]);
        double f1_double_error = fabs((f1_double[i] - (double)f1_long_double[i]) / (double)f1_long_double[i]);

        double f2_float_error = fabs(((double)f2_float[i] - (double)f2_long_double[i]) / (double)f2_long_double[i]);
        double f2_double_error = fabs((f2_double[i] - (double)f2_long_double[i]) / (double)f2_long_double[i]);

        double f3_float_error = fabs(((double)f3_float[i] - (double)f3_long_double[i]) / (double)f3_long_double[i]);
        double f3_double_error = fabs((f3_double[i] - (double)f3_long_double[i]) / (double)f3_long_double[i]);

        double f4_float_error = fabs(((double)f4_float[i] - (double)f4_long_double[i]) / (double)f4_long_double[i]);
        double f4_double_error = fabs((f4_double[i] - (double)f4_long_double[i]) / (double)f4_long_double[i]);

        fprintf(file, "%f,%e,%e,%e,%e,%e,%e,%e,%e\n",
                (double)values_x[i],
                f1_float_error, f1_double_error,
                f2_float_error, f2_double_error,
                f3_float_error, f3_double_error,
                f4_float_error, f4_double_error);
    }

    fclose(file);
    printf("Error analysis saved to file %s.\n", file_name);

    // Create gnuplot script for error analysis
    sprintf(file_name, "chart_error_analysis.gp");
    FILE* script = fopen(file_name, "w");
    if (!script) {
        printf("Could not create gnuplot script for error analysis.\n");
        return;
    }

    fprintf(script, "set terminal png size 1600,1200\n");
    fprintf(script, "set output 'chart_error_analysis.png'\n");
    fprintf(script, "set title 'Relative Error Analysis (compared to long double)'\n");
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'Relative Error'\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside\n");
    fprintf(script, "set datafile separator \",\"\n");
    // fprintf(script, "set logscale y\n");

    fprintf(script, "plot '%s_error_analysis.csv' using 1:2 title 'f1_float' with lines lw 2, ", base_name);
    fprintf(script, "'%s_error_analysis.csv' using 1:3 title 'f1_double' with lines lw 2, ", base_name);
    fprintf(script, "'%s_error_analysis.csv' using 1:4 title 'f2_float' with lines lw 2, ", base_name);
    fprintf(script, "'%s_error_analysis.csv' using 1:5 title 'f2_double' with lines lw 2, ", base_name);
    fprintf(script, "'%s_error_analysis.csv' using 1:6 title 'f3_float' with lines lw 2, ", base_name);
    fprintf(script, "'%s_error_analysis.csv' using 1:7 title 'f3_double' with lines lw 2, ", base_name);
    fprintf(script, "'%s_error_analysis.csv' using 1:8 title 'f4_float' with lines lw 2, ", base_name);
    fprintf(script, "'%s_error_analysis.csv' using 1:9 title 'f4_double' with lines lw 2\n", base_name);

    fclose(script);
    printf("Gnuplot script for error analysis created: %s\n", file_name);
}

// Function to generate individual plots for each function and variable type combination
void generate_individual_plots(const char* base_name, int num_functions) {
    const char* type_names[] = {"float", "double", "long_double"};
    int num_types = 3;

    char script_name[100];
    char output_name[100];
    char csv_file[100];

    // Generate plots for each function and each type
    for (int f = 1; f <= num_functions; f++) {
        sprintf(csv_file, "%s_f%d.csv", base_name, f);

        for (int t = 0; t < num_types; t++) {
            // Create script name and output file name
            sprintf(script_name, "chart_f%d_%s.gp", f, type_names[t]);
            sprintf(output_name, "chart_f%d_%s.png", f, type_names[t]);

            FILE* script = fopen(script_name, "w");
            if (!script) {
                printf("Could not create gnuplot script for f%d_%s.\n", f, type_names[t]);
                continue;
            }

            // Write gnuplot script
            fprintf(script, "set terminal png size 800,600\n");
            fprintf(script, "set output '%s'\n", output_name);
            fprintf(script, "set title 'Function %d with %s precision'\n", f, type_names[t]);
            fprintf(script, "set xlabel 'x'\n");
            fprintf(script, "set ylabel 'f%d(x)'\n", f);
            fprintf(script, "set grid\n");
            fprintf(script, "set datafile separator \",\"\n");
            // fprintf(script, "set logscale y\n");

            // Plot using appropriate column (t+2 because column 1 is x, and columns 2,3,4 are float,double,long_double)
            fprintf(script, "plot '%s' using 1:%d title 'f%d_%s' with linespoints lw 2 pt 7 ps 1\n",
                    csv_file, t+2, f, type_names[t]);

            fclose(script);
            printf("Gnuplot script created: %s\n", script_name);
        }
    }
}