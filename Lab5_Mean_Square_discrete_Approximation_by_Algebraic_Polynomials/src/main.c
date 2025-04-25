/**
 * @file main.c
 * @brief Main program driver for performing and analyzing least-squares polynomial approximation.
 *
 * This program performs the following steps:
 * 1. Prompts the user for the number of sample points (n) and the maximum polynomial degree (max_m).
 * 2. Validates user input.
 * 3. Generates a dense set of points for plotting the original function f(x) over the interval [a, b].
 * 4. Saves the original function data to 'data/original_function.dat'.
 * 5. Generates 'n' uniformly distributed sample points (x_i, y_i = f(x_i)) within [a, b].
 * 6. Saves the sample points to 'data/sample_points.dat'.
 * 7. Iterates through polynomial degrees m from 0 to max_m:
 *    a. Calculates the coefficients of the least-squares polynomial P_m(x) using `leastSquaresApprox`.
 *    b. If successful, evaluates P_m(x) on the dense plotting grid using `evaluatePolynomial`.
 *    c. Saves the approximated polynomial data to 'data/approximation_degree{m}_points{n}.dat'.
 *    d. Calculates the maximum absolute error and MSE between f(x) and P_m(x) using `calculateError`.
 *    e. Stores the calculated errors.
 * 8. Saves the collected errors (max absolute and MSE vs. degree m) to 'data/approximation_errors_vs_degree.csv'.
 * 9. Generates Gnuplot scripts ('scripts/plot_approximations.gp' and 'scripts/plot_approx_errors.gp')
 *    for visualizing the results.
 * 10. Prints instructions on how to generate the plots using Gnuplot or the Makefile.
 */
#include "../include/common.h"        // Common definitions (constants like a, b, k, m, PI, MAX_NODES)
#include "../include/function.h"      // Function f(x) and its derivative df(x)
#include "../include/nodes.h"         // Node generation functions (uniformNodes used)
#include "../include/approximation.h" // Least squares approximation logic (leastSquaresApprox, evaluatePolynomial)
#include "../include/error.h"         // Error calculation (calculateError)
#include "../include/fileio.h"        // File I/O (saving data, generating Gnuplot scripts)
#include <stdio.h>                   // Standard Input/Output (printf, scanf, fprintf)
#include <stdlib.h>                  // Standard Library (exit, malloc/free implicitly used elsewhere)
#include <math.h>                    // For isnan() to check error results

// --- Configuration Constants ---
/**
 * @brief Maximum recommended polynomial degree for approximation.
 * Higher degrees can lead to ill-conditioned Gram matrices and numerical instability
 * in the least-squares calculation, especially with uniformly spaced points.
 */
#define MAX_POLY_DEGREE 50
/**
 * @brief Number of points used for plotting smooth curves (original function and approximations).
 */
#define NUM_PLOT_POINTS 1000

/**
 * @brief Main entry point for the least-squares polynomial approximation program.
 *
 * Orchestrates the entire process of getting user input, generating data,
 * performing approximations for various degrees, calculating errors, saving results,
 * and generating plotting scripts.
 *
 * @return 0 on successful completion.
 * @return 1 if there is an error during input processing or initial setup.
 *         Other functions called (like leastSquaresApprox) may return non-zero
 *         status codes which are handled within the main loop, but the program
 *         attempts to continue processing lower degrees if possible.
 */
int main() {
    // --- Variable Declarations ---
    int numSamplePoints; // n: Number of discrete sample points (x_i, y_i) for approximation.
    int maxDegree;       // max_m: Maximum degree of the approximating polynomial to test.

    // --- Get User Input ---
    printf("--- Least Squares Polynomial Approximation ---\n");
    printf("Enter the number of sample points (n > 0, e.g., 50): ");
    if (scanf("%d", &numSamplePoints) != 1 || numSamplePoints <= 0) {
        fprintf(stderr, "Error: Invalid number of sample points entered.\n");
        return 1; // Exit on invalid input
    }
    // Check against the hardcoded array limit from common.h if necessary
    if (numSamplePoints > MAX_NODES) {
         fprintf(stderr, "Error: Number of sample points (%d) exceeds MAX_NODES (%d).\n", numSamplePoints, MAX_NODES);
         return 1;
    }

    printf("Enter the maximum degree of the approximating polynomial (m >= 0, m < n, recommended m <= %d): ", MAX_POLY_DEGREE);
     if (scanf("%d", &maxDegree) != 1 || maxDegree < 0 ) {
        fprintf(stderr, "Error: Invalid maximum polynomial degree entered.\n");
        return 1; // Exit on invalid input
    }
    // Crucial condition for least squares: need more points than coefficients (n > m implies n >= m+1)
    if (maxDegree >= numSamplePoints) {
         fprintf(stderr, "Error: Maximum degree (%d) must be strictly less than the number of sample points (%d).\n", maxDegree, numSamplePoints);
         return 1; // Exit if condition not met
    }
    // Warn user about potential numerical issues with high degrees
    if (maxDegree > MAX_POLY_DEGREE) {
        printf("Warning: Maximum degree %d exceeds recommended limit %d. Numerical instability may occur.\n", maxDegree, MAX_POLY_DEGREE);
    }

    // --- Generate Dense Grid for Plotting Original Function ---
    double x_plot[NUM_PLOT_POINTS];        // x-coordinates for smooth plotting
    double y_true_plot[NUM_PLOT_POINTS];   // f(x) values at x_plot points

    double plot_step = (b - a) / (NUM_PLOT_POINTS - 1.0); // Step size for plotting grid
    printf("\nGenerating data for original function f(x) on [%.2f, %.2f]...\n", a, b);
    for (int i = 0; i < NUM_PLOT_POINTS; i++) {
        x_plot[i] = a + i * plot_step;
        y_true_plot[i] = f(x_plot[i]); // Calculate true function value
    }
    // Ensure the last point is exactly 'b' to avoid floating-point drift issues
    if (NUM_PLOT_POINTS > 1) x_plot[NUM_PLOT_POINTS - 1] = b;

    // Save the generated original function data to a file for Gnuplot
    saveDataToFile("original_function.dat", x_plot, y_true_plot, NUM_PLOT_POINTS);
    printf("Saved original function data to data/original_function.dat\n");

    // --- Generate Sample Points (Uniform Distribution) ---
    // Using arrays sized with MAX_NODES for simplicity, actual size used is numSamplePoints.
    double sample_x[MAX_NODES]; // x-coordinates of sample points
    double sample_y[MAX_NODES]; // y-coordinates (f(x_i)) of sample points
    printf("Generating n = %d uniformly distributed sample points...\n", numSamplePoints);
    uniformNodes(sample_x, numSamplePoints); // Generate uniform x_i in [a, b]
    for (int i = 0; i < numSamplePoints; i++) {
        sample_y[i] = f(sample_x[i]);       // Calculate y_i = f(x_i)
    }
    // Save the sample points to a file for Gnuplot and verification
    saveNodesToFile("sample_points.dat", sample_x, sample_y, numSamplePoints); // Reusing saveNodesToFile for sample points
    printf("Saved sample points to data/sample_points.dat\n");

    // --- Initialize Arrays for Storing Results ---
    // Size maxDegree + 1 to store results for degrees 0 through maxDegree.
    // Use MAX_POLY_DEGREE+1 for safety, though only maxDegree+1 entries will be filled.
    double approx_max_errors[MAX_POLY_DEGREE + 1]; // Stores max absolute error for each degree m
    double approx_mse_errors[MAX_POLY_DEGREE + 1]; // Stores MSE for each degree m
    double coefficients[MAX_POLY_DEGREE + 1];      // Buffer to hold coefficients for the current degree m
    double y_approx_plot[NUM_PLOT_POINTS];         // Buffer to hold evaluated polynomial P_m(x) values for plotting

    // --- Output Header Information ---
    printf("\nStarting Approximation Analysis for f(x) = sin(%.1fx/pi) * exp(-%.1fx/pi)\n", k, m);
    printf("Interval: [%.2f, %.2f]\n", a, b);
    printf("Number of sample points (n): %d\n", numSamplePoints);
    printf("Testing polynomial degrees (m) from 0 to %d\n", maxDegree);
    printf("=========================================================================\n");

    // --- Main Loop: Iterate Through Polynomial Degrees m = 0 to maxDegree ---
    for (int m_deg = 0; m_deg <= maxDegree; m_deg++) {
        printf("\n--- Processing Degree m = %d ---\n", m_deg);
        char filename_approx[100]; // Buffer for approximation data filename

        // 1. Calculate Least Squares Coefficients for degree m_deg
        int status = leastSquaresApprox(sample_x, sample_y, numSamplePoints, m_deg, coefficients);

        // 2. Check if coefficient calculation was successful
        if (status != 0) {
            printf("  Failed to compute coefficients for degree m = %d. Skipping this degree.\n", m_deg);
            // Mark errors as NaN for this degree to indicate failure in the results file/plot
            approx_max_errors[m_deg] = NAN;
            approx_mse_errors[m_deg] = NAN;
            continue; // Skip to the next degree
        } else {
             printf("  Successfully computed coefficients for degree m = %d.\n", m_deg);
             // Optional: Print coefficients if needed for debugging
             // printf("  Coefficients (a0..a%d): ", m_deg);
             // for(int c=0; c<=m_deg; ++c) printf("%.3e ", coefficients[c]);
             // printf("\n");
        }


        // 3. Evaluate the Approximating Polynomial P_m(x) on the dense plotting grid
        for (int j = 0; j < NUM_PLOT_POINTS; j++) {
            y_approx_plot[j] = evaluatePolynomial(x_plot[j], coefficients, m_deg);
        }

        // 4. Save the calculated polynomial curve data to a file
        snprintf(filename_approx, sizeof(filename_approx), "approximation_degree%d_points%d.dat", m_deg, numSamplePoints);
        saveDataToFile(filename_approx, x_plot, y_approx_plot, NUM_PLOT_POINTS);
        printf("  Saved approximation curve data to data/%s\n", filename_approx);

        // 5. Calculate and Store Errors (Max Absolute and MSE)
        ErrorResult errors = calculateError(y_true_plot, y_approx_plot, NUM_PLOT_POINTS);

        // Check if error calculation returned valid numbers
        if (isnan(errors.max_error) || isnan(errors.mean_squared_error)) {
             printf("  Warning: Error calculation resulted in NaN for degree m = %d.\n", m_deg);
             approx_max_errors[m_deg] = NAN; // Store NaN
             approx_mse_errors[m_deg] = NAN;
        } else {
            approx_max_errors[m_deg] = errors.max_error;
            approx_mse_errors[m_deg] = errors.mean_squared_error;
            // Print errors for the current degree
            printf("  Max Absolute Error: %.5e\n", approx_max_errors[m_deg]);
            printf("  Mean Squared Error (MSE): %.5e\n", approx_mse_errors[m_deg]);
        }
    } // --- End of loop over degrees m ---

    printf("=========================================================================\n");

    // --- Save Accumulated Error Data to CSV File ---
    const char *error_filename = "data/approximation_errors_vs_degree.csv";
    printf("\nSaving approximation errors vs. degree to %s...\n", error_filename);
    if (saveApproximationErrorsToFile(error_filename, maxDegree, approx_max_errors, approx_mse_errors) != 0) {
        // Error message already printed by saveApproximationErrorsToFile if it failed
        fprintf(stderr, "Warning: Failed to save error data to file %s. Error plot may not work.\n", error_filename);
        // Continue execution if possible, but plotting might fail.
    } else {
        printf("Successfully saved error data.\n");
    }

    // --- Generate Gnuplot Scripts ---
    printf("\nGenerating Gnuplot scripts...\n");
    // Script 1: Individual plots for each degree m
    generateApproxGnuplotScript(maxDegree, numSamplePoints);
    // Script 2: Summary plot of errors vs. degree m
    generateApproxErrorPlotScript(maxDegree);

    // --- Final Instructions for User ---
    printf("\n=========================================================================\n");
    printf("Analysis complete.\n");
    printf("Data files saved in: data/\n");
    printf("Gnuplot scripts saved in: scripts/\n");
    printf("\nTo generate plots:\n");
    printf("1. Ensure Gnuplot is installed.\n");
    printf("2. Open a terminal in the project's root directory.\n");
    printf("3. Run the scripts:\n");
    printf("   gnuplot scripts/plot_approximations.gp\n");
    printf("   gnuplot scripts/plot_approx_errors.gp\n");
    printf("4. Alternatively, use the Makefile target:\n");
    printf("   make plots\n");
    printf("Generated plots (.png) will be saved in: plots/\n");
    printf("=========================================================================\n");

    return 0; // Indicate successful program execution
}