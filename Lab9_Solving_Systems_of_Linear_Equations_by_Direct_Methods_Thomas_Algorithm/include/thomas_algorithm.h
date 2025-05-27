#ifndef THOMAS_ALGORITHM_H
#define THOMAS_ALGORITHM_H

#include "common.h"

bool thomas_solve(const Matrix *A_banded_nx3, const Vector *d_rhs, Vector *x_sol, const char* precision_dtype);

#endif // THOMAS_ALGORITHM_H