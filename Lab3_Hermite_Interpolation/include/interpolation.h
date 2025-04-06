#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include "common.h"

double lagrangeInterpolation(double x, double nodes[], double values[], int n);
double newtonInterpolation(double x, double nodes[], double values[], int n);
double hermiteInterpolation(double x, double nodes[], double values[], double derivatives[], int n);

#endif // INTERPOLATION_H