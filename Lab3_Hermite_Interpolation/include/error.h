/**
* @file error.h
 * @brief Header file for error calculation functions.
 *
 * Defines the structure for storing error results and the function
 * prototype for calculating interpolation errors.
 */
#ifndef ERROR_H
#define ERROR_H
#include "common.h"

/**
 * @brief Structure to hold the results of error calculations.
 */
typedef struct {
    double max_error;           /**< Maximum absolute error between true and interpolated values. */
    double mean_squared_error;  /**< Mean squared error (MSE) between true and interpolated values. */
} ErrorResult;

/**
 * @brief Calculates the maximum absolute error and mean squared error.
 *
 * Compares the true function values with the interpolated values at a set of points.
 *
 * @param trueValues Array containing the true function values.
 * @param interpValues Array containing the interpolated function values.
 * @param numPoints The number of points at which the comparison is made.
 * @return An ErrorResult struct containing the calculated max_error and mean_squared_error.
 *         Returns NAN for errors if numPoints <= 0.
 */
ErrorResult calculateError(double trueValues[], double interpValues[], int numPoints);
#endif // ERROR_H