#include "../include/functions.h"
#include <math.h>

// Function 1: f(x) = x^8 - 8x^7 + 28x^6 - 56x^5 + 70x^4 - 56x^3 + 28x^2 - 8x + 1
float function1_float(float x) {
    return powf(x, 8) - 8*powf(x, 7) + 28*powf(x, 6) - 56*powf(x, 5) + 70*powf(x, 4) - 56*powf(x, 3) + 28*powf(x, 2) - 8*x + 1;
}

double function1_double(double x) {
    return pow(x, 8) - 8*pow(x, 7) + 28*pow(x, 6) - 56*pow(x, 5) + 70*pow(x, 4) - 56*pow(x, 3) + 28*pow(x, 2) - 8*x + 1;
}

long double function1_long_double(long double x) {
    return powl(x, 8) - 8*powl(x, 7) + 28*powl(x, 6) - 56*powl(x, 5) + 70*powl(x, 4) - 56*powl(x, 3) + 28*powl(x, 2) - 8*x + 1;
}

// Function 2: f(x) = (((((((x - 8)x + 28)x - 56)x + 70)x - 56)x + 28)x - 8)x + 1 (Horner's diagram)
float function2_float(float x) {
    float outcome = x - 8;
    outcome = outcome * x + 28;
    outcome = outcome * x - 56;
    outcome = outcome * x + 70;
    outcome = outcome * x - 56;
    outcome = outcome * x + 28;
    outcome = outcome * x - 8;
    outcome = outcome * x + 1;
    return outcome;
}

double function2_double(double x) {
    double outcome = x - 8;
    outcome = outcome * x + 28;
    outcome = outcome * x - 56;
    outcome = outcome * x + 70;
    outcome = outcome * x - 56;
    outcome = outcome * x + 28;
    outcome = outcome * x - 8;
    outcome = outcome * x + 1;
    return outcome;
}

long double function2_long_double(long double x) {
    long double outcome = x - 8;
    outcome = outcome * x + 28;
    outcome = outcome * x - 56;
    outcome = outcome * x + 70;
    outcome = outcome * x - 56;
    outcome = outcome * x + 28;
    outcome = outcome * x - 8;
    outcome = outcome * x + 1;
    return outcome;
}

// Function 3: f(x) = (x - 1)^8
float function3_float(float x) {
    float difference = x - 1.0f;
    return powf(difference, 8);
}

double function3_double(double x) {
    double difference = x - 1.0;
    return pow(difference, 8);
}

long double function3_long_double(long double x) {
    long double difference = x - 1.0L;
    return powl(difference, 8);
}

// Function 4: f(x) = e^(8*ln(abs(x-1))), x ≠ 1
float function4_float(float x) {
    if (x == 1.0f) return NAN; // Handling the case x = 1
    return expf(8.0f * logf(fabsf(x - 1.0f)));
}

double function4_double(double x) {
    if (x == 1.0) return NAN; // Handling the case x = 1
    return exp(8.0 * log(fabs(x - 1.0)));
}

long double function4_long_double(long double x) {
    if (x == 1.0L) return NAN; // Handling the case x = 1
    return expl(8.0L * logl(fabsl(x - 1.0L)));
}