/**
 * @file error.c
 * @brief Implementation of error calculation functions.
 */
#include "../include/error.h"
#include <math.h> // Include math.h specifically for fabs and potentially NAN/INFINITY

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


    double sum_sq_error = 0.0; // Use double for sum to avoid potential overflow with large errors/numPoints
    // Iterate through all points to compare values
    for (int i = 0; i < numPoints; i++) {
        // Calculate absolute error at this point
        double error = fabs(trueValues[i] - interpValues[i]);
        // Update maximum error if current error is larger
        if (error > result.max_error) {
            result.max_error = error;
        }
        // Add squared error to the sum for MSE calculation
        sum_sq_error += error * error;
    }

    // Calculate Mean Squared Error
    result.mean_squared_error = sum_sq_error / numPoints;
    return result;
}