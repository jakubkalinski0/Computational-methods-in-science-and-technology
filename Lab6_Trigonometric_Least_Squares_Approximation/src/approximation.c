/**
 * @file approximation.c
 * @brief Implementation of trigonometric approximation using direct summation formulas.
 *
 * Contains functions to calculate trigonometric coefficients using direct formulas
 * (approximating Fourier integrals) and to evaluate the resulting trigonometric sum.
 * This approach is computationally efficient but relies on uniform sampling properties
 * and enforces the m < n/2 condition based on sampling theory.
 */
#include "../include/approximation.h"
#include <stdio.h>   // For fprintf
#include <stdlib.h>  // Standard library utilities
#include <math.h>    // For cos, sin, NAN

/**
 * @brief Calculates trigonometric coefficients using direct summation formulas.
 *
 * Implements the direct calculation based on sums approximating Fourier integrals.
 * Enforces the condition m < n/2 for validity and stability.
 */
int calculateTrigonometricCoeffsDirect(double points_x[], double points_y[], int n, int max_harmonic_m, double coeffs_out[]) {
    // --- Input Validation ---
    if (max_harmonic_m < 0) {
        fprintf(stderr, "Error [calculateTrigonometricCoeffsDirect]: Max harmonic m (%d) cannot be negative.\n", max_harmonic_m);
        return -1;
    }
    if (n <= 0) {
        fprintf(stderr, "Error [calculateTrigonometricCoeffsDirect]: Number of points n (%d) must be positive.\n", n);
        return -1;
    }

    // --- Enforce the crucial condition m < n/2 derived from sampling theory ---
    // Use floating point comparison to handle n/2 correctly for odd n
    if (max_harmonic_m >= (double)n / 2.0) {
         // This case should be caught in main.c, but double-check here.
         // Do not print error here as main handles logging NAN.
         // fprintf(stderr, "Error [calculateTrigonometricCoeffsDirect]: Condition m < n/2 not met (m=%d, n=%d). Skipping calculation.\n", max_harmonic_m, n);
         return -1; // Indicate failure for this combination
    }

    // --- Calculate a0 coefficient ---
    // a_k = (2/n) * Sum[y_i * cos(k*omega*x_i)], for k=0, cos(0)=1
    double sum_y = 0.0;
    for (int i = 0; i < n; i++) {
        sum_y += points_y[i];
    }
    // Store a0 directly. The factor 1/2 is applied during evaluation.
    coeffs_out[0] = (2.0 / (double)n) * sum_y;

    // --- Calculate ak and bk coefficients for k = 1 to m ---
    for (int k = 1; k <= max_harmonic_m; k++) {
        double sum_y_cos = 0.0;
        double sum_y_sin = 0.0;
        for (int i = 0; i < n; i++) {
            // Use the global omega adapted to the original interval [a,b]
            double arg = (double)k * omega * points_x[i];
            sum_y_cos += points_y[i] * cos(arg);
            sum_y_sin += points_y[i] * sin(arg);
        }
        // ak = (2/n) * Sum[y_i * cos(k*omega*x_i)]
        double ak = (2.0 / (double)n) * sum_y_cos;
        // bk = (2/n) * Sum[y_i * sin(k*omega*x_i)]
        double bk = (2.0 / (double)n) * sum_y_sin;

        // Store coefficients in the output array:
        // a_k -> index 2k-1
        // b_k -> index 2k
        coeffs_out[2 * k - 1] = ak;
        coeffs_out[2 * k]     = bk;
    }

    return 0; // Indicate successful calculation
}

/**
 * @brief Evaluates a trigonometric sum T_m(x) at a given point x.
 *
 * Calculates T_m(x) = coeffs[0]/2 + Sum_{k=1}^{m} [coeffs[2k-1]*cos(k*omega*x) + coeffs[2k]*sin(k*omega*x)]
 */
double evaluateTrigonometricSum(double x, double coeffs[], int max_harmonic_m) {
     // --- Input Validation ---
    if (max_harmonic_m < 0) {
         // Return Not-a-Number for invalid max harmonic order
         return NAN;
    }

    // Initialize result with the constant term (a0/2)
    double result = coeffs[0] / 2.0;

    // Add contributions from higher harmonics (k = 1 to m)
    for (int k = 1; k <= max_harmonic_m; k++) {
        double arg = (double)k * omega * x; // Argument for trig functions
        // Retrieve ak and bk from the correctly indexed positions in the coeffs array
        double ak = coeffs[2 * k - 1]; // a_k is at index 2k-1
        double bk = coeffs[2 * k];     // b_k is at index 2k
        // Add the contribution of the k-th harmonic
        result += ak * cos(arg) + bk * sin(arg);
    }

    return result; // Return the final computed value of the sum
}