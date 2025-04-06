/* utilities.h */
#ifndef UTILITIES_H
#define UTILITIES_H

#include <math.h>

// Helper function to save results to separate CSV files
void save_results_to_separate_files(const char* dataDir, const char* base_name_sep, const char* base_name_orig, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n);

// Original helper function for saving results to CSV file
void save_results(const char* dataDir, const char* base_name, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n);

// Function to generate a script for each function comparing all types
void generate_gnuplot_script_function_all_types(const char* dataDir, const char* plotsDir, const char* base_name_csv, int function);

// Function to generate a script for a collage of plots
void generate_multiplot_script(const char* dataDir, const char* plotsDir, const char* base_name_csv, int num_functions, int num_types);

// Original helper function to generate a script for gnuplot for one function
void generate_gnuplot_script(const char* dataDir, const char* plotsDir, const char* base_name_csv, int function);

// Original function to generate scripts for comparing all functions for a given type
void generate_gnuplot_script_type(const char* dataDir, const char* plotsDir, const char* base_name_csv, const char* type, int offset);

// Function to create detailed error analysis
void generate_error_analysis(const char* dataDir, const char* plotsDir, const char* base_name_csv, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n);

// Function to generate individual plots for each function and variable type combination
void generate_individual_plots(const char* dataDir, const char* plotsDir, const char* base_name_csv, int num_functions);

#endif /* UTILITIES_H */