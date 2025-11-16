#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h> // For timing

// --- Problem Parameters ---
#define K_PARAM 9.0
#define M_PARAM 0.5
#define TRUE_X_SEED 42 // Seed for generating true x vector
#define POWER_ITERATION_MAX_ITER 1000
#define POWER_ITERATION_TOL 1e-9

// --- Jacobi Solver Parameters ---
#define JACOBI_MAX_ITER 10000 // Max iterations for Jacobi

// Enum for stopping criterion
typedef enum {
    INCREMENT_CRITERION, // 0
    RESIDUAL_CRITERION   // 1
} StopCriterionType;

// Enum for identifying initial vectors
typedef enum {
    X0_ZEROS,            // 0
    X0_ONES,             // 1
    X0_RANDOM_LOW,       // 2
    X0_ALTERNATING_HIGH  // 3
} InitialVectorType;

// Structure to hold results for a single experiment run
typedef struct {
    int n;
    InitialVectorType x0_type;
    StopCriterionType criterion_type;
    double rho;
    int iterations_taken;
    double time_solve_sec;
    double time_per_iteration_sec;
    double final_abs_error; // ||x_computed - x_true||_max
    double final_criterion_metric; // Value of the criterion at stop
} ExperimentData;

#endif // COMMON_H