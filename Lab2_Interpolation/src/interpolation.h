#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#define MAX_NODES 100

// Structure for interpolation data
typedef struct {
    int n;              // number of nodes
    double a, b;        // interval [a,b]
    double x[MAX_NODES]; // interpolation nodes
    double y[MAX_NODES]; // function values at nodes
    int node_type;      // 0 - uniform, 1 - Chebyshev
    int method;         // 0 - Lagrange, 1 - Newton
} InterpolationData;

// Function declarations
void initData(InterpolationData *data);
void generateNodes(InterpolationData *data);
double lagrangeInterpolation(InterpolationData *data, double x);
void calculateDividedDifferences(InterpolationData *data, double *dd);
double newtonInterpolation(InterpolationData *data, double x, double *dd);

#endif // INTERPOLATION_H