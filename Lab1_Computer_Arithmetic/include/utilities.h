/* utilities.h */
#ifndef UTILITIES_H
#define UTILITIES_H

#include <math.h> // Included for potential use by implementations, e.g., NAN, fabs

/**
 * @file utilities.h
 * @brief Header file containing declarations for utility functions.
 *        These functions handle tasks like saving computed results to CSV files
 *        and generating Gnuplot scripts for visualization and error analysis.
 *        Functions require directory paths for input data, output scripts, and plot images.
 */

// Define buffer size for constructing file paths to avoid magic numbers
#define FILE_PATH_BUFFER_SIZE 256

/**
 * @brief Helper function to save computed results into separate CSV files, one per function.
 *        Each file contains columns: x, float_result, double_result, long_double_result.
 *        Also calls the original `save_results` to maintain the combined CSV format.
 * @param dataDir Path to the directory where data files (.csv) will be saved.
 * @param base_name_sep Base filename for the separate CSV files (e.g., "separate_results").
 *                      Actual filenames will be like "separate_results_f1.csv", etc.
 * @param base_name_orig Base filename for the original combined CSV file (e.g., "results").
 * @param values_x Array of x-coordinates.
 * @param f1_float, f1_double, f1_long_double Arrays of results for Function 1.
 * @param f2_float, f2_double, f2_long_double Arrays of results for Function 2.
 * @param f3_float, f3_double, f3_long_double Arrays of results for Function 3.
 * @param f4_float, f4_double, f4_long_double Arrays of results for Function 4.
 * @param n Number of data points.
 */
void save_results_to_separate_files(const char* dataDir, const char* base_name_sep, const char* base_name_orig, float* values_x,
                                    float* f1_float, double* f1_double, long double* f1_long_double,
                                    float* f2_float, double* f2_double, long double* f2_long_double,
                                    float* f3_float, double* f3_double, long double* f3_long_double,
                                    float* f4_float, double* f4_double, long double* f4_long_double,
                                    int n);

/**
 * @brief Original helper function for saving all results into a single, wide CSV file.
 *        Columns: x, f1_float, f1_double, ..., f4_long_double.
 * @param dataDir Path to the directory where the combined data file (.csv) will be saved.
 * @param base_name Base filename for the combined CSV file (e.g., "results").
 * @param values_x Array of x-coordinates.
 * @param f1_float, f1_double, f1_long_double Arrays of results for Function 1.
 * @param f2_float, f2_double, f2_long_double Arrays of results for Function 2.
 * @param f3_float, f3_double, f3_long_double Arrays of results for Function 3.
 * @param f4_float, f4_double, f4_long_double Arrays of results for Function 4.
 * @param n Number of data points.
 */
void save_results(const char* dataDir, const char* base_name, float* values_x,
                  float* f1_float, double* f1_double, long double* f1_long_double,
                  float* f2_float, double* f2_double, long double* f2_long_double,
                  float* f3_float, double* f3_double, long double* f3_long_double,
                  float* f4_float, double* f4_double, long double* f4_long_double,
                  int n);

/**
 * @brief Generates a Gnuplot script (.gp) to plot the results of a specific function
 *        comparing all three precision types (float, double, long double).
 *        Reads data from the separate CSV file for that function.
 * @param dataDir Path to the directory containing the input data CSV files.
 * @param gpScriptsDir Path to the directory where the generated Gnuplot script (.gp) will be saved.
 * @param plotImagesDir Path to the directory where the generated plot image (.png) should be saved (specified inside the .gp script).
 * @param base_name_csv Base name of the separate data files (e.g., "separate_results").
 * @param function The function number (1-4) to generate the script for.
 */
void generate_gnuplot_script_function_all_types(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, int function);

/**
 * @brief Generates a Gnuplot script (.gp) for a multiplot collage, showing a grid
 *        of plots, with each plot representing one function and one precision type.
 *        Reads data from the separate CSV files (one per function).
 * @param dataDir Path to the directory containing the input data CSV files.
 * @param gpScriptsDir Path to the directory where the generated Gnuplot script (.gp) will be saved.
 * @param plotImagesDir Path to the directory where the generated plot image (.png) should be saved (specified inside the .gp script).
 * @param base_name_csv Base name of the separate data files (e.g., "separate_results").
 * @param num_functions Total number of functions (e.g., 4).
 * @param num_types Total number of precision types (e.g., 3 for float, double, long double).
 */
void generate_multiplot_script(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, int num_functions, int num_types);

/**
 * @brief Original helper function to generate a Gnuplot script (.gp) for one function,
 *        comparing all precision types. Reads data from the combined CSV file.
 * @param dataDir Path to the directory containing the combined input data CSV file.
 * @param gpScriptsDir Path to the directory where the generated Gnuplot script (.gp) will be saved.
 * @param plotImagesDir Path to the directory where the generated plot image (.png) should be saved (specified inside the .gp script).
 * @param base_name_csv Base name of the combined data file (e.g., "results").
 * @param function The function number (1-4) to generate the script for.
 */
void generate_gnuplot_script(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, int function);

/**
 * @brief Original function to generate a Gnuplot script (.gp) comparing all functions
 *        for a single, given precision type. Reads data from the combined CSV file.
 * @param dataDir Path to the directory containing the combined input data CSV file.
 * @param gpScriptsDir Path to the directory where the generated Gnuplot script (.gp) will be saved.
 * @param plotImagesDir Path to the directory where the generated plot image (.png) should be saved (specified inside the .gp script).
 * @param base_name_csv Base name of the combined data file (e.g., "results").
 * @param type String identifier for the type (e.g., "float", "double").
 * @param offset Column offset in the combined CSV file corresponding to the type.
 */
void generate_gnuplot_script_type(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, const char* type, int offset);

/**
 * @brief Creates a CSV file containing relative error data and generates a Gnuplot script (.gp)
 *        to visualize this error analysis. The error is calculated for float and double results
 *        relative to the long double result (considered as the reference).
 * @param dataDir Path to the directory where the error analysis CSV file will be saved.
 * @param gpScriptsDir Path to the directory where the generated Gnuplot script (.gp) will be saved.
 * @param plotImagesDir Path to the directory where the generated plot image (.png) should be saved (specified inside the .gp script).
 * @param base_name_csv Base name used for naming the output files (e.g., "separate_results").
 * @param values_x Array of x-coordinates.
 * @param f1_float, f1_double, f1_long_double Arrays of results for Function 1.
 * @param f2_float, f2_double, f2_long_double Arrays of results for Function 2.
 * @param f3_float, f3_double, f3_long_double Arrays of results for Function 3.
 * @param f4_float, f4_double, f4_long_double Arrays of results for Function 4.
 * @param n Number of data points.
 */
void generate_error_analysis(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, float* values_x,
                             float* f1_float, double* f1_double, long double* f1_long_double,
                             float* f2_float, double* f2_double, long double* f2_long_double,
                             float* f3_float, double* f3_double, long double* f3_long_double,
                             float* f4_float, double* f4_double, long double* f4_long_double,
                             int n);

/**
 * @brief Generates individual Gnuplot scripts (.gp), one for each combination of
 *        function and precision type (e.g., function 1 - float, function 1 - double, etc.).
 *        Each script plots a single line corresponding to that specific combination.
 *        Reads data from the separate CSV files (one per function).
 * @param dataDir Path to the directory containing the input data CSV files.
 * @param gpScriptsDir Path to the directory where the generated Gnuplot scripts (.gp) will be saved.
 * @param plotImagesDir Path to the directory where the generated plot images (.png) should be saved (specified inside the .gp scripts).
 * @param base_name_csv Base name of the separate data files (e.g., "separate_results").
 * @param num_functions Total number of functions (e.g., 4).
 */
void generate_individual_plots(const char* dataDir, const char* gpScriptsDir, const char* plotImagesDir, const char* base_name_csv, int num_functions);

#endif /* UTILITIES_H */