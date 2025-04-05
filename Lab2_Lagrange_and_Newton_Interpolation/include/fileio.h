#ifndef FILEIO_H
#define FILEIO_H
#include "common.h"

void saveDataToFile(const char* filename, double x[], double y[], int n);
void saveNodesToFile(const char* filename, double nodes[], double values[], int n);
void generateErrorPlotScript(int maxNodes,
                           double lagrange_uniform_errors[],
                           double lagrange_chebyshev_errors[],
                           double newton_uniform_errors[],
                           double newton_chebyshev_errors[]);
void saveLagrangeUniformErrorsToFile(int maxNodes, double errors[], double mse[]);
void saveLagrangeChebyshevErrorsToFile(int maxNodes, double errors[], double mse[]);
void saveNewtonUniformErrorsToFile(int maxNodes, double errors[], double mse[]);
void saveNewtonChebyshevErrorsToFile(int maxNodes, double errors[], double mse[]);
void generateGnuplotScript(int maxNodes,
                          double lagrange_uniform_errors[],
                          double lagrange_chebyshev_errors[],
                          double newton_uniform_errors[],
                          double newton_chebyshev_errors[]);
#endif // FILEIO_H