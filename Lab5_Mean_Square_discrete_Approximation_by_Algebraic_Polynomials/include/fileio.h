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
 * @param filename The base name of the file (e.g., "original_function.dat", "approximation_degree5.dat").
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
 * Files are saved in the 'data/' subdirectory. Although named `saveNodesToFile` for
 * historical reasons (reuse from interpolation code), in the context of approximation,
 * these are the *sample points*, not necessarily interpolation nodes.
 *
 * @param filename The base name of the file (e.g., "sample_points.dat").
 *                 The final path will be "data/<filename>".
 * @param points_x Array of x-coordinates of the sample points.
 * @param points_y Array of y-coordinates (function values f(x_i)) of the sample points.
 * @param n The number of sample points.
 */
void saveNodesToFile(const char* filename, double points_x[], double points_y[], int n); // Function reused for sample points

/**
 * @brief Saves approximation errors (maximum absolute and MSE) versus polynomial degree to a CSV file.
 *
 * Creates a CSV file at the specified `filename` path. It's recommended this path
 * includes the "data/" subdirectory (e.g., "data/approximation_errors.csv").
 * The CSV file contains columns: Degree, MaxAbsoluteError, MeanSquaredError.
 * This file is typically used as input for plotting error trends.
 *
 * @param filename The full or relative path name of the CSV file to save the errors to
 *                 (e.g., "data/approximation_errors_vs_degree.csv").
 * @param maxDegree The highest polynomial degree for which error data is provided.
 *                  The input arrays `max_errors` and `mse_errors` must have size at least `maxDegree + 1`.
 * @param max_errors Array containing the maximum absolute errors, indexed by degree [0...maxDegree].
 * @param mse_errors Array containing the mean squared errors, indexed by degree [0...maxDegree].
 * @return 0 on success.
 * @return -1 if the file could not be opened for writing (an error message is printed to stderr).
 */
int saveApproximationErrorsToFile(const char* filename, int maxDegree, double max_errors[], double mse_errors[]);

/**
 * @brief Generates a Gnuplot script ('scripts/plot_approximations.gp') to visualize individual approximation results.
 *
 * Creates a Gnuplot script that, when executed, generates a series of PNG plots,
 * one for each polynomial degree `m` from 0 to `maxDegree`. Each plot compares:
 * 1. The original function `f(x)` (read from 'data/original_function.dat').
 * 2. The approximating polynomial `P_m(x)` (read from 'data/approximation_degree{m}_points{n}.dat').
 * 3. The discrete sample points (x_i, y_i) used for the approximation (read from 'data/sample_points.dat').
 * Output plots (.png) are saved in the 'plots/' directory with names like 'plots/approximation_m{m}_n{numSamplePoints}.png'.
 * The script includes commands to ensure the 'plots/' and 'data/' directories exist.
 *
 * @param maxDegree The maximum polynomial degree `m` for which plots will be generated.
 * @param numSamplePoints The number of sample points `n` used in the approximation (included in filenames and titles).
 */
void generateApproxGnuplotScript(int maxDegree, int numSamplePoints);

/**
 * @brief Generates a Gnuplot script ('scripts/plot_approx_errors.gp') to visualize approximation errors vs. degree.
 *
 * Creates a Gnuplot script that plots the maximum absolute error and Mean Squared Error (MSE)
 * as a function of the approximating polynomial degree `m`.
 * The script reads data from a CSV file (typically 'data/approximation_errors_vs_degree.csv'
 * as generated by `saveApproximationErrorsToFile`).
 * The output plot is saved as 'plots/approximation_errors.png'.
 * It uses a logarithmic scale for the y-axis (error) for better visualization of trends
 * across potentially many orders of magnitude.
 * The script includes commands to ensure the 'plots/' and 'data/' directories exist.
 *
 * @param maxDegree The maximum polynomial degree `m` plotted on the x-axis. This determines the x-range of the plot.
 */
void generateApproxErrorPlotScript(int maxDegree);


#endif // FILEIO_H