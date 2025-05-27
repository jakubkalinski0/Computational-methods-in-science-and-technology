#ifndef GAUSSIAN_ELIM_H
#define GAUSSIAN_ELIM_H

#include "common.h"

bool solve_system_gauss_spp(const Matrix *A_orig, const Vector *b_orig, Vector *x_sol, const char* precision_dtype);

#endif // GAUSSIAN_ELIM_H