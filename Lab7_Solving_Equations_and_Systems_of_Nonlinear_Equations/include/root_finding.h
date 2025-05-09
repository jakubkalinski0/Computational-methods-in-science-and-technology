/**
 * @file root_finding.h
 * @brief Header file for root-finding algorithms (Newton, Secant).
 */
#ifndef ROOT_FINDING_H
#define ROOT_FINDING_H

#include "common.h" // Includes common definitions like MAX_ITERATIONS

/**
 * @brief Enum to specify the stopping criterion for root-finding methods.
 */
typedef enum {
    STOP_ON_X_DIFF,  /**< Stop when |x_next - x_curr| < precision */
    STOP_ON_F_ABS,   /**< Stop when |f(x_next)| < precision */
    STOP_ON_BOTH     /**< Stop when both conditions are met */
} StopCriterionType;

/**
 * @brief Structure to hold the results of a root-finding iteration.
 */
typedef struct {
    double root;         /**< The estimated root found. NAN if not converged or error. */
    int iterations;      /**< Number of iterations performed. */
    double final_error;  /**< The value of the stopping criterion met.
                              For STOP_ON_X_DIFF, it's |x_i+1-x_i|.
                              For STOP_ON_F_ABS, it's |f(xi+1)|.
                              For STOP_ON_BOTH, it's the smaller of the two.
                              NAN if max iterations reached without convergence. */
    int status;          /**< Status code: 0=Success, 1=Max iterations reached, 2=Division by zero / stagnation. */
} RootResult;

/**
 * @brief Finds the root of the function f(x) using Newton's method.
 *
 * Implements the iterative formula: x_{i+1} = x_i - f(x_i) / f'(x_i).
 * Requires the function f(x) and its derivative df(x) (defined in function.c).
 *
 * @param x0 Initial guess for the root.
 * @param precision The desired precision (rho).
 * @param max_iterations Maximum number of iterations allowed.
 * @param criterion The stopping criterion to use (STOP_ON_X_DIFF, STOP_ON_F_ABS, or STOP_ON_BOTH).
 * @return A RootResult struct containing the outcome (root, iterations, error, status).
 *         Status codes:
 *         0: Success, convergence criteria met.
 *         1: Failed to converge within max_iterations.
 *         2: Potential division by zero (f'(x) near zero).
 */
RootResult newtonMethod(double x0, double precision, int max_iterations, StopCriterionType criterion);

/**
 * @brief Finds the root of the function f(x) using the Secant method.
 *
 * Implements the iterative formula:
 * x_{i+2} = x_{i+1} - f(x_{i+1}) * (x_{i+1} - x_i) / (f(x_{i+1}) - f(x_i)).
 * Requires only the function f(x) (defined in function.c).
 *
 * @param x0 First initial point.
 * @param x1 Second initial point.
 * @param precision The desired precision (rho).
 * @param max_iterations Maximum number of iterations allowed.
 * @param criterion The stopping criterion to use (STOP_ON_X_DIFF, STOP_ON_F_ABS, or STOP_ON_BOTH).
 * @return A RootResult struct containing the outcome (root, iterations, error, status).
 *         Status codes:
 *         0: Success, convergence criteria met.
 *         1: Failed to converge within max_iterations.
 *         2: Potential division by zero (f(x_i+1) - f(x_i) near zero, indicating stagnation or parallel secant line).
 */
RootResult secantMethod(double x0, double x1, double precision, int max_iterations, StopCriterionType criterion);

#endif // ROOT_FINDING_H