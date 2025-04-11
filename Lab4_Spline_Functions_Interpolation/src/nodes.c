/**
 * @file nodes.c
 * @brief Implementation of interpolation node generation functions.
 */
#include "../include/nodes.h"
#include <math.h>   // For cos() and NAN if needed
#include <stdio.h>  // For fprintf

/**
 * @brief Generates n uniformly spaced nodes in the interval [a, b].
 */
void uniformNodes(double nodes[], int n) {
    if (n < 1) {
        fprintf(stderr, "Warning: uniformNodes called with n=%d. No nodes generated.\n", n);
        return;
    }
    if (n == 1) {
        nodes[0] = (a + b) / 2.0;
        return;
    }
    double step = (b - a) / (n - 1.0);
    if (fabs(b-a) < 1e-15 && n > 1) {
         fprintf(stderr, "Warning: uniformNodes called on a zero-width interval [a=b=%g] with n=%d.\n", a, n);
         for (int i = 0; i < n; i++) nodes[i] = a;
         return;
    }
     if (step <= 0 && n > 1) {
         fprintf(stderr, "Error: Calculated step size is non-positive (%g) for n=%d on interval [%g, %g].\n", step, n, a, b);
          for (int i = 0; i < n; i++) nodes[i] = NAN;
         return;
     }
    for (int i = 0; i < n; i++) {
        nodes[i] = a + i * step;
    }
    nodes[n-1] = b;
}

/**
 * @brief Generates n Chebyshev nodes in the interval [a, b].
 * Nodes are roots of Chebyshev polynomial mapped to [a,b], sorted ascending.
 */
void chebyshevNodes(double nodes[], int n) {
     if (n < 1) {
        fprintf(stderr, "Warning: chebyshevNodes called with n=%d. No nodes generated.\n", n);
        return;
     }
    for (int i = 0; i < n; i++) {
        // Standard formula for Chebyshev nodes of the first kind in [-1, 1]
        double angle = (2.0 * (double)i + 1.0) * PI / (2.0 * (double)n);
        double t = cos(angle); // Roots in [-1, 1], descending order

        // Map t from [-1, 1] to [a, b]
        double node_val = 0.5 * (a + b) + 0.5 * (b - a) * t;

        // Store in ascending order by filling the array backwards
        nodes[n - 1 - i] = node_val;
    }
    // Ensure endpoints for Chebyshev nodes (optional, standard formula doesn't guarantee exact a, b)
    // If needed, adjust mapping slightly or use Chebyshev nodes of the second kind (extrema).
    // Let's stick to the standard roots formula for now.
}