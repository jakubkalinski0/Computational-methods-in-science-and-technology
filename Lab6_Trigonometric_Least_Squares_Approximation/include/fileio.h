/**
 * @file fileio.h
 * @brief Header file for file input/output operations related to trigonometric approximation.
 *
 * Defines function prototypes for saving data sets (function points, sample points,
 * approximated sum points) and approximation errors to files. Includes prototypes
 * for generating Gnuplot scripts to visualize approximation results and error trends,
 * assuming a standard project directory structure ('data/', 'scripts/', 'plots/').
 * Adjusted for parametrization by max harmonic 'm'.
 */
#ifndef FILEIO_H
#define FILEIO_H

#include "common.h" // Includes common definitions like MAX_NODES, a, b, omega
#include <stdio.h> // Provides FILE* type definition

/**
 * @brief Saves a set of (x, y) data points to a specified file within the 'data/' directory.
 *
 * Versatile function for saving original function, evaluated trigonometric sum, etc.
 * Automatically prepends "data/" to the filename.
 *
 * @param filename Base name of the file (e.g., "original_function_plot.dat", "trig_approx_m5_points50.dat").
 *                 Final path will be "data/<filename>".
 * @param x Array of x-coordinates.
 * @param y Array of y-coordinates.
 * @param n Number of data points to save.
 */
void saveDataToFile(const char* filename, double x[], double y[], int n);

/**
 * @brief Saves the sample points (x_i, y_i) used for generating the least-squares trigonometric approximation.
 *
 * Saves the discrete points used as input to the coefficient calculation function.
 * Files are saved in the 'data/' subdirectory.
 *
 * @param filename Base name of the file (e.g., "sample_points_n50.dat").
 *                 Final path will be "data/<filename>".
 * @param points_x Array of sample point x-coordinates.
 * @param points_y Array of sample point y-coordinates.
 * @param n Number of sample points.
 */
void saveNodesToFile(const char* filename, double points_x[], double points_y[], int n);

/**
 * @brief Appends approximation errors (N, m, Max Absolute, MSE) to a CSV file for heatmap plotting.
 *
 * Designed to write a single row of data for a specific (n, m) combination
 * to an already opened file stream. The file must be opened and the header
 * written by the caller. Handles NAN values appropriately.
 *
 * @param file The already opened FILE pointer for the heatmap CSV file.
 * @param n The number of sample points.
 * @param m The maximum harmonic order used in the approximation.
 * @param max_error The calculated maximum absolute error for this (n, m). Should be NAN if m >= n/2 or calculation failed.
 * @param mse_error The calculated mean squared error for this (n, m). Should be NAN if m >= n/2 or calculation failed.
 */
void appendErrorToHeatmapFile(FILE* file, int n, int m, double max_error, double mse_error);


/**
 * @brief Generates a single Gnuplot script ('scripts/plot_all_trig_approximations.gp')
 *        to visualize individual trigonometric approximation results for valid (n, m) combinations (m < n/2).
 *
 * Creates a Gnuplot script that, when executed, generates a series of PNG plots,
 * one for each valid (n, m) combination where the condition `m < n/2` holds.
 * Each plot compares:
 * 1. The original function `f(x)` (read from 'data/original_function_plot.dat').
 * 2. The approximating trigonometric sum `T_m(x)` (read from 'data/trig_approx_m{m}_points{n}.dat').
 *    The script includes a check to see if this file exists before trying to plot it.
 * 3. The discrete sample points (x_i, y_i) used for the approximation (read from 'data/sample_points_n{n}.dat').
 *
 * Output plots (.png) are saved in the 'plots/' directory with names like 'plots/trig_approx_m{m}_n{n}.png'.
 * The script uses Gnuplot's `do for` loops and includes commands to ensure directories exist.
 * It incorporates Gnuplot's `fileexists()` function (Gnuplot >= 5.2) or simulates it to handle cases where
 * the approximation data file might not have been generated (due to m >= n/2 or other errors).
 *
 * @param min_n Minimum number of sample points included in the data.
 * @param max_n Maximum number of sample points included in the data.
 * @param max_m Maximum harmonic order included in the data (used as the upper loop limit for m).
 */
void generateAllIndividualTrigApproxScripts(int min_n, int max_n, int max_m);

#endif // FILEIO_H