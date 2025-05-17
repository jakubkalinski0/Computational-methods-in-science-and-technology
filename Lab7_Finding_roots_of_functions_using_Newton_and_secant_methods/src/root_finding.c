/**
 * @file root_finding.c
 * @brief Implementation of root-finding algorithms (Newton, Secant).
 *        Uses strict equality checks (== 0.0) instead of tolerance.
 */
#include "../include/root_finding.h"
#include "../include/function.h" // Needs f(x) and df(x)
#include <math.h>                // For fabs, NAN, isinf, isnan
#include <stdio.h>               // For debugging printf (optional)
#include <stdbool.h>             // For bool type

/**
 * @brief Finds the root of the function f(x) using Newton's method.
 */
RootResult newtonMethod(double x0, double precision, int max_iterations, StopCriterionType criterion) {
    RootResult result;
    result.iterations = 0;
    result.status = 1; // Default to max iterations reached
    result.root = NAN;
    result.final_error = NAN;

    double x_curr = x0;
    double x_next;
    double fx, dfx;
    double step = 0.0; // Initialize step to a default value

    if (max_iterations == 0) { // Handle edge case: no iterations allowed
        result.root = x_curr;
        // final_error and status are already set for failure
        // iterations is 0
        // Calculate initial f(x_curr) if needed for STOP_ON_F_ABS or STOP_ON_BOTH
        if (criterion == STOP_ON_F_ABS || criterion == STOP_ON_BOTH) {
            result.final_error = fabs(f(x_curr));
        } else {
            result.final_error = NAN; // No x_diff to calculate
        }
        return result;
    }

    for (int i = 0; i < max_iterations; ++i) {
        fx = f(x_curr);
        dfx = df(x_curr);

        if (dfx == 0.0) {
            result.status = 2;
            result.root = x_curr;
            result.iterations = i; // Iterations up to this point
            // final_error for stagnation might be the current f(x_curr)
            result.final_error = fabs(fx);
            return result;
        }

        step = fx / dfx; // 'step' is now guaranteed to be initialized if loop runs

        if (isinf(step) || isnan(step)) {
             result.status = 2;
             result.root = x_curr;
             result.iterations = i; // Iterations up to this point
             result.final_error = NAN; // Error from step calculation
             return result;
        }

        x_next = x_curr - step;
        result.iterations = i + 1;

        double error_diff = fabs(x_next - x_curr);
        double error_abs_f = fabs(f(x_next));
        bool converged = false;

        switch (criterion) {
            case STOP_ON_X_DIFF:
                result.final_error = error_diff; // Always store the current error type being checked
                if (error_diff < precision) {
                    converged = true;
                }
                break;
            case STOP_ON_F_ABS:
                result.final_error = error_abs_f; // Always store the current error type
                if (error_abs_f < precision) {
                    converged = true;
                }
                break;
            case STOP_ON_BOTH:
            default:
                result.final_error = (error_diff < error_abs_f) ? error_diff : error_abs_f; // Store smaller
                if (error_diff < precision && error_abs_f < precision) {
                    converged = true;
                }
                break;
        }

        if (converged) {
            result.status = 0; // Success
            result.root = x_next;
            // final_error is already set based on criterion in the switch
            return result;
        }

        x_curr = x_next;
    }

    // Max iterations reached if loop finishes
    result.root = x_curr; // Return the last computed value
    // result.final_error will hold the error calculated in the *last iteration* of the loop
    // This is correct as it's the "best" error achieved before hitting max_iterations.
    // No need to recalculate here.
    return result;
}

/**
 * @brief Finds the root of the function f(x) using the Secant method.
 */
RootResult secantMethod(double x0, double x1, double precision, int max_iterations, StopCriterionType criterion) {
    RootResult result;
    result.iterations = 0;
    result.status = 1; // Default to max iterations reached
    result.root = NAN;
    result.final_error = NAN;

    double x_prev = x0;
    double x_curr = x1;

    if (max_iterations == 0) {
        result.root = x_curr; // Or x0, depending on definition
        if (criterion == STOP_ON_F_ABS || criterion == STOP_ON_BOTH) {
            result.final_error = fabs(f(x_curr));
        } else if (criterion == STOP_ON_X_DIFF) {
             result.final_error = fabs(x_curr - x_prev);
        } else {
            result.final_error = NAN;
        }
        return result;
    }


    if (x_prev == x_curr) { // Initial points are the same
        result.status = 2; // Stagnation / invalid input
        result.root = x0;
        result.iterations = 0;
        result.final_error = (criterion == STOP_ON_F_ABS || criterion == STOP_ON_BOTH) ? fabs(f(x0)) : 0.0; // 0.0 for x_diff
        return result;
    }

    double fx_prev = f(x_prev);
    double fx_curr = f(x_curr);
    double x_next;
    // For secant, initial error check before first iteration might be useful for final_error if max_iter=0
    // But handled by max_iterations == 0 check above.

    for (int i = 0; i < max_iterations; ++i) {
        double f_diff = fx_curr - fx_prev;

        if (f_diff == 0.0) {
            // Stagnation: f(x_curr) == f(x_prev)
            bool potentially_converged = false;
            double current_x_diff = fabs(x_curr - x_prev); // This is the x_diff from previous step
            double current_f_abs = fabs(fx_curr);          // This is f(current x_curr)

            result.iterations = i; // Iterations up to this point
            result.root = x_curr;

            switch (criterion) {
                case STOP_ON_X_DIFF:
                    result.final_error = current_x_diff;
                    if (current_x_diff < precision) potentially_converged = true;
                    break;
                case STOP_ON_F_ABS:
                    result.final_error = current_f_abs;
                    if (current_f_abs < precision) potentially_converged = true;
                    break;
                case STOP_ON_BOTH:
                default:
                    result.final_error = fmin(current_x_diff, current_f_abs);
                    if (current_x_diff < precision && current_f_abs < precision) potentially_converged = true;
                    break;
            }

            if (potentially_converged) {
                 result.status = 0; // Converged despite f_diff == 0
            } else {
                result.status = 2; // Stagnation without convergence
            }
            return result;
        }

        // x_curr == x_prev is unlikely here if f_diff != 0, but for safety:
        if (x_curr == x_prev) {
             result.status = 2;
             result.root = x_curr;
             result.iterations = i;
             result.final_error = fabs(fx_curr); // Or some other relevant error
             return result;
        }

        double update_term = fx_curr * (x_curr - x_prev) / f_diff;

        if (isinf(update_term) || isnan(update_term)) {
             result.status = 2;
             result.root = x_curr;
             result.iterations = i;
             result.final_error = NAN; // Error from update_term
             return result;
        }

        x_next = x_curr - update_term;
        result.iterations = i + 1;

        double error_diff = fabs(x_next - x_curr);
        double error_abs_f = fabs(f(x_next)); // f at the *new* point
        bool converged = false;

        switch (criterion) {
            case STOP_ON_X_DIFF:
                result.final_error = error_diff;
                if (error_diff < precision) {
                    converged = true;
                }
                break;
            case STOP_ON_F_ABS:
                result.final_error = error_abs_f;
                if (error_abs_f < precision) {
                    converged = true;
                }
                break;
            case STOP_ON_BOTH:
            default:
                result.final_error = (error_diff < error_abs_f) ? error_diff : error_abs_f;
                if (error_diff < precision && error_abs_f < precision) {
                    converged = true;
                }
                break;
        }

        if (converged) {
            result.status = 0; // Success
            result.root = x_next;
            return result;
        }

        // Update points for next iteration
        x_prev = x_curr;
        fx_prev = fx_curr;
        x_curr = x_next;
        fx_curr = f(x_curr); // Calculate f for the *new* x_curr for the next iteration's f_diff
    }

    // Max iterations reached
    result.root = x_curr; // Return the last computed value
    // result.final_error holds the error from the last iteration's check.
    return result;
}