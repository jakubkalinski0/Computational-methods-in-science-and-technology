/* utilities.c */
#include "utilities.h"
#include <stdio.h>

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

// Helper function to generate a script for gnuplot for one function
void generate_gnuplot_script(const char* csv_file_name, const char* file_name_png, int function) {
    char script_name[100];
    sprintf(script_name, "wykres_f%d.gp", function);

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

    fprintf(script, "plot '%s' using 1:%d title 'float' with lines lw 2, ", csv_file_name, function * 3 - 1);
    fprintf(script, "'%s' using 1:%d title 'double' with lines lw 2, ", csv_file_name, function * 3);
    fprintf(script, "'%s' using 1:%d title 'long double' with lines lw 2\n", csv_file_name, function * 3 + 1);

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
    fprintf(script, "set title 'Comparison of all functions for a type %s'\n", type);
    fprintf(script, "set xlabel 'x'\n");
    fprintf(script, "set ylabel 'f(x)'\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set key outside\n");

    fprintf(script, "plot '%s' using 1:%d title 'f1_%s' with lines lw 2, ", csv_file_name, 2 + offset, type);
    fprintf(script, "'%s' using 1:%d title 'f2_%s' with lines lw 2, ", csv_file_name, 5 + offset, type);
    fprintf(script, "'%s' using 1:%d title 'f3_%s' with lines lw 2, ", csv_file_name, 8 + offset, type);
    fprintf(script, "'%s' using 1:%d title 'f4_%s' with lines lw 2\n", csv_file_name, 11 + offset, type);

    fclose(script);
    printf("Gnuplot script created: %s\n", script_name);
}