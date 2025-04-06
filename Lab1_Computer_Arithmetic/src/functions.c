#include "../include/functions.h"
#include <math.h> // Required for powf, pow, powl, expf, exp, expl, logf, log, logl, fabsf, fabs, fabsl, NAN

/**
 * @file functions.c
 * @brief Implementation of various mathematical functions with different
 *        floating-point precisions (float, double, long double).
 */

/**
 * ---------------------------------------------------------------------------------------------------------------------
 * Function 1: Direct evaluation of a polynomial of degree 8.
 * f(x) = x^8 - 8x^7 + 28x^6 - 56x^5 + 70x^4 - 56x^3 + 28x^2 - 8x + 1
 * This function uses the naive method of computing each power individually using `pow` functions.
 * This method can be computationally more expensive and potentially less numerically stable
 * than alternatives like Horner's method, especially due to repeated power calculations.
 * ---------------------------------------------------------------------------------------------------------------------
 */

/**
 * @brief Calculates Function 1 using single-precision (float).
 * @param x Input value.
 * @return Result of the polynomial evaluation.
 */
float function1_float(float x) {
    // Naive sum of terms using powf for single precision
    return powf(x, 8) - 8.0f * powf(x, 7) + 28.0f * powf(x, 6) - 56.0f * powf(x, 5) + 70.0f * powf(x, 4) - 56.0f * powf(x, 3) + 28.0f * powf(x, 2) - 8.0f * x + 1.0f;
}

/**
 * @brief Calculates Function 1 using double-precision (double).
 * @param x Input value.
 * @return Result of the polynomial evaluation.
 */
double function1_double(double x) {
    // Naive sum of terms using pow for double precision
    return pow(x, 8) - 8.0 * pow(x, 7) + 28.0 * pow(x, 6) - 56.0 * pow(x, 5) + 70.0 * pow(x, 4) - 56.0 * pow(x, 3) + 28.0 * pow(x, 2) - 8.0 * x + 1.0;
}

/**
 * @brief Calculates Function 1 using extended-precision (long double).
 * @param x Input value.
 * @return Result of the polynomial evaluation.
 */
long double function1_long_double(long double x) {
    // Naive sum of terms using powl for long double precision
    return powl(x, 8) - 8.0L * powl(x, 7) + 28.0L * powl(x, 6) - 56.0L * powl(x, 5) + 70.0L * powl(x, 4) - 56.0L * powl(x, 3) + 28.0L * powl(x, 2) - 8.0L * x + 1.0L;
}


/**
 * ---------------------------------------------------------------------------------------------------------------------
 * Function 2: Horner’s method to evaluate the same degree-8 polynomial.
 * f(x) = (((((((x - 8)x + 28)x - 56)x + 70)x - 56)x + 28)x - 8)x + 1
 * This is a numerically more stable and computationally efficient method for polynomial evaluation.
 * It minimizes the number of multiplications and avoids large intermediate power calculations.
 * ---------------------------------------------------------------------------------------------------------------------
 */

/**
 * @brief Calculates Function 2 (Horner's method) using single-precision (float).
 * @param x Input value.
 * @return Result of the polynomial evaluation.
 */
float function2_float(float x) {
    // Horner's method implementation for float
    float outcome = x - 8.0f;
    outcome = outcome * x + 28.0f;
    outcome = outcome * x - 56.0f;
    outcome = outcome * x + 70.0f;
    outcome = outcome * x - 56.0f;
    outcome = outcome * x + 28.0f;
    outcome = outcome * x - 8.0f;
    outcome = outcome * x + 1.0f;
    return outcome;
}

/**
 * @brief Calculates Function 2 (Horner's method) using double-precision (double).
 * @param x Input value.
 * @return Result of the polynomial evaluation.
 */
double function2_double(double x) {
    // Horner's method implementation for double
    double outcome = x - 8.0;
    outcome = outcome * x + 28.0;
    outcome = outcome * x - 56.0;
    outcome = outcome * x + 70.0;
    outcome = outcome * x - 56.0;
    outcome = outcome * x + 28.0;
    outcome = outcome * x - 8.0;
    outcome = outcome * x + 1.0;
    return outcome;
}

/**
 * @brief Calculates Function 2 (Horner's method) using extended-precision (long double).
 * @param x Input value.
 * @return Result of the polynomial evaluation.
 */
long double function2_long_double(long double x) {
    // Horner's method implementation for long double
    long double outcome = x - 8.0L;
    outcome = outcome * x + 28.0L;
    outcome = outcome * x - 56.0L;
    outcome = outcome * x + 70.0L;
    outcome = outcome * x - 56.0L;
    outcome = outcome * x + 28.0L;
    outcome = outcome * x - 8.0L;
    outcome = outcome * x + 1.0L;
    return outcome;
}


/**
 * ---------------------------------------------------------------------------------------------------------------------
 * Function 3: Perfect eighth power function.
 * f(x) = (x - 1)^8
 * This form is mathematically equivalent to the polynomials in Function 1 and 2
 * (since (x-1)^8 expands to the same polynomial).
 * It is useful for testing numerical behavior, especially the loss of precision
 * when x is very close to 1 (subtractive cancellation).
 * ---------------------------------------------------------------------------------------------------------------------
 */

/**
 * @brief Calculates Function 3 using single-precision (float).
 * @param x Input value.
 * @return Result of (x - 1)^8.
 */
float function3_float(float x) {
    // Calculate difference first, then power using powf
    float difference = x - 1.0f;
    return powf(difference, 8);
}

/**
 * @brief Calculates Function 3 using double-precision (double).
 * @param x Input value.
 * @return Result of (x - 1)^8.
 */
double function3_double(double x) {
    // Calculate difference first, then power using pow
    double difference = x - 1.0;
    return pow(difference, 8);
}

/**
 * @brief Calculates Function 3 using extended-precision (long double).
 * @param x Input value.
 * @return Result of (x - 1)^8.
 */
long double function3_long_double(long double x) {
    // Calculate difference first, then power using powl
    long double difference = x - 1.0L;
    return powl(difference, 8);
}


/**
 * ---------------------------------------------------------------------------------------------------------------------
 * Function 4: Equivalent formulation using exponential and logarithmic functions.
 * f(x) = e^(8 * ln(|x - 1|)), for x != 1
 * Mathematically, this is equivalent to Function 3 (for x != 1).
 * However, it involves logarithm and exponential functions, which introduce their own
 * sources of numerical error and potential performance differences compared to direct power calculation.
 * It explicitly handles the case x = 1, where log(0) is undefined, by returning NAN.
 * ---------------------------------------------------------------------------------------------------------------------
 */

/**
 * @brief Calculates Function 4 using single-precision (float).
 * @param x Input value.
 * @return Result of exp(8 * log(|x - 1|)), or NAN if x == 1.0f.
 */
float function4_float(float x) {
    // Check for the undefined case x = 1
    if (x == 1.0f) return NAN;
    // Calculate using expf, logf, fabsf for single precision
    return expf(8.0f * logf(fabsf(x - 1.0f)));
}

/**
 * @brief Calculates Function 4 using double-precision (double).
 * @param x Input value.
 * @return Result of exp(8 * log(|x - 1|)), or NAN if x == 1.0.
 */
double function4_double(double x) {
    // Check for the undefined case x = 1
    if (x == 1.0) return NAN;
    // Calculate using exp, log, fabs for double precision
    return exp(8.0 * log(fabs(x - 1.0)));
}

/**
 * @brief Calculates Function 4 using extended-precision (long double).
 * @param x Input value.
 * @return Result of exp(8 * log(|x - 1|)), or NAN if x == 1.0L.
 */
long double function4_long_double(long double x) {
    // Check for the undefined case x = 1
    if (x == 1.0L) return NAN; // Use L suffix for long double literal
    // Calculate using expl, logl, fabsl for long double precision
    return expl(8.0L * logl(fabsl(x - 1.0L)));
}