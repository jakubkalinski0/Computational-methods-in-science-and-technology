/**
 * @file error.h
 * @brief Header file for error calculation functions in approximation analysis.
 *
 * Defines the structure for storing error calculation results (comparing true
 * function values against approximated values) and the function prototype
 * for calculating these errors.
 */
#ifndef ERROR_H
#define ERROR_H
#include "common.h"

/**
 * @brief Structure to hold the results of error calculations between true and approximated values.
 *
 * Stores key metrics used to evaluate the quality of an approximation or interpolation.
 */
typedef struct {
    double max_error;           /**< Maximum absolute error: max|trueValue_i - approxValue_i| over all comparison points. */
    double mean_squared_error;  /**< Mean Squared Error (MSE): (1/N) * Sum[(trueValue_i - approxValue_i)^2] over N comparison points. */
} ErrorResult;

/**
 * @brief Calculates the maximum absolute error and mean squared error between two datasets.
 *
 * Compares the true function values (`trueValues`) with the corresponding
 * approximated values (`approxValues`) at a specified number of points (`numPoints`).
 * This is typically used to quantify the accuracy of a polynomial approximation
 * or other numerical methods against the known true function.
 *
 * @param trueValues Array containing the true function values at the comparison points.
 * @param approxValues Array containing the approximated function values at the comparison points.
 *                     Must be the same size as `trueValues`.
 * @param numPoints The number of comparison points (the size of the `trueValues` and `approxValues` arrays).
 *                  Must be greater than 0 for meaningful results.
 * @return An ErrorResult struct containing the calculated `max_error` and `mean_squared_error`.
 *         If `numPoints <= 0`, returns an ErrorResult with `max_error` and `mean_squared_error` set to NAN
 *         and prints a warning to stderr.
 */
ErrorResult calculateError(double trueValues[], double approxValues[], int numPoints);

#endif // ERROR_H