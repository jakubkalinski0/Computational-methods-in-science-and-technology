/**
 * @file root_finding.c
 * @brief Implementation of root-finding algorithms (Newton, Secant).
 *        Uses strict equality checks (== 0.0) instead of tolerance.
 */
#include "../include/root_finding.h"
#include "../include/function.h" // Needs f(x) and df(x)
#include <math.h>                // For fabs, NAN, isinf, isnan
#include <stdio.h>               // For debugging printf (optional)

/**
 * @brief Finds the root of the function f(x) using Newton's method.
 */
RootResult newtonMethod(double x0, double precision, int max_iterations) {
    RootResult result;
    result.iterations = 0;
    result.status = 1; // Default to max iterations reached
    result.root = NAN;
    result.final_error = NAN;

    double x_curr = x0;
    double x_next;
    double fx, dfx, step;

    for (int i = 0; i < max_iterations; ++i) {
        fx = f(x_curr);
        dfx = df(x_curr);

        // Check for potential division by EXACT zero
        if (dfx == 0.0) {
            // fprintf(stderr, "Warning [Newton]: Derivative is exactly zero at x = %g (iteration %d).\n", x_curr, i);
            result.status = 2; // Division by zero / stagnation
            result.root = x_curr; // Return the last valid point
            result.iterations = i;
            return result;
        }

        step = fx / dfx;

        // Check for potential issues after division (Inf/NaN)
        if (isinf(step) || isnan(step)) {
             // fprintf(stderr, "Warning [Newton]: Step became Inf/NaN at x = %g (iteration %d).\n", x_curr, i);
             result.status = 2; // Treat as error/stagnation
             result.root = x_curr;
             result.iterations = i;
             return result;
        }

        x_next = x_curr - step;

        result.iterations = i + 1; // Count this iteration

        // Calculate errors for convergence check
        double error_diff = fabs(x_next - x_curr);
        double error_abs_f = fabs(f(x_next)); // Check f at the *new* point

        // Store the smaller error value as the final_error representative
        result.final_error = (error_diff < error_abs_f) ? error_diff : error_abs_f;

        // Check convergence: BOTH conditions must be met
        if (error_diff < precision && error_abs_f < precision) {
            result.status = 0; // Success
            result.root = x_next;
            return result;
        }

        // Check for stagnation (very small change, might be near non-simple root or limit)
        // Note: This check might be less necessary without tolerance, but can prevent
        // infinite loops if precision is extremely small and convergence is slow.
        // It's somewhat redundant with the max_iterations check.
        // if (error_diff < 1e-16) { // Example: using machine epsilon range check
        //     result.status = 2; // Indicate possible stagnation/limit
        //     result.root = x_next;
        //     return result;
        // }


        x_curr = x_next; // Prepare for next iteration
    }

    // If loop finishes, max iterations were reached without convergence
    result.root = x_curr; // Return the last computed value
    return result;
}

/**
 * @brief Finds the root of the function f(x) using the Secant method.
 */
RootResult secantMethod(double x0, double x1, double precision, int max_iterations) {
    RootResult result;
    result.iterations = 0;
    result.status = 1; // Default to max iterations reached
    result.root = NAN;
    result.final_error = NAN;

    double x_prev = x0;
    double x_curr = x1;

    // Avoid immediate failure if x0 == x1 is passed initially
    if (x_prev == x_curr) {
        // fprintf(stderr, "Warning [Secant]: Initial points x0 and x1 are identical (%g).\n", x0);
        result.status = 2; // Indicate invalid input / stagnation
        result.root = x0;
        result.iterations = 0;
        return result;
    }

    double fx_prev = f(x_prev);
    double fx_curr = f(x_curr);
    double x_next;


    for (int i = 0; i < max_iterations; ++i) {
        double f_diff = fx_curr - fx_prev;

        // Check for potential division by EXACT zero
        if (f_diff == 0.0) {
            // fprintf(stderr, "Warning [Secant]: f(x_curr) - f(x_prev) is exactly zero (stagnation?) at iteration %d.\n", i);
             // Check if we actually converged (both f(x_curr) and difference are small)
            if (fabs(fx_curr) < precision && fabs(x_curr - x_prev) < precision) {
                 result.status = 0; // Converged despite f_diff == 0
                 result.root = x_curr;
                 result.iterations = i; // Iterations completed before this step
                 result.final_error = (fabs(x_curr - x_prev) < fabs(fx_curr)) ? fabs(x_curr - x_prev) : fabs(fx_curr);
            } else {
                result.status = 2; // Stagnation without convergence
                result.root = x_curr; // Return the last point
                result.iterations = i;
            }
            return result;
        }

        // Avoid x_curr == x_prev which could lead to division by zero if f_diff != 0 due to float issues
        // This check is technically redundant if f_diff != 0, but adds safety
        if (x_curr == x_prev) {
             //fprintf(stderr, "Warning [Secant]: x_curr == x_prev occurred unexpectedly at iteration %d.\n", i);
             result.status = 2; // Treat as stagnation
             result.root = x_curr;
             result.iterations = i;
             return result;
        }


        // Calculate the update term
        double update_term = fx_curr * (x_curr - x_prev) / f_diff;

         // Check for potential issues after division (Inf/NaN)
        if (isinf(update_term) || isnan(update_term)) {
             // fprintf(stderr, "Warning [Secant]: Update term became Inf/NaN at iteration %d.\n", i);
             result.status = 2; // Treat as error/stagnation
             result.root = x_curr;
             result.iterations = i;
             return result;
        }

        // Calculate the next approximation
        x_next = x_curr - update_term;

        result.iterations = i + 1; // Count this iteration

        // Calculate errors for convergence check
        double error_diff = fabs(x_next - x_curr);
        double error_abs_f = fabs(f(x_next)); // Check f at the *new* point

        // Store the smaller error value
        result.final_error = (error_diff < error_abs_f) ? error_diff : error_abs_f;

        // Check convergence: BOTH conditions must be met
        if (error_diff < precision && error_abs_f < precision) {
            result.status = 0; // Success
            result.root = x_next;
            return result;
        }

        // Update points for next iteration
        x_prev = x_curr;
        fx_prev = fx_curr;
        x_curr = x_next;
        fx_curr = f(x_curr);

        // Check if the iteration stalled (x_curr didn't change significantly)
        // This can happen with very flat functions or near machine precision limits
        // if (x_curr == x_prev) { // Direct check after update
        //     //fprintf(stderr, "Warning [Secant]: Iteration stalled (x_curr == x_prev) at iteration %d.\n", i);
        //     result.status = 2; // Indicate stagnation
        //     result.root = x_curr;
        //     return result;
        // }

    }

    // If loop finishes, max iterations were reached without convergence
    result.root = x_curr; // Return the last computed value
    return result;
}