#ifndef JACOBI_SOLVER_H
#define JACOBI_SOLVER_H

#include "common.h"
// matrix_operations.h jest zwykle potrzebny, jeśli typy argumentów go używają,
// ale common.h jest kluczowy dla StopCriterionType
// #include "matrix_operations.h" // Jeśli ExperimentData używałoby typów z matrix_operations

// Deklaracja funkcji jacobi_solve
int jacobi_solve(double** A, double* b, double* x_solution, int n,
                 double* x_initial, double rho_tolerance, StopCriterionType criterion_type,
                 int max_iterations, int* iterations_taken_ptr,
                 double* final_criterion_metric_ptr, double* time_taken_sec_ptr);

#endif // JACOBI_SOLVER_H