/**
 * @file fileio.h
 * @brief Header file for file input/output operations for spline interpolation.
 * Handles uniform and Chebyshev nodes.
 */
#ifndef FILEIO_H
#define FILEIO_H
#include "common.h" // Includes BoundaryConditionType

// saveDataToFile, saveNodesToFile - bez zmian w sygnaturze

void saveDataToFile(const char* filename, double x[], double y[], int n);
void saveNodesToFile(const char* filename, double nodes[], double values[], int n);

/**
 * @brief Saves spline interpolation errors (max and MSE) to a CSV file.
 * @param filename_base Base name for the CSV file (e.g., "cubic_natural_uniform_errors"). Saved as "data/filename_base.csv".
 * @param maxNodes The maximum number of nodes used (results are for n=2 to maxNodes).
 * @param errors Array containing the maximum absolute errors for each node count (index i -> n=i+2).
 * @param mse Array containing the mean squared errors for each node count (index i -> n=i+2).
 */
void saveSplineErrorsToFile(const char* filename_base, int maxNodes, double errors[], double mse[]);


/**
 * @brief Generates Gnuplot script to plot comparison of maximum spline interpolation errors for different node types.
 * Creates 'scripts/plot_spline_errors.gp'. Output plot 'plots/spline_interpolation_errors.png'.
 *
 * @param maxNodes The maximum number of nodes used (plotting n=2 to maxNodes).
 * @param errors_cubic_natural_uniform Array of max errors for Cubic Natural (Uniform Nodes).
 * @param errors_cubic_clamped_uniform Array of max errors for Cubic Clamped (Uniform Nodes).
 * @param errors_quad_clamped_uniform Array of max errors for Quadratic Clamped (Uniform Nodes).
 * @param errors_quad_zero_start_uniform Array of max errors for Quadratic Zero Start (Uniform Nodes).
 * @param errors_cubic_natural_chebyshev Array of max errors for Cubic Natural (Chebyshev Nodes).
 * @param errors_cubic_clamped_chebyshev Array of max errors for Cubic Clamped (Chebyshev Nodes).
 * @param errors_quad_clamped_chebyshev Array of max errors for Quadratic Clamped (Chebyshev Nodes).
 * @param errors_quad_zero_start_chebyshev Array of max errors for Quadratic Zero Start (Chebyshev Nodes).
 */
void generateSplineErrorPlotScript(int maxNodes,
                                   double errors_cubic_natural_uniform[],
                                   double errors_cubic_clamped_uniform[],
                                   double errors_quad_clamped_uniform[],
                                   double errors_quad_zero_start_uniform[],
                                   double errors_cubic_natural_chebyshev[],
                                   double errors_cubic_clamped_chebyshev[],
                                   double errors_quad_clamped_chebyshev[],
                                   double errors_quad_zero_start_chebyshev[]);


/**
 * @brief Generates Gnuplot script to plot individual spline interpolation results for each node count and type.
 * Creates 'scripts/plot_spline_interpolation.gp'. Generates plots in 'plots/'.
 *
 * @param maxNodes The maximum number of nodes (plots generated for n=2 to maxNodes).
 */
void generateSplineGnuplotScript(int maxNodes);

#endif // FILEIO_H