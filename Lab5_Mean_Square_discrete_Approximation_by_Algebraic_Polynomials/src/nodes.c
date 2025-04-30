/**
 * @file nodes.c
 * @brief Implementation of functions for generating node distributions (uniform, Chebyshev).
 *
 * Provides the implementation details for creating arrays of x-coordinates
 * representing either uniformly spaced nodes or Chebyshev nodes within the
 * interval [a, b] defined in common.h.
 */
#include "../include/nodes.h"
#include <math.h>   // For cos() and the PI constant (though PI is also extern from common.h)
#include <stdio.h>  // For fprintf (warning messages)

/**
 * @brief Generates `n` uniformly spaced nodes in the interval [a, b].
 *
 * Calculates nodes using the formula: x_i = a + i * step, for i = 0, ..., n-1.
 * The step size `step` is calculated as (b - a) / (n - 1) when n > 1.
 * If n = 1, the single node is placed at the midpoint (a + b) / 2.0.
 * If n < 1, a warning is printed to stderr, and the function returns without modifying `nodes`.
 * Ensures the last node (nodes[n-1]) is exactly equal to `b` to mitigate potential
 * floating-point accumulation errors.
 *
 * @param nodes Output array (size `n`) where the generated node x-coordinates will be stored.
 * @param n The number of nodes to generate (must be >= 1).
 */
void uniformNodes(double nodes[], int n) {
    // --- Input Validation ---
    if (n < 1) {
        fprintf(stderr, "Warning [uniformNodes]: Called with n=%d (must be >= 1). No nodes generated.\n", n);
        return; // Do nothing if n is invalid
    }

    // --- Handle Edge Case: n = 1 ---
    if (n == 1) {
        nodes[0] = (a + b) / 2.0; // Place the single node at the interval midpoint
        return;
    }

    // --- Calculate Nodes for n > 1 ---
    // Calculate the step size between nodes. Use floating-point division.
    double step = (b - a) / (double)(n - 1);

    // Generate nodes using the formula x_i = a + i * step
    for (int i = 0; i < n; i++) {
        nodes[i] = a + (double)i * step;
    }

    // Ensure the last node is precisely 'b' to avoid floating-point drift.
    // This is important as accumulation of 'step' might not exactly reach 'b'.
    nodes[n-1] = b;
}

/**
 * @brief Generates `n` Chebyshev nodes in the interval [a, b].
 *
 * Chebyshev nodes are derived from the roots of the Chebyshev polynomial of the
 * first kind, T_n(t). The roots in the standard interval [-1, 1] are given by:
 *   t_k = cos( (2*k + 1) * PI / (2*n) )  for k = 0, 1, ..., n-1.
 * These roots `t_k` are then linearly mapped to the target interval [a, b] using:
 *   x_k = midpoint + half_range * t_k
 *   x_k = (a + b) / 2.0 + (b - a) / 2.0 * t_k
 *
 * The resulting nodes `x_k` are clustered near the endpoints `a` and `b`.
 * Handles n < 1 by printing a warning and returning.
 * The nodes are stored in the output array in ascending order.
 *
 * @param nodes Output array (size `n`) where the generated Chebyshev node x-coordinates
 *              will be stored in ascending order. The caller must ensure the array has sufficient size.
 * @param n The number of nodes to generate (must be >= 1).
 */
void chebyshevNodes(double nodes[], int n) {
    // --- Input Validation ---
     if (n < 1) {
        fprintf(stderr, "Warning [chebyshevNodes]: Called with n=%d (must be >= 1). No nodes generated.\n", n);
        return; // Do nothing if n is invalid
     }

    // --- Calculate Interval Properties ---
    double midpoint = 0.5 * (a + b);
    double half_range = 0.5 * (b - a);

    // --- Generate Nodes ---
    // Iterate k from 0 to n-1 to calculate each node.
    for (int k = 0; k < n; k++) {
        // Calculate the angle for the k-th Chebyshev root argument.
        // Using k directly (0 to n-1) in the formula (2k+1)PI/(2n).
        double angle = (2.0 * (double)k + 1.0) * PI / (2.0 * (double)n);

        // Calculate the Chebyshev root t_k in the interval [-1, 1].
        // Note: As k increases from 0 to n-1, angle goes from PI/(2n) towards (2n-1)PI/(2n),
        // so cos(angle) goes from nearly cos(0)=1 towards nearly cos(PI)=-1.
        // This generates nodes in descending order based on the t_k value.
        double t_k = cos(angle);

        // Map the root t_k from [-1, 1] to the interval [a, b].
        double node_val = midpoint + half_range * t_k;

        // Store the node value. Since t_k values are generated in descending order,
        // storing them in the array indexed by `n - 1 - k` results in an
        // array sorted in ascending order.
        // k=0 -> angle near 0, t_k near 1, node near b -> store at index n-1
        // k=n-1 -> angle near PI, t_k near -1, node near a -> store at index 0
        nodes[n - 1 - k] = node_val;
    }
    // Special case n=1: loop runs once with k=0. angle=PI/2, t_k=cos(PI/2)=0.
    // node_val = midpoint + 0 = (a+b)/2. Stored in nodes[1-1-0] = nodes[0]. Correct.
}