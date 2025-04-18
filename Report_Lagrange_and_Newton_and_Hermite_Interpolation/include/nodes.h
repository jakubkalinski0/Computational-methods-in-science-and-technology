/**
* @file nodes.h
 * @brief Header file for interpolation node generation functions.
 *
 * Defines function prototypes for generating uniformly spaced nodes
 * and Chebyshev nodes within the specified interval [a, b].
 */
#ifndef NODES_H
#define NODES_H

#include "common.h"

/**
 * @brief Generates n uniformly spaced nodes in the interval [a, b].
 *
 * The nodes are distributed evenly across the interval.
 * Handles edge cases n < 1 and n = 1.
 *
 * @param nodes Output array where the generated node x-coordinates will be stored.
 * @param n The number of nodes to generate.
 */
void uniformNodes(double nodes[], int n);

/**
 * @brief Generates n Chebyshev nodes in the interval [a, b].
 *
 * These nodes correspond to the roots of the Chebyshev polynomial of the
 * first kind, scaled and shifted to the interval [a, b]. They are denser
 * near the ends of the interval. Handles edge case n < 1.
 *
 * @param nodes Output array where the generated node x-coordinates will be stored (in ascending order).
 * @param n The number of nodes to generate.
 */
void chebyshevNodes(double nodes[], int n);

#endif // NODES_H