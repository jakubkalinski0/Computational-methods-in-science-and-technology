#include "../include/common.h"
#include "../include/function.h"
#include "../include/nodes.h"
#include "../include/interpolation.h"
#include "../include/error.h"
#include "../include/fileio.h"
#include <stdio.h>
#include <string.h> // For sprintf

int main() {
    int maxNodes;

    printf("Enter the maximum number of interpolation nodes (2-%d): ", MAX_NODES);
    if (scanf("%d", &maxNodes) != 1) {
        fprintf(stderr, "Error reading the number of nodes.\n");
        return 1;
    }
    if (maxNodes < 2 || maxNodes > MAX_NODES) {
        fprintf(stderr, "Invalid number of nodes. Must be between 2 and %d\n", MAX_NODES);
        return 1;
    }

    const int numPoints = 1000;
    double x_plot[numPoints];
    double y_true[numPoints];

    double plot_step = (b - a) / (numPoints - 1.0);
    for (int i = 0; i < numPoints; i++) {
        x_plot[i] = a + i * plot_step;
        y_true[i] = f(x_plot[i]);
    }
    if (numPoints > 1) x_plot[numPoints - 1] = b;
    saveDataToFile("original_function.dat", x_plot, y_true, numPoints);

    double true_deriv_a = df(a);
    double true_deriv_b = df(b);
    printf("\nTrue function derivative at a=%.4f: f'(a) = %.6f\n", a, true_deriv_a);
    printf("True function derivative at b=%.4f: f'(b) = %.6f\n", b, true_deriv_b);

    // --- Arrays to Store Error Results (8 sets) ---
    int error_array_size = maxNodes - 1;
    // Uniform Nodes Errors
    double errors_cubic_natural_uniform[error_array_size];
    double errors_cubic_clamped_uniform[error_array_size];
    double errors_quad_clamped_uniform[error_array_size];
    double errors_quad_zero_start_uniform[error_array_size];
    double mse_cubic_natural_uniform[error_array_size];
    double mse_cubic_clamped_uniform[error_array_size];
    double mse_quad_clamped_uniform[error_array_size];
    double mse_quad_zero_start_uniform[error_array_size];
    // Chebyshev Nodes Errors
    double errors_cubic_natural_chebyshev[error_array_size];
    double errors_cubic_clamped_chebyshev[error_array_size];
    double errors_quad_clamped_chebyshev[error_array_size];
    double errors_quad_zero_start_chebyshev[error_array_size];
    double mse_cubic_natural_chebyshev[error_array_size];
    double mse_cubic_clamped_chebyshev[error_array_size];
    double mse_quad_clamped_chebyshev[error_array_size];
    double mse_quad_zero_start_chebyshev[error_array_size];


    // --- Temporary Arrays ---
    double nodes[MAX_NODES];
    double values[MAX_NODES];
    double y_interp[numPoints];

    printf("\nStarting Spline Interpolation Analysis for n = 2 to %d nodes...\n", maxNodes);
    printf("Using both Uniform and Chebyshev nodes.\n");
    printf("=========================================================================\n");

    // --- Main Loop over n ---
    for (int n = 2; n <= maxNodes; n++) {
        char filename[150]; // Increased buffer size for longer filenames
        int error_index = n - 2;

        printf("\n--- Processing for n = %d nodes ---\n", n);

        // ========================================
        // === Part 1: Uniformly Spaced Nodes ===
        // ========================================
        printf("--> Uniform Nodes <--\n");

        uniformNodes(nodes, n);
        for (int j = 0; j < n; j++) values[j] = f(nodes[j]);
        sprintf(filename, "uniform_nodes_n%d.dat", n);
        saveNodesToFile(filename, nodes, values, n);

        // --- 1.1 Cubic Spline - Natural (Uniform) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = cubicSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_NATURAL, 0, 0);
        sprintf(filename, "cubic_natural_uniform_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_cn_u = calculateError(y_true, y_interp, numPoints);
        errors_cubic_natural_uniform[error_index] = err_cn_u.max_error;
        mse_cubic_natural_uniform[error_index] = err_cn_u.mean_squared_error;
        printf("  Cubic Natural (Uniform):    MaxErr=%.3e, MSE=%.3e\n", err_cn_u.max_error, err_cn_u.mean_squared_error);

        // --- 1.2 Cubic Spline - Clamped (Uniform) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = cubicSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_CLAMPED, true_deriv_a, true_deriv_b);
        sprintf(filename, "cubic_clamped_uniform_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_cc_u = calculateError(y_true, y_interp, numPoints);
        errors_cubic_clamped_uniform[error_index] = err_cc_u.max_error;
        mse_cubic_clamped_uniform[error_index] = err_cc_u.mean_squared_error;
        printf("  Cubic Clamped (Uniform):    MaxErr=%.3e, MSE=%.3e\n", err_cc_u.max_error, err_cc_u.mean_squared_error);

        // --- 1.3 Quadratic Spline - Clamped Start (Uniform) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = quadraticSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_CLAMPED, true_deriv_a);
        sprintf(filename, "quadratic_clamped_uniform_n%d.dat", n); // Renamed file
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_qc_u = calculateError(y_true, y_interp, numPoints);
        errors_quad_clamped_uniform[error_index] = err_qc_u.max_error;
        mse_quad_clamped_uniform[error_index] = err_qc_u.mean_squared_error;
        printf("  Quadratic Clamped (Uniform): MaxErr=%.3e, MSE=%.3e\n", err_qc_u.max_error, err_qc_u.mean_squared_error);

        // --- 1.4 Quadratic Spline - Zero Slope Start (Uniform) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = quadraticSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_ZERO_SLOPE_START, 0);
        sprintf(filename, "quadratic_zero_start_uniform_n%d.dat", n); // Renamed file
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_qz_u = calculateError(y_true, y_interp, numPoints);
        errors_quad_zero_start_uniform[error_index] = err_qz_u.max_error;
        mse_quad_zero_start_uniform[error_index] = err_qz_u.mean_squared_error;
        printf("  Quadratic Zero St (Uniform): MaxErr=%.3e, MSE=%.3e\n", err_qz_u.max_error, err_qz_u.mean_squared_error);

        // =====================================
        // === Part 2: Chebyshev Nodes       ===
        // =====================================
        printf("--> Chebyshev Nodes <--\n");

        chebyshevNodes(nodes, n); // Generate Chebyshev nodes
        for (int j = 0; j < n; j++) values[j] = f(nodes[j]);
        sprintf(filename, "chebyshev_nodes_n%d.dat", n);
        saveNodesToFile(filename, nodes, values, n);

        // --- 2.1 Cubic Spline - Natural (Chebyshev) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = cubicSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_NATURAL, 0, 0);
        sprintf(filename, "cubic_natural_chebyshev_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_cn_c = calculateError(y_true, y_interp, numPoints);
        errors_cubic_natural_chebyshev[error_index] = err_cn_c.max_error;
        mse_cubic_natural_chebyshev[error_index] = err_cn_c.mean_squared_error;
        printf("  Cubic Natural (Chebyshev):  MaxErr=%.3e, MSE=%.3e\n", err_cn_c.max_error, err_cn_c.mean_squared_error);

        // --- 2.2 Cubic Spline - Clamped (Chebyshev) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = cubicSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_CLAMPED, true_deriv_a, true_deriv_b);
        sprintf(filename, "cubic_clamped_chebyshev_n%d.dat", n);
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_cc_c = calculateError(y_true, y_interp, numPoints);
        errors_cubic_clamped_chebyshev[error_index] = err_cc_c.max_error;
        mse_cubic_clamped_chebyshev[error_index] = err_cc_c.mean_squared_error;
        printf("  Cubic Clamped (Chebyshev):  MaxErr=%.3e, MSE=%.3e\n", err_cc_c.max_error, err_cc_c.mean_squared_error);

        // --- 2.3 Quadratic Spline - Clamped Start (Chebyshev) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = quadraticSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_CLAMPED, true_deriv_a);
        sprintf(filename, "quadratic_clamped_chebyshev_n%d.dat", n); // Renamed file
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_qc_c = calculateError(y_true, y_interp, numPoints);
        errors_quad_clamped_chebyshev[error_index] = err_qc_c.max_error;
        mse_quad_clamped_chebyshev[error_index] = err_qc_c.mean_squared_error;
        printf("  Quadratic Clamped (Chebyshev):MaxErr=%.3e, MSE=%.3e\n", err_qc_c.max_error, err_qc_c.mean_squared_error);

        // --- 2.4 Quadratic Spline - Zero Slope Start (Chebyshev) ---
        for (int j = 0; j < numPoints; j++) y_interp[j] = quadraticSplineInterpolation(x_plot[j], nodes, values, n, BOUNDARY_ZERO_SLOPE_START, 0);
        sprintf(filename, "quadratic_zero_start_chebyshev_n%d.dat", n); // Renamed file
        saveDataToFile(filename, x_plot, y_interp, numPoints);
        ErrorResult err_qz_c = calculateError(y_true, y_interp, numPoints);
        errors_quad_zero_start_chebyshev[error_index] = err_qz_c.max_error;
        mse_quad_zero_start_chebyshev[error_index] = err_qz_c.mean_squared_error;
        printf("  Quadratic Zero St (Chebyshev):MaxErr=%.3e, MSE=%.3e\n", err_qz_c.max_error, err_qz_c.mean_squared_error);

    } // End of loop over n

    printf("\n=========================================================================\n");
    printf("Calculations complete. Saving error summaries...\n");

    // Save error data to CSV files (8 files)
    saveSplineErrorsToFile("cubic_natural_uniform_errors", maxNodes, errors_cubic_natural_uniform, mse_cubic_natural_uniform);
    saveSplineErrorsToFile("cubic_clamped_uniform_errors", maxNodes, errors_cubic_clamped_uniform, mse_cubic_clamped_uniform);
    saveSplineErrorsToFile("quadratic_clamped_uniform_errors", maxNodes, errors_quad_clamped_uniform, mse_quad_clamped_uniform);
    saveSplineErrorsToFile("quadratic_zero_start_uniform_errors", maxNodes, errors_quad_zero_start_uniform, mse_quad_zero_start_uniform);
    saveSplineErrorsToFile("cubic_natural_chebyshev_errors", maxNodes, errors_cubic_natural_chebyshev, mse_cubic_natural_chebyshev);
    saveSplineErrorsToFile("cubic_clamped_chebyshev_errors", maxNodes, errors_cubic_clamped_chebyshev, mse_cubic_clamped_chebyshev);
    saveSplineErrorsToFile("quadratic_clamped_chebyshev_errors", maxNodes, errors_quad_clamped_chebyshev, mse_quad_clamped_chebyshev);
    saveSplineErrorsToFile("quadratic_zero_start_chebyshev_errors", maxNodes, errors_quad_zero_start_chebyshev, mse_quad_zero_start_chebyshev);


    printf("Generating Gnuplot scripts...\n");
    generateSplineGnuplotScript(maxNodes); // Generates individual plots for both node types
    generateSplineErrorPlotScript(maxNodes, // Generates combined error plot
                                  errors_cubic_natural_uniform, errors_cubic_clamped_uniform,
                                  errors_quad_clamped_uniform, errors_quad_zero_start_uniform,
                                  errors_cubic_natural_chebyshev, errors_cubic_clamped_chebyshev,
                                  errors_quad_clamped_chebyshev, errors_quad_zero_start_chebyshev);

    // --- Final Output and Instructions ---
    printf("\n=========================================================================\n");
    printf("Analysis complete.\n");
    printf("Data files saved in: data/\n");
    printf("Gnuplot scripts saved in: scripts/\n");
    printf("CSV error summaries saved in: data/\n");
    printf("\nTo generate the plots, navigate to the project root directory and run:\n");
    printf("  gnuplot scripts/plot_spline_interpolation.gp\n");
    printf("  gnuplot scripts/plot_spline_errors.gp\n");
    printf("Alternatively, use the Makefile target:\n");
    printf("  make plots\n");
    printf("Generated plots (.png files) will be saved in: plots/\n");


    return 0;
}