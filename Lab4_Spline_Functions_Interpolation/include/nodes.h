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
 * Requires n >= 2 for spline interpolation.
 */
void uniformNodes(double nodes[], int n);

/**
 * @brief Generates n Chebyshev nodes in the interval [a, b].
 * Requires n >= 2 for spline interpolation. Nodes are sorted ascending.
 */
void chebyshevNodes(double nodes[], int n);

#endif // NODES_H