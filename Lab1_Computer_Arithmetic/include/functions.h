/* functions.h */
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/**
 * @file functions.h
 * @brief Header file containing declarations for various mathematical functions
 *        implemented with different floating-point precision types (float, double, long double).
 *        These functions are designed to compare numerical stability and performance
 *        of different calculation methods and precisions.
 */

// --- Function 1: Direct Polynomial Evaluation ---
// Calculates f(x) = x^8 - 8x^7 + 28x^6 - 56x^5 + 70x^4 - 56x^3 + 28x^2 - 8x + 1
// using the naive method (summing individual powers).

/**
 * @brief Calculates Function 1 using single-precision floating-point arithmetic.
 * @param x Input value (float).
 * @return Result of the polynomial evaluation (float).
 */
float function1_float(float x);

/**
 * @brief Calculates Function 1 using double-precision floating-point arithmetic.
 * @param x Input value (double).
 * @return Result of the polynomial evaluation (double).
 */
double function1_double(double x);

/**
 * @brief Calculates Function 1 using extended-precision floating-point arithmetic.
 * @param x Input value (long double).
 * @return Result of the polynomial evaluation (long double).
 */
long double function1_long_double(long double x);


// --- Function 2: Horner's Method Polynomial Evaluation ---
// Calculates the same polynomial as Function 1:
// f(x) = (((((((x - 8)x + 28)x - 56)x + 70)x - 56)x + 28)x - 8)x + 1
// using Horner's method for potentially better numerical stability and efficiency.

/**
 * @brief Calculates Function 2 (Horner's method) using single-precision floating-point arithmetic.
 * @param x Input value (float).
 * @return Result of the polynomial evaluation (float).
 */
float function2_float(float x);

/**
 * @brief Calculates Function 2 (Horner's method) using double-precision floating-point arithmetic.
 * @param x Input value (double).
 * @return Result of the polynomial evaluation (double).
 */
double function2_double(double x);

/**
 * @brief Calculates Function 2 (Horner's method) using extended-precision floating-point arithmetic.
 * @param x Input value (long double).
 * @return Result of the polynomial evaluation (long double).
 */
long double function2_long_double(long double x);


// --- Function 3: Direct Power Calculation ---
// Calculates f(x) = (x - 1)^8 using direct power computation.
// This is mathematically equivalent to Function 1 and Function 2.

/**
 * @brief Calculates Function 3 using single-precision floating-point arithmetic.
 * @param x Input value (float).
 * @return Result of (x - 1)^8 (float).
 */
float function3_float(float x);

/**
 * @brief Calculates Function 3 using double-precision floating-point arithmetic.
 * @param x Input value (double).
 * @return Result of (x - 1)^8 (double).
 */
double function3_double(double x);

/**
 * @brief Calculates Function 3 using extended-precision floating-point arithmetic.
 * @param x Input value (long double).
 * @return Result of (x - 1)^8 (long double).
 */
long double function3_long_double(long double x);


// --- Function 4: Exponential/Logarithmic Formulation ---
// Calculates f(x) = exp(8 * log(|x - 1|)) for x != 1.
// Mathematically equivalent to Function 3, but prone to different numerical errors,
// especially near x = 1. Returns NAN for x = 1.

/**
 * @brief Calculates Function 4 using single-precision floating-point arithmetic.
 * @param x Input value (float).
 * @return Result of exp(8 * log(|x - 1|)) (float), or NAN if x is 1.
 */
float function4_float(float x);

/**
 * @brief Calculates Function 4 using double-precision floating-point arithmetic.
 * @param x Input value (double).
 * @return Result of exp(8 * log(|x - 1|)) (double), or NAN if x is 1.
 */
double function4_double(double x);

/**
 * @brief Calculates Function 4 using extended-precision floating-point arithmetic.
 * @param x Input value (long double).
 * @return Result of exp(8 * log(|x - 1|)) (long double), or NAN if x is 1.
 */
long double function4_long_double(long double x);

#endif /* FUNCTIONS_H */