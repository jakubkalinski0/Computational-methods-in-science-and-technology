/**
 * @file fileio.h
 * @brief Header file for file input/output operations.
 *
 * Defines function prototypes for saving data (function points, nodes, errors)
 * to files and generating Gnuplot scripts for visualization.
 */
#ifndef FILEIO_H
#define FILEIO_H
#include "common.h"

/**
 * @brief Saves a set of (x, y) data points to a file.
 *
 * Used for saving the original function curve or interpolated curves.
 * Files are saved in the 'data/' subdirectory.
 *
 * @param filename The name of the file to save (e.g., "original_function.dat").
 * @param x Array of x-coordinates.
 * @param y Array of y-coordinates.
 * @param n The number of data points.
 */
void saveDataToFile(const char* filename, double x[], double y[], int n);

/**
 * @brief Saves the interpolation nodes (x, y) to a file.
 *
 * Files are saved in the 'data/' subdirectory.
 *
 * @param filename The name of the file to save (e.g., "uniform_nodes_n5.dat").
 * @param nodes Array of node x-coordinates.
 * @param values Array of node y-coordinates (function values at nodes).
 * @param n The number of nodes.
 */
void saveNodesToFile(const char* filename, double nodes[], double values[], int n);

/**
 * @brief Generates a Gnuplot script to plot the comparison of maximum interpolation errors.
 *
 * Creates a script ('scripts/plot_errors.gp') that plots the maximum error
 * versus the number of nodes for different interpolation methods and node types.
 * The plot is saved as 'plots/interpolation_errors.png'.
 *
 * @param maxNodes The maximum number of nodes used in the analysis.
 * @param lagrange_uniform_errors Array of max errors for Lagrange with uniform nodes.
 * @param lagrange_chebyshev_errors Array of max errors for Lagrange with Chebyshev nodes.
 * @param newton_uniform_errors Array of max errors for Newton with uniform nodes.
 * @param newton_chebyshev_errors Array of max errors for Newton with Chebyshev nodes.
 */
void generateErrorPlotScript(int maxNodes,
                           double lagrange_uniform_errors[],
                           double lagrange_chebyshev_errors[],
                           double newton_uniform_errors[],
                           double newton_chebyshev_errors[]);

/**
 * @brief Saves Lagrange/Uniform interpolation errors (max and MSE) to a CSV file.
 * @param maxNodes The maximum number of nodes used.
 * @param errors Array containing the maximum absolute errors for each node count.
 * @param mse Array containing the mean squared errors for each node count.
 */
void saveLagrangeUniformErrorsToFile(int maxNodes, double errors[], double mse[]);

/**
 * @brief Saves Lagrange/Chebyshev interpolation errors (max and MSE) to a CSV file.
 * @param maxNodes The maximum number of nodes used.
 * @param errors Array containing the maximum absolute errors for each node count.
 * @param mse Array containing the mean squared errors for each node count.
 */
void saveLagrangeChebyshevErrorsToFile(int maxNodes, double errors[], double mse[]);

/**
 * @brief Saves Newton/Uniform interpolation errors (max and MSE) to a CSV file.
 * @param maxNodes The maximum number of nodes used.
 * @param errors Array containing the maximum absolute errors for each node count.
 * @param mse Array containing the mean squared errors for each node count.
 */
void saveNewtonUniformErrorsToFile(int maxNodes, double errors[], double mse[]);

/**
 * @brief Saves Newton/Chebyshev interpolation errors (max and MSE) to a CSV file.
 * @param maxNodes The maximum number of nodes used.
 * @param errors Array containing the maximum absolute errors for each node count.
 * @param mse Array containing the mean squared errors for each node count.
 */
void saveNewtonChebyshevErrorsToFile(int maxNodes, double errors[], double mse[]);

/**
 * @brief Generates a Gnuplot script to plot individual interpolation results for each node count.
 *
 * Creates a script ('scripts/plot_interpolation.gp') that generates separate plots
 * for each combination of method (Lagrange/Newton), node type (Uniform/Chebyshev),
 * and node count (n=1 to maxNodes). Each plot shows the original function,
 * the interpolated function, and the interpolation nodes.
 * Plots are saved in the 'plots/' subdirectory (e.g., 'plots/lagrange_uniform_with_nodes_n5.png').
 *
 * @param maxNodes The maximum number of nodes for which plots should be generated.
 */
void generateGnuplotScript(int maxNodes);
#endif // FILEIO_H