/**
 * @file fileio.h
 * @brief Header file for file input/output operations related to root finding.
 *
 * Defines function prototypes for saving root-finding results to a CSV file
 * and for generating Gnuplot scripts to visualize the function and iteration counts.
 */
#ifndef FILEIO_H
#define FILEIO_H

#include "common.h"       // Includes common definitions like a, b
#include "root_finding.h" // Includes RootResult struct definition
#include <stdio.h>        // Provides FILE* type definition

/**
 * @brief Opens and prepares the main CSV file for storing all root-finding results.
 *
 * Creates the file (overwriting if it exists) and writes the header row.
 * The caller is responsible for closing the file using fclose().
 *
 * @param filename The base name of the CSV file (e.g., "root_finding_results.csv").
 *                 The final path will be "data/<filename>".
 * @return A FILE pointer to the opened file, or NULL on failure. Prints error to stderr on failure.
 */
FILE* openResultCsvFile(const char* filename);

/**
 * @brief Appends a single result row for Newton's method to the opened CSV file.
 *
 * @param file The FILE pointer returned by openResultCsvFile. Must not be NULL.
 * @param stop_criterion_name String representation of the stopping criterion used.
 * @param x0 The starting point used.
 * @param precision The precision (rho) used for stopping criteria.
 * @param result The RootResult struct containing the outcome of the Newton method run.
 */
void appendNewtonResultToCsv(FILE* file, const char* stop_criterion_name, double x0, double precision, RootResult result);

/**
 * @brief Appends a single result row for the Secant method to the opened CSV file.
 *
 * @param file The FILE pointer returned by openResultCsvFile. Must not be NULL.
 * @param stop_criterion_name String representation of the stopping criterion used.
 * @param x0 The first starting point used.
 * @param x1 The second starting point used.
 * @param precision The precision (rho) used for stopping criteria.
 * @param result The RootResult struct containing the outcome of the Secant method run.
 */
void appendSecantResultToCsv(FILE* file, const char* stop_criterion_name, double x0, double x1, double precision, RootResult result);

/**
 * @brief Generates a Gnuplot script to plot the function f(x) over the interval [a, b].
 *
 * Creates a script that plots f(x) and saves it as a PNG file. Also generates the necessary data file.
 *
 * @param script_filename Base name for the Gnuplot script (e.g., "plot_function.gp").
 *                        Final path: "scripts/<script_filename>".
 * @param plot_filename Base name for the output plot PNG (e.g., "function_plot.png").
 *                      Final path: "plots/<plot_filename>".
 * @param num_points Number of points to use for plotting the function curve.
 */
void generateFunctionPlotScript(const char* script_filename, const char* plot_filename, int num_points);


// Removed declaration for generateIterationPlotScripts.
// Python script (plot_results.py) is used for actual heatmap generation.

#endif // FILEIO_H