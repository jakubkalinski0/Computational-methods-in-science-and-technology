/**
 * @file fileio.h
 * @brief Header file for file input/output operations related to polynomial approximation.
 *
 * Defines function prototypes for saving various data sets (like function points,
 * approximation sample points, and approximation errors) to files. Also includes
 * prototypes for generating Gnuplot scripts used to visualize the approximation
 * results and error trends. Assumes standard project directory structure
 * (e.g., 'data/', 'scripts/', 'plots/').
 */
#ifndef FILEIO_H
#define FILEIO_H

#include "common.h" // Includes common definitions like MAX_NODES, a, b
#include <stdio.h> // Provides FILE* type definition (good practice for prototypes using it)

/**
 * @brief Saves a set of (x, y) data points to a specified file within the 'data/' directory.
 *
 * This function is versatile and can be used to save various datasets, such as:
 * - Points representing the original function curve sampled densely.
 * - Points representing the evaluated approximating polynomial curve.
 * The function automatically prepends "data/" to the provided `filename`.
 * It ensures the `data/` directory is expected to exist (scripts ensure its creation).
 *
 * @param filename The base name of the file (e.g., "original_function_plot.dat", "approximation_degree5_points50.dat").
 *                 The final path will be "data/<filename>".
 * @param x Array of x-coordinates for the data points.
 * @param y Array of y-coordinates for the data points.
 * @param n The number of data points to save (size of x and y arrays).
 */
void saveDataToFile(const char* filename, double x[], double y[], int n);

/**
 * @brief Saves the sample points (x_i, y_i) used for generating the least-squares approximation.
 *
 * This function saves the discrete points that the approximation process attempts to fit.
 * Files are saved in the 'data/' subdirectory.
 *
 * @param filename The base name of the file (e.g., "sample_points_n50.dat").
 *                 The final path will be "data/<filename>".
 * @param points_x Array of sample point x-coordinates.
 * @param points_y Array of sample point y-coordinates.
 * @param n The number of sample points.
 */
void saveNodesToFile(const char* filename, double points_x[], double points_y[], int n);

/**
 * @brief Saves approximation errors (maximum absolute and MSE) versus polynomial degree to a CSV file.
 *
 * This function is from the original project's single-n analysis. It might not be needed
 * if the heatmap CSV is the only error output. Kept for compatibility if desired.
 *
 * @param filename The full or relative path name of the CSV file to save the errors to
 *                 (e.g., "data/approximation_errors_vs_degree_n50.csv").
 * @param maxDegree The highest polynomial degree for which error data is provided.
 *                  The input arrays `max_errors` and `mse_errors` must have size at least `maxDegree + 1`.
 * @param max_errors Array containing the maximum absolute errors, indexed by degree [0...maxDegree].
 * @param mse_errors Array containing the mean squared errors, indexed by degree [0...maxDegree].
 * @return 0 on success.
 * @return -1 if the file could not be opened for writing (an error message is printed to stderr).
 */
int saveApproximationErrorsToFile(const char* filename, int maxDegree, double max_errors[], double mse_errors[]);


/**
 * @brief Saves approximation errors (N, M, Max Absolute, MSE) to a CSV file for heatmap plotting.
 *
 * This function is designed to write a single row of data for a specific (n, m) combination
 * to an already opened file stream. The file must be opened and the header written by the caller.
 *
 * @param file The already opened FILE pointer for the heatmap CSV file.
 * @param n The number of sample points.
 * @param m The polynomial degree.
 * @param max_error The calculated maximum absolute error for this (n, m).
 * @param mse_error The calculated mean squared error for this (n, m).
 */
void appendErrorToHeatmapFile(FILE* file, int n, int m, double max_error, double mse_error);


/**
 * @brief Generates a single Gnuplot script ('scripts/plot_all_approximations.gp')
 *        to visualize individual approximation results for ALL (n, m) combinations.
 *
 * Creates a Gnuplot script that, when executed, generates a series of PNG plots,
 * one for each valid (n, m) combination (where m < n) within the analyzed ranges.
 * Each plot compares:
 * 1. The original function `f(x)` (read from 'data/original_function_plot.dat').
 * 2. The approximating polynomial `P_m(x)` (read from 'data/approximation_degree{m}_points{n}.dat').
 * 3. The discrete sample points (x_i, y_i) used for the approximation (read from 'data/sample_points_n{n}.dat').
 * Output plots (.png) are saved in the 'plots/' directory with names like 'plots/approximation_m{m}_n{n}.png'.
 * The script uses Gnuplot's `do for` loops to iterate through n and m,
 * and includes commands to ensure the 'plots/' and 'data/' directories exist.
 *
 * @param min_n Minimum number of sample points included in the data.
 * @param max_n Maximum number of sample points included in the data.
 * @param max_m Maximum polynomial degree included in the data.
 */
void generateAllIndividualApproxScripts(int min_n, int max_n, int max_m);

/**
 * @brief Generates Gnuplot script for plotting the Max Absolute Error heatmap.
 *
 * The plot shows Max Absolute Error as a function of 'm' (x-axis)
 * and 'n' (y-axis, reversed). The color scale (z-axis) is logarithmic
 * with a fixed minimum to better show variation in small errors.
 *
 * @param min_n Minimum number of sample points in the data.
 * @param max_n Maximum number of sample points in the data.
 * @param max_m Maximum polynomial degree in the data.
 */
void generateApproxMaxErrorHeatmapScript(int min_n, int max_n, int max_m);

/**
 * @brief Generates Gnuplot script for plotting the Mean Squared Error heatmap.
 *
 * The plot shows MSE as a function of 'm' (x-axis)
 * and 'n' (y-axis, reversed). The color scale (z-axis) is logarithmic
 * with a fixed minimum to better show variation in small errors.
 *
 * @param min_n Minimum number of sample points in the data.
 * @param max_n Maximum number of sample points in the data.
 * @param max_m Maximum polynomial degree in the data.
 */
void generateApproxMseHeatmapScript(int min_n, int max_n, int max_m);

#endif // FILEIO_H