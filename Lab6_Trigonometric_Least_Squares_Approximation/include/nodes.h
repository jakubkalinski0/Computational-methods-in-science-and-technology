/**
 * @file nodes.h
 * @brief Header file for functions generating sets of points (nodes) within an interval.
 *
 * Defines function prototypes for generating node distributions commonly used in
 * numerical analysis, specifically:
 * - Uniformly spaced nodes.
 * - Chebyshev nodes (roots of Chebyshev polynomials mapped to the interval).
 * These nodes are typically used as sample points for approximation or as interpolation nodes.
 * The interval [a, b] is defined by global constants from common.h.
 */
#ifndef NODES_H
#define NODES_H

#include "common.h" // Provides interval bounds a, b and PI

/**
 * @brief Generates `n` uniformly spaced nodes within the interval [a, b].
 *
 * The nodes are distributed evenly across the interval, including the endpoints.
 * The formula used is x_i = a + i * h, where h = (b - a) / (n - 1) for n > 1.
 * Handles the edge case n=1 by placing the single node at the midpoint (a+b)/2.
 * Prints a warning to stderr if n < 1.
 *
 * @param nodes Output array (size `n`) where the generated node x-coordinates will be stored.
 *              The caller must ensure the array has sufficient size.
 * @param n The number of nodes to generate. Must be >= 1.
 */
void uniformNodes(double nodes[], int n);

/**
 * @brief Generates `n` Chebyshev nodes within the interval [a, b].
 *
 * Chebyshev nodes are the roots of the Chebyshev polynomial of the first kind, T_n(t),
 * mapped from the interval [-1, 1] to [a, b]. They are known to be optimal for
 * minimizing the maximum error in polynomial interpolation (Runge's phenomenon).
 * The formula for the nodes in [-1, 1] is t_k = cos( (2k + 1) * PI / (2n) ) for k = 0, ..., n-1.
 * These are then mapped to [a, b] using x_k = 0.5*(a+b) + 0.5*(b-a)*t_k.
 * The resulting nodes are denser near the endpoints 'a' and 'b'.
 * Handles the edge case n < 1 by printing a warning to stderr.
 * The nodes are stored in the output array in ascending order.
 *
 * @param nodes Output array (size `n`) where the generated Chebyshev node x-coordinates
 *              will be stored in ascending order. The caller must ensure the array has sufficient size.
 * @param n The number of Chebyshev nodes to generate. Must be >= 1.
 */
void chebyshevNodes(double nodes[], int n);

#endif // NODES_H