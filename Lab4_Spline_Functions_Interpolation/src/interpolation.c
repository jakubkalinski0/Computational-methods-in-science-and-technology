/**
 * @file interpolation.c
 * @brief Implementation of cubic and quadratic spline interpolation algorithms.
 */
#include "../include/interpolation.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> // For memset if needed

// Forward declaration for the tridiagonal solver helper function
static bool solveTridiagonal(int n, double *a, double *b, double *c, double *r, double *x);

/**
 * @brief Finds the interval index 'i' such that nodes[i] <= x < nodes[i+1].
 * Assumes nodes are sorted. Handles x outside the interval [nodes[0], nodes[n-1]].
 * Uses linear search for simplicity. For large n, binary search would be faster.
 *
 * @param x The point to locate.
 * @param nodes Sorted array of node x-coordinates.
 * @param n Number of nodes.
 * @return Index 'i' of the interval, or 0 if x < nodes[1], or n-2 if x >= nodes[n-1].
 */
static int findInterval(double x, double nodes[], int n) {
    if (n < 2) return 0; // Need at least 2 nodes for an interval

    // Handle cases outside the main range
    if (x <= nodes[0]) return 0; // Point is at or before the first node
    if (x >= nodes[n-1]) return n - 2; // Point is at or after the last node

    // Linear search for the interval
    // Find i such that nodes[i] <= x < nodes[i+1]
    for (int i = 0; i < n - 1; ++i) {
        if (x >= nodes[i] && x < nodes[i+1]) {
            return i;
        }
    }
     // Should technically be covered by the x >= nodes[n-1] check,
     // but as a fallback if x is exactly nodes[n-1]
     return n - 2;
}


/**
 * @brief Performs cubic spline interpolation.
 * Calculates coefficients and evaluates the spline.
 * See header interpolation.h for parameter details.
 */
double cubicSplineInterpolation(double x, double nodes[], double values[], int n,
                                BoundaryConditionType bc_type, double deriv_a, double deriv_b)
{
    if (n < 2) {
        fprintf(stderr, "Error (Cubic Spline): Need at least 2 nodes (n=%d).\n", n);
        return NAN;
    }
     if (n > MAX_NODES) {
         fprintf(stderr, "Error (Cubic Spline): Too many nodes (%d > MAX_NODES=%d)\n", n, MAX_NODES);
         return NAN;
     }
     if (nodes == NULL || values == NULL) {
        fprintf(stderr, "Error (Cubic Spline): Nodes or values array is NULL.\n");
        return NAN;
     }


    // Step 1: Calculate interval lengths h_i
    double h[MAX_NODES]; // n-1 intervals, use MAX_NODES-1 for safety
    for (int i = 0; i < n - 1; ++i) {
        h[i] = nodes[i+1] - nodes[i];
        if (h[i] <= 0) {
            fprintf(stderr, "Error (Cubic Spline): Nodes must be strictly increasing (h[%d]=%g <= 0).\n", i, h[i]);
            return NAN;
        }
    }

    // Step 2: Set up the tridiagonal system Ax = B for second derivatives M_i (M_0 to M_{n-1})
    // System size is n x n.
    // 'a' = sub-diagonal, 'b' = main diagonal, 'c' = super-diagonal, 'r' = right-hand side
    double *diag_a = (double*)malloc((n) * sizeof(double)); // sub-diagonal (indices 1 to n-1)
    double *diag_b = (double*)malloc((n) * sizeof(double)); // main diagonal (indices 0 to n-1)
    double *diag_c = (double*)malloc((n) * sizeof(double)); // super-diagonal (indices 0 to n-2)
    double *rhs   = (double*)malloc((n) * sizeof(double)); // right-hand side B
    double *M     = (double*)malloc((n) * sizeof(double)); // Solution vector (second derivatives M_i)

    if (!diag_a || !diag_b || !diag_c || !rhs || !M) {
        fprintf(stderr, "Error (Cubic Spline): Memory allocation failed for tridiagonal system.\n");
        free(diag_a); free(diag_b); free(diag_c); free(rhs); free(M);
        return NAN;
    }

    // --- Fill the interior equations (row i corresponds to M_i, for i=1 to n-2) ---
    // Equation: h[i-1]*M[i-1] + 2*(h[i-1]+h[i])*M[i] + h[i]*M[i+1] = 6 * ((y[i+1]-y[i])/h[i] - (y[i]-y[i-1])/h[i-1])
    for (int i = 1; i < n - 1; ++i) {
        diag_a[i] = h[i-1]; // Coefficient of M[i-1]
        diag_b[i] = 2.0 * (h[i-1] + h[i]); // Coefficient of M[i]
        diag_c[i] = h[i];   // Coefficient of M[i+1]
        rhs[i] = 6.0 * ((values[i+1] - values[i]) / h[i] - (values[i] - values[i-1]) / h[i-1]);
    }

    // --- Apply Boundary Conditions (equations for M_0 and M_{n-1}) ---
    if (bc_type == BOUNDARY_NATURAL) {
        // Natural Spline: M_0 = 0, M_{n-1} = 0
        // Equation 0: 1*M_0 + 0*M_1 = 0
        diag_b[0] = 1.0;
        diag_c[0] = 0.0;
        rhs[0] = 0.0;
        // Equation n-1: 0*M_{n-2} + 1*M_{n-1} = 0
        diag_a[n-1] = 0.0;
        diag_b[n-1] = 1.0;
        rhs[n-1] = 0.0;

    } else if (bc_type == BOUNDARY_CLAMPED) {
         // Clamped Spline: Specify S'(a)=deriv_a, S'(b)=deriv_b
         // Equation 0: 2*h[0]*M_0 + h[0]*M_1 = 6 * ((y[1]-y[0])/h[0] - deriv_a)
         diag_b[0] = 2.0 * h[0];
         diag_c[0] = h[0];
         rhs[0] = 6.0 * ((values[1] - values[0]) / h[0] - deriv_a);
         // Equation n-1: h[n-2]*M_{n-2} + 2*h[n-2]*M_{n-1} = 6 * (deriv_b - (y[n-1]-y[n-2])/h[n-2])
         diag_a[n-1] = h[n-2];
         diag_b[n-1] = 2.0 * h[n-2];
         rhs[n-1] = 6.0 * (deriv_b - (values[n-1] - values[n-2]) / h[n-2]);

    } else {
        fprintf(stderr, "Error (Cubic Spline): Unsupported boundary condition type.\n");
        free(diag_a); free(diag_b); free(diag_c); free(rhs); free(M);
        return NAN;
    }

    // Step 3: Solve the tridiagonal system for M
    bool success = solveTridiagonal(n, diag_a, diag_b, diag_c, rhs, M);

    // Free temporary arrays used for solver setup
    free(diag_a); free(diag_b); free(diag_c); free(rhs);

    if (!success) {
        fprintf(stderr, "Error (Cubic Spline): Tridiagonal solver failed.\n");
        free(M);
        return NAN;
    }

    // Step 4: Evaluate the spline S(x) at the given point x
    int i = findInterval(x, nodes, n); // Find interval i such that nodes[i] <= x < nodes[i+1]

    // Coefficients for the cubic polynomial S_i(x) on [nodes[i], nodes[i+1]]
    // S_i(x) = a_i + b_i*(x-x_i) + c_i*(x-x_i)^2 + d_i*(x-x_i)^3
    // where x_i = nodes[i], y_i = values[i]
    double xi = nodes[i];
    double x_minus_xi = x - xi;
    double hi = h[i]; // Use precalculated h[i]

    // Coefficients in terms of y_i and M_i:
    double a_i = values[i];
    double b_i = (values[i+1] - values[i]) / hi - hi / 6.0 * (M[i+1] + 2.0 * M[i]);
    double c_i = M[i] / 2.0;
    double d_i = (M[i+1] - M[i]) / (6.0 * hi);

    // Evaluate the polynomial
    double result = a_i + b_i * x_minus_xi + c_i * x_minus_xi * x_minus_xi + d_i * x_minus_xi * x_minus_xi * x_minus_xi;

    // Free the solution vector M
    free(M);

    return result;
}


/**
 * @brief Performs quadratic spline interpolation.
 * Calculates coefficients and evaluates the spline.
 * We use the formulation where S_i(x_i)=y_i, S_i(x_{i+1})=y_{i+1}, and S'(x) is continuous.
 * This leads to needing one boundary condition, typically S'(a).
 * See header interpolation.h for parameter details.
 */
double quadraticSplineInterpolation(double x, double nodes[], double values[], int n,
                                    BoundaryConditionType bc_type, double deriv_a)
{
     if (n < 2) {
        fprintf(stderr, "Error (Quadratic Spline): Need at least 2 nodes (n=%d).\n", n);
        return NAN;
    }
     if (n > MAX_NODES) {
         fprintf(stderr, "Error (Quadratic Spline): Too many nodes (%d > MAX_NODES=%d)\n", n, MAX_NODES);
         return NAN;
     }
     if (nodes == NULL || values == NULL) {
        fprintf(stderr, "Error (Quadratic Spline): Nodes or values array is NULL.\n");
        return NAN;
     }

    // Step 1: Calculate interval lengths h_i
    double h[MAX_NODES]; // n-1 intervals
    for (int i = 0; i < n - 1; ++i) {
        h[i] = nodes[i+1] - nodes[i];
        if (h[i] <= 0) {
            fprintf(stderr, "Error (Quadratic Spline): Nodes must be strictly increasing (h[%d]=%g <= 0).\n", i, h[i]);
            return NAN;
        }
    }

    // Step 2: Determine the derivatives m_i = S'(x_i) at the nodes.
    // We have n unknowns (m_0 to m_{n-1}) and n-1 equations from derivative continuity:
    // S'_{i-1}(x_i) = S'_i(x_i)  =>  m_i + m_{i-1} = 2 * (y_i - y_{i-1}) / h_{i-1}  (for i=1 to n-1)
    // We need one boundary condition to solve this system.

    double *m = (double*)malloc(n * sizeof(double)); // Derivatives m_i
    if (!m) {
        fprintf(stderr, "Error (Quadratic Spline): Memory allocation failed for derivatives.\n");
        return NAN;
    }

    // Apply the boundary condition at the start (node 0)
    if (bc_type == BOUNDARY_CLAMPED) {
        m[0] = deriv_a; // Use provided f'(a)
    } else if (bc_type == BOUNDARY_ZERO_SLOPE_START) {
        m[0] = 0.0;     // Set S'(a) = 0
    } else {
        fprintf(stderr, "Error (Quadratic Spline): Unsupported boundary condition type for quadratic spline.\n");
        free(m);
        return NAN;
    }

    // Calculate remaining derivatives m_1 to m_{n-1} using the recurrence relation
    for (int i = 1; i < n; ++i) {
        // m_i = 2 * (y_i - y_{i-1}) / h_{i-1} - m_{i-1}
        if (h[i-1] == 0) { // Should have been caught earlier, but double-check
             fprintf(stderr, "Error (Quadratic Spline): Zero interval length h[%d].\n", i-1);
             free(m);
             return NAN;
        }
        m[i] = 2.0 * (values[i] - values[i-1]) / h[i-1] - m[i-1];
    }

    // Step 3: Evaluate the spline S(x) at the given point x
    int i = findInterval(x, nodes, n); // Find interval i such that nodes[i] <= x < nodes[i+1]

    // Coefficients for the quadratic polynomial S_i(x) on [nodes[i], nodes[i+1]]
    // S_i(x) = a_i + b_i*(x-x_i) + c_i*(x-x_i)^2
    // where x_i = nodes[i], y_i = values[i]
    double xi = nodes[i];
    double x_minus_xi = x - xi;
    double hi = h[i];

    // Coefficients derived from S_i(x_i) = y_i and S'_i(x_i) = m_i
    // and S_i(x_{i+1}) = y_{i+1}
    double a_i = values[i];
    double b_i = m[i];
    // From y_{i+1} = a_i + b_i*h_i + c_i*h_i^2
    double c_i = (values[i+1] - a_i - b_i * hi) / (hi * hi);
    // Alternative calculation for c_i using derivatives:
    // m_{i+1} = S'_i(x_{i+1}) = b_i + 2*c_i*h_i => c_i = (m[i+1] - m[i]) / (2.0 * hi);
    // Let's use the first formulation as it directly uses node values.

    // Evaluate the polynomial
    double result = a_i + b_i * x_minus_xi + c_i * x_minus_xi * x_minus_xi;

    // Free the derivatives array
    free(m);

    return result;
}


// --- Helper Function: Tridiagonal Solver (Thomas Algorithm) ---

/**
 * @brief Solves a tridiagonal linear system Ax = r using the Thomas algorithm.
 * Modifies the input arrays c and r during the process.
 *
 * @param n The size of the system (number of equations/unknowns).
 * @param a Pointer to the sub-diagonal array (a[0] is not used, indices 1 to n-1).
 * @param b Pointer to the main diagonal array (indices 0 to n-1).
 * @param c Pointer to the super-diagonal array (indices 0 to n-2).
 * @param r Pointer to the right-hand side vector (indices 0 to n-1). Will be modified.
 * @param x Pointer to the solution vector (output, indices 0 to n-1).
 * @return true if successful, false if division by zero occurs (matrix is singular or unstable).
 */
static bool solveTridiagonal(int n, double *a, double *b, double *c, double *r, double *x) {
    if (n <= 0) return false; // No system to solve

    // Allocate temporary storage for modified c' and r' coefficients if we don't want to modify inputs directly
    // Alternatively, modify c and r in place. Let's modify in place for efficiency.
    double *c_prime = c; // Use input 'c' array for c'
    double *r_prime = r; // Use input 'r' array for r'

    // Forward elimination
    if (fabs(b[0]) < 1e-15) { // Check for zero pivot
        fprintf(stderr, "Error (Tridiagonal Solver): Division by zero at forward stage (b[0]).\n");
        return false;
    }
    if (n > 1) { // Avoid accessing c_prime[0] if n=1
      c_prime[0] = c[0] / b[0];
    }
    r_prime[0] = r[0] / b[0];

    for (int i = 1; i < n; ++i) {
        double m = b[i] - a[i] * c_prime[i-1];
        if (fabs(m) < 1e-15) { // Check for zero pivot
             fprintf(stderr, "Error (Tridiagonal Solver): Division by zero at forward stage (i=%d).\n", i);
            return false;
        }
        if (i < n - 1) { // Avoid accessing c_prime[i] on last iteration
          c_prime[i] = c[i] / m;
        }
        r_prime[i] = (r[i] - a[i] * r_prime[i-1]) / m;
    }

    // Back substitution
    x[n-1] = r_prime[n-1];
    for (int i = n - 2; i >= 0; --i) {
        x[i] = r_prime[i] - c_prime[i] * x[i+1];
    }

    return true;
}