/* utilities.h */
#ifndef UTILITIES_H
#define UTILITIES_H

// Helper function for saving results to CSV file
void save_results(const char* file_name, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n);

// Helper function to generate a script for gnuplot for one function
void generate_gnuplot_script(const char* csv_file_name, const char* file_name_png, int function);

// Function to generate scripts for comparing all functions for a given type
void generate_gnuplot_script_type(const char* csv_file_name, const char* file_name_png, const char* type, int offset);

#endif /* UTILITIES_H */