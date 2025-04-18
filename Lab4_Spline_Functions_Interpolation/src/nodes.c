/**
 * @file nodes.c
 * @brief Implementation of interpolation node generation functions.
 */
#include "../include/nodes.h"
#include <math.h> // For cos() and PI constant (though PI is extern const from common.h/function.c)
#include <stdio.h>  // For fprintf

/**
 * @brief Generates n uniformly spaced nodes in the interval [a, b].
 *
 * Nodes are calculated as x_i = a + i * h, where h = (b - a) / (n - 1).
 * For n=1, the node is placed at the midpoint (a+b)/2.
 * Ensures the last node is exactly 'b' to avoid floating point drift.
 *
 * @param nodes Output array (size n) where the generated node x-coordinates will be stored.
 * @param n The number of nodes to generate (must be >= 1).
 */
void uniformNodes(double nodes[], int n) {
    // Handle invalid input (n < 1)
    if (n < 1) {
        fprintf(stderr, "Warning: uniformNodes called with n=%d. No nodes generated.\n", n);
        return;
    }
    // Handle the edge case of a single node (n=1)
    if (n == 1) {
        nodes[0] = (a + b) / 2.0; // Place the single node in the middle of the interval
        return;
    }
    // Calculate the step size between nodes for n > 1
    double step = (b - a) / (n - 1.0); // Use 1.0 for floating point division
    // Generate nodes using the formula x_i = a + i * step
    for (int i = 0; i < n; i++) {
        nodes[i] = a + i * step;
    }
    // Ensure the last node is exactly 'b' to counteract potential floating-point errors
    // especially if step calculation introduces small inaccuracies.
    nodes[n-1] = b;
}

/**
 * @brief Generates n Chebyshev nodes in the interval [a, b].
 *
 * These nodes are the projections onto the x-axis of points equally spaced
 * around a semicircle. They are derived from the roots of the Chebyshev
 * polynomial of the first kind, T_n(t).
 * Formula for roots in [-1, 1]: t_k = cos( (2k + 1) * PI / (2n) ) for k = 0, ..., n-1
 * (or equivalently k=1..n using (2k-1)*PI/(2n))
 * These roots t_k are then mapped from [-1, 1] to the interval [a, b] using:
 * x_k = (a + b) / 2 + (b - a) / 2 * t_k
 * The nodes are denser near the endpoints 'a' and 'b'.
 *
 * @param nodes Output array (size n) where the generated node x-coordinates will be stored (in ascending order).
 * @param n The number of nodes to generate (must be >= 1).
 */
void chebyshevNodes(double nodes[], int n) {
    // Handle invalid input (n < 1)
     if (n < 1) {
        fprintf(stderr, "Warning: chebyshevNodes called with n=%d. No nodes generated.\n", n);
        return;
     }

    // Iterate to calculate each Chebyshev node
    for (int i = 0; i < n; i++) {
        // Calculate the k-th root (using index 'i' from 0 to n-1)
        // The argument to cos is (2*(i+1) - 1) * PI / (2*n) = (2i + 1) * PI / (2n)
        double angle = (2.0 * (double)i + 1.0) * PI / (2.0 * (double)n);
        // Calculate the Chebyshev root t_i in the interval [-1, 1]
        double t = cos(angle); // Note: cos argument order gives nodes in descending order

        // Map the root t_i from [-1, 1] to the interval [a, b]
        // node = midpoint + half_range * t
        double node_val = 0.5 * (a + b) + 0.5 * (b - a) * t;

        // Store the nodes in ascending order. Since cos() gives descending t values
        // for increasing angle in [0, PI], we store them in reverse order in the array.
        nodes[n - 1 - i] = node_val;
    }
    // Note: For n=1, angle=PI/2, t=cos(PI/2)=0, node=(a+b)/2, which is correct.
    // The loop correctly places nodes[0] = (a+b)/2 + (b-a)/2 * cos(PI/2) = (a+b)/2.
}