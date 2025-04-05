#ifndef ERROR_H
#define ERROR_H
#include "common.h"

typedef struct {
    double max_error;
    double mean_squared_error;
} ErrorResult;

ErrorResult calculateError(double trueValues[], double interpValues[], int numPoints);
#endif // ERROR_H