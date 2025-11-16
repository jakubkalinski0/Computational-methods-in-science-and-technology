#ifndef SPECTRAL_RADIUS_H
#define SPECTRAL_RADIUS_H

#include "common.h"
#include "matrix_operations.h"

double** create_jacobi_iteration_matrix_T(double** A, int n);
double estimate_spectral_radius_power_iteration(double** T_J, int n, int max_iter, double tol, int* iters_taken);

#endif // SPECTRAL_RADIUS_H