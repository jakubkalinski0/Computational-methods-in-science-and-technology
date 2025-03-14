/* main.c */
#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "utilities.h"

int main() {
    const int N = 101;  // Number of points
    const float xmin = 0.99f;
    const float xmax = 1.01f;

    // Memory allocation for x values and results
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

    // Memory allocation check
    if (!values_x || !f1_float || !f1_double || !f1_long_double ||
        !f2_float || !f2_double || !f2_long_double ||
        !f3_float || !f3_double || !f3_long_double ||
        !f4_float || !f4_double || !f4_long_double) {
        printf("Memory allocation error.\n");
        return 1;
    }

    // Generating equidistant x values
    float step = (xmax - xmin) / (N - 1);
    for (int i = 0; i < N; i++) {
        values_x[i] = xmin + i * step;
    }

    // Calculating the value of a function for each x
    for (int i = 0; i < N; i++) {
        float x_float = values_x[i];
        double x_double = (double)x_float;
        long double x_long_double = (long double)x_float;

        // Function 1
        f1_float[i] = function1_float(x_float);
        f1_double[i] = function1_double(x_double);
        f1_long_double[i] = function1_long_double(x_long_double);

        // Function 2
        f2_float[i] = function2_float(x_float);
        f2_double[i] = function2_double(x_double);
        f2_long_double[i] = function2_long_double(x_long_double);

        // Function 3
        f3_float[i] = function3_float(x_float);
        f3_double[i] = function3_double(x_double);
        f3_long_double[i] = function3_long_double(x_long_double);

        // Function 4
        f4_float[i] = function4_float(x_float);
        f4_double[i] = function4_double(x_double);
        f4_long_double[i] = function4_long_double(x_long_double);
    }

    // Saving results to a CSV file
    save_results("results.csv", values_x,
                 f1_float, f1_double, f1_long_double,
                 f2_float, f2_double, f2_long_double,
                 f3_float, f3_double, f3_long_double,
                 f4_float, f4_double, f4_long_double,
                 N);

    // Generating scripts for gnuplot
    // Scripts for individual functions
    for (int i = 1; i <= 4; i++) {
        char file_name[100];
        sprintf(file_name, "chart_f%d.png", i);
        generate_gnuplot_script("results.csv", file_name, i);
    }

    // Scripts for specific variable types
    generate_gnuplot_script_type("results.csv", "chart_float.png", "float", 0);
    generate_gnuplot_script_type("results.csv", "chart_double.png", "double", 1);
    generate_gnuplot_script_type("results.csv", "chart_long_double.png", "long_double", 2);

    // Comparison of results between functions and variable types
    printf("Comparison of results between functions and variable types:\n");

    // Checking the differences between functions for x = 1.0
    int idx_middle = N / 2; // Index for x = 1.0

    printf("\nWartości dla x = %.10f:\n", (double)values_x[idx_middle]);
    printf("Function 1 (float): %.10e\n", (double)f1_float[idx_middle]);
    printf("Function 1 (double): %.10e\n", f1_double[idx_middle]);
    printf("Function 1 (long double): %.10Le\n", f1_long_double[idx_middle]);

    printf("\nFunction 2 (float): %.10e\n", (double)f2_float[idx_middle]);
    printf("Function 2 (double): %.10e\n", f2_double[idx_middle]);
    printf("Function 2 (long double): %.10Le\n", f2_long_double[idx_middle]);

    printf("\nFunction 3 (float): %.10e\n", (double)f3_float[idx_middle]);
    printf("Function 3 (double): %.10e\n", f3_double[idx_middle]);
    printf("Function 3 (long double): %.10Le\n", f3_long_double[idx_middle]);

    printf("\nFunction 4 (float): %.10e\n", (double)f4_float[idx_middle]);
    printf("Function 4 (double): %.10e\n", f4_double[idx_middle]);
    printf("Function 4 (long double): %.10Le\n", f4_long_double[idx_middle]);

    // Freeing up memory
    free(values_x);
    free(f1_float); free(f1_double); free(f1_long_double);
    free(f2_float); free(f2_double); free(f2_long_double);
    free(f3_float); free(f3_double); free(f3_long_double);
    free(f4_float); free(f4_double); free(f4_long_double);

    printf("\nExecuting gnuplot scripts...\n");
    system("gnuplot chart_f1.gp");
    system("gnuplot chart_f2.gp");
    system("gnuplot chart_f3.gp");
    system("gnuplot chart_f4.gp");
    system("gnuplot chart_float.gp");
    system("gnuplot chart_double.gp");
    system("gnuplot chart_long_double.gp");

    printf("Calculations and graph generation completed.\n");

    return 0;
}