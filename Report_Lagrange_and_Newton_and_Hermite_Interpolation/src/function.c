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

/**
 * @brief Computes the first derivative of the function f(x) with respect to x.
 *
 * This function calculates f'(x), the derivative of f(x) = sin(k * x / PI) * exp(-m * x / PI).
 * The derivative is calculated using the product rule:
 * f'(x) = (d/dx [sin(kx/PI)]) * exp(-mx/PI) + sin(kx/PI) * (d/dx [exp(-mx/PI)])
 * f'(x) = (k/PI * cos(kx/PI)) * exp(-mx/PI) + sin(kx/PI) * (-m/PI * exp(-mx/PI))
 * f'(x) = (1/PI) * exp(-mx/PI) * [ k*cos(kx/PI) - m*sin(kx/PI) ]
 * This derivative is needed for methods like Hermite interpolation.
 *
 * @param x The input value (independent variable) at which to evaluate the derivative.
 * @return The computed value of the first derivative f'(x).
 */
double df(double x) {
    // Calculate the first term: (k/PI) * cos(k*x/PI) * exp(-m*x/PI)
    double term1 = (k / PI) * cos(k * x / PI) * exp(-m * x / PI);
    // Calculate the second term: (-m/PI) * sin(k*x/PI) * exp(-m*x/PI)
    double term2 = (-m / PI) * sin(k * x / PI) * exp(-m * x / PI);
    // Return the sum of the two terms (result of the product rule)
    return term1 + term2;
}