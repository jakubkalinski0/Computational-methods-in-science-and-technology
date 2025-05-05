/**
 * @file error.c
 * @brief Implementation of error calculation functions for approximation/interpolation analysis.
 *
 * Contains the function to compute the maximum absolute error and mean squared error
 * between two sets of data points (typically true values vs. calculated values).
 */
#include "../include/error.h"
#include <math.h>   // For fabs() and NAN constant
#include <stdio.h>  // For fprintf (warning message)

/**
 * @brief Calculates the maximum absolute error and mean squared error between two datasets.
 *
 * Iterates through the `trueValues` and `approxValues` arrays, comparing element-wise.
 * - Maximum Absolute Error: Finds the largest absolute difference between corresponding elements.
 * - Mean Squared Error (MSE): Calculates the average of the squared differences.
 *
 * @param trueValues Array containing the true function values.
 * @param approxValues Array containing the approximated or interpolated values.
 * @param numPoints The number of points in each array (must be the same for both).
 * @return An ErrorResult struct containing:
 *         - `max_error`: The maximum value of `fabs(trueValues[i] - approxValues[i])`.
 *         - `mean_squared_error`: The value of `(1/numPoints) * sum( (trueValues[i] - approxValues[i])^2 )`.
 *         Returns NAN for both errors if `numPoints <= 0`, along with a warning message to stderr.
 */
ErrorResult calculateError(double trueValues[], double approxValues[], int numPoints) {
    ErrorResult result;
    // Initialize errors; max_error starts at 0, assuming non-negative errors.
    result.max_error = 0.0;
    result.mean_squared_error = 0.0;

    // --- Input Validation ---
    if (numPoints <= 0) {
        // Handle invalid input gracefully by returning Not-a-Number (NAN).
        result.max_error = NAN;
        result.mean_squared_error = NAN;
        // Inform the user about the problematic call.
        fprintf(stderr, "Warning [calculateError]: Called with numPoints = %d. Cannot compute errors. Returning NAN.\n", numPoints);
        return result;
    }

    // Accumulator for the sum of squared errors. Use double for precision.
    double sum_sq_error = 0.0;

    // --- Iterate and Calculate Errors ---
    for (int i = 0; i < numPoints; i++) {
        // Calculate the absolute difference at this point.
        double diff = trueValues[i] - approxValues[i];
        double error = fabs(diff);

        // Update maximum error if the current error is larger.
        if (error > result.max_error) {
            result.max_error = error;
        }

        // Add the squared error to the sum for MSE calculation.
        sum_sq_error += diff * diff; // Use diff*diff instead of error*error to preserve sign if needed (though not here)
                                     // Equivalent to error * error since error is fabs(diff).
    }

    // --- Finalize Mean Squared Error ---
    // Avoid division by zero (already handled by the numPoints <= 0 check, but good practice).
    result.mean_squared_error = sum_sq_error / (double)numPoints;

    return result; // Return the struct containing both calculated errors.
}