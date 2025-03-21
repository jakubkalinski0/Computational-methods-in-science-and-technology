#include <stdio.h>
#include <math.h>
#include "interpolation.h"

#define PI 3.14159265358979323846

// Function to initialize data
void initData(InterpolationData *data) {
    printf("Enter number of nodes: ");
    scanf_s("%d", &data->n);
    
    if (data->n > MAX_NODES) {
        printf("Maximum number of nodes exceeded. Setting n = %d\n", MAX_NODES);
        data->n = MAX_NODES;
    }
    
    printf("Enter interval [a,b] (e.g. -1 1): ");
    scanf_s("%lf %lf", &data->a, &data->b);
    
    printf("Choose node distribution (0 - uniform, 1 - Chebyshev): ");
    scanf_s("%d", &data->node_type);
    
    printf("Choose interpolation method (0 - Lagrange, 1 - Newton): ");
    scanf_s("%d", &data->method);
    
    generateNodes(data);
    
    printf("Enter function values at nodes:\n");
    for (int i = 0; i < data->n; i++) {
        printf("f(%.4f) = ", data->x[i]);
        scanf_s("%lf", &data->y[i]);
    }
}

// Function to generate nodes
void generateNodes(InterpolationData *data) {
    if (data->node_type == 0) {
        // Uniform nodes
        double h = (data->b - data->a) / (data->n - 1);
        for (int i = 0; i < data->n; i++) {
            data->x[i] = data->a + i * h;
        }
    } else {
        // Chebyshev nodes
        for (int i = 0; i < data->n; i++) {
            data->x[i] = 0.5 * (data->a + data->b) + 
                         0.5 * (data->b - data->a) * 
                         cos(PI * (2 * i + 1) / (2 * data->n));
        }
    }
    
    printf("Generated nodes:\n");
    for (int i = 0; i < data->n; i++) {
        printf("x[%d] = %.4f\n", i, data->x[i]);
    }
}

// Lagrange interpolation function
double lagrangeInterpolation(InterpolationData *data, double x) {
    double result = 0.0;
    
    for (int i = 0; i < data->n; i++) {
        double term = data->y[i];
        
        for (int j = 0; j < data->n; j++) {
            if (j != i) {
                term *= (x - data->x[j]) / (data->x[i] - data->x[j]);
            }
        }
        
        result += term;
    }
    
    return result;
}

// Function to calculate divided differences for Newton's method
void calculateDividedDifferences(InterpolationData *data, double *dd) {
    // Copy function values to the first column of divided differences array
    for (int i = 0; i < data->n; i++) {
        dd[i * data->n] = data->y[i];
    }
    
    // Calculate higher order divided differences
    for (int j = 1; j < data->n; j++) {
        for (int i = 0; i < data->n - j; i++) {
            dd[i * data->n + j] = (dd[i * data->n + j - 1] - dd[(i + 1) * data->n + j - 1]) / 
                                   (data->x[i] - data->x[i + j]);
        }
    }
}

// Newton interpolation function
double newtonInterpolation(InterpolationData *data, double x, double *dd) {
    double result = dd[0]; // first divided difference (value at first node)
    double term = 1.0;
    
    for (int i = 1; i < data->n; i++) {
        term *= (x - data->x[i - 1]);
        result += dd[i * data->n] * term;
    }
    
    return result;
}