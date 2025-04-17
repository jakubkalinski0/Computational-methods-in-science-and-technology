/**
 * @file function.c
 * @brief Implementation of the function to be interpolated and its parameters.
 */
#include "../include/function.h"
#include <math.h> // For sin() and exp()

// Define function parameters and interval bounds as global constants
// These are declared 'extern' in common.h
const double k = 4.0;  // Parameter 'k' in the sin term
const double m = 0.4;  // Parameter 'm' in the exp term
const double PI = 3.14159265358979323846; // Value of Pi
const double a = -2.0 * PI * PI; // Start of the interval [a, b] (~ -19.74)
const double b = PI * PI;        // End of the interval [a, b] (~ 9.87)

/**
 * @brief Computes the value of the function f(x) to be interpolated.
 *
 * The function is defined as f(x) = sin(k * x / PI) * exp(-m * x / PI).
 *
 * @param x The input value (independent variable).
 * @return The computed value of the function f(x).
 */
double f(double x) {
    // Calculate sin(k * x / PI)
    double sin_term = sin(k * x / PI);
    // Calculate exp(-m * x / PI)
    double exp_term = exp(-m * x / PI);
    // Return the product
    return sin_term * exp_term;
}