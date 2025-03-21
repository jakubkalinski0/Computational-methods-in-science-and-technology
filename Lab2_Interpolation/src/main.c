#include <stdio.h>
#include <stdlib.h>
#include "interpolation.h"
#include "visualization.h"

int main() {
    InterpolationData data;
    
    printf("Polynomial Interpolation Program\n");
    printf("================================\n");
    
    initData(&data);
    
    // Allocate memory for divided differences for Newton's method
    double *dividedDifferences = (double*)malloc(data.n * data.n * sizeof(double));
    
    if (data.method == 1) {
        calculateDividedDifferences(&data, dividedDifferences);
        
        printf("\nDivided differences:\n");
        for (int i = 0; i < data.n; i++) {
            printf("dd[%d] = %.6f\n", i, dividedDifferences[i * data.n]);
        }
    }
    
    // Option to test interpolation for a specific x value
    char choice;
    printf("\nDo you want to test interpolation for a specific x value? (y/n): ");
    scanf(" %c", &choice);
    
    if (choice == 'y' || choice == 'Y') {
        double x;
        printf("Enter x value: ");
        scanf("%lf", &x);
        
        double result;
        if (data.method == 0) {
            result = lagrangeInterpolation(&data, x);
            printf("Lagrange polynomial value at x = %.4f is: %.6f\n", x, result);
        } else {
            result = newtonInterpolation(&data, x, dividedDifferences);
            printf("Newton polynomial value at x = %.4f is: %.6f\n", x, result);
        }
    }
    
    // Save data for visualization
    saveDataForPlot(&data, dividedDifferences);
    
    // Free memory
    free(dividedDifferences);
    
    return 0;
}