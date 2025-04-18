/**
 * @file error.c
 * @brief Implementation of error calculation functions.
 */
#include "../include/error.h"
#include <math.h> // Include math.h specifically for fabs and potentially NAN/INFINITY
#include <stdio.h> // For fprintf

/**
 * @brief Calculates the maximum absolute error and mean squared error.
 *
 * Iterates through the provided arrays of true and interpolated values,
 * calculating the absolute difference at each point to find the maximum,
 * and summing the squared differences to calculate the mean squared error.
 *
 * @param trueValues Array containing the true function values.
 * @param interpValues Array containing the interpolated function values.
 * @param numPoints The number of points at which the comparison is made.
 * @return An ErrorResult struct containing the calculated max_error and mean_squared_error.
 *         Returns NAN for errors if numPoints <= 0.
 */
ErrorResult calculateError(double trueValues[], double interpValues[], int numPoints) {
    ErrorResult result;
    result.max_error = 0.0;
    result.mean_squared_error = 0.0;

    // Basic input validation
    if (numPoints <= 0) {
        // Handle invalid input: return Not-a-Number
        result.max_error = NAN;
        result.mean_squared_error = NAN;
        fprintf(stderr, "Warning: calculateError called with numPoints = %d\n", numPoints);
        return result;
    }
    if (trueValues == NULL || interpValues == NULL) {
         fprintf(stderr, "Warning: calculateError called with NULL array pointer.\n");
         result.max_error = NAN;
         result.mean_squared_error = NAN;
         return result;
    }


    double sum_sq_error = 0.0; // Use double for sum to avoid potential overflow with large errors/numPoints
    int nan_count = 0;
    // Iterate through all points to compare values
    for (int i = 0; i < numPoints; i++) {
        // Check for NaN in interpolation results
        if (isnan(interpValues[i])) {
            // Option 1: Treat NaN as infinite error
            // result.max_error = INFINITY;
            // sum_sq_error = INFINITY;
            // nan_count++;
            // break; // Stop calculation if NaN is found

            // Option 2: Skip NaN points and report
            nan_count++;
            continue; // Skip this point
        }

        // Calculate absolute error at this point
        double error = fabs(trueValues[i] - interpValues[i]);

        // Update maximum error if current error is larger
        if (error > result.max_error) {
            result.max_error = error;
        }
        // Add squared error to the sum for MSE calculation
        sum_sq_error += error * error;
    }

    // Handle case where all points were NaN or numPoints was adjusted
    int validPoints = numPoints - nan_count;
    if (validPoints <= 0) {
        fprintf(stderr, "Warning: calculateError found %d NaN values out of %d points. Returning NaN/Inf.\n", nan_count, numPoints);
        // If we stopped early due to NaN (Option 1), max_error/sum_sq_error might be INFINITY
        // If we skipped (Option 2), we set to NAN if no valid points remain.
        if (result.max_error != INFINITY) result.max_error = NAN;
         result.mean_squared_error = NAN;
    } else {
        // Calculate Mean Squared Error based on valid points
        result.mean_squared_error = sum_sq_error / validPoints;
        if (nan_count > 0) {
             fprintf(stderr, "Warning: calculateError skipped %d NaN values during calculation.\n", nan_count);
        }
    }


    return result;
}