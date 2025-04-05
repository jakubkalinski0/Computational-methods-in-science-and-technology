#include "../include/nodes.h"
#include <math.h>

// Generowanie węzłów równoodległych
void uniformNodes(double nodes[], int n) {
    double step = (b - a) / (n - 1);
    for (int i = 0; i < n; i++) {
        nodes[i] = a + i * step;
    }
}

// Generowanie węzłów Czebyszewa
void chebyshevNodes(double nodes[], int n) {
    for (int i = 0; i < n; i++) {
        // Współrzędne Czebyszewa w przedziale [-1, 1]
        double t = cos((2 * (i + 1) - 1) * PI / (2 * n));
        // Transformacja do przedziału [a, b]
        nodes[i] = 0.5 * (a + b) + 0.5 * (b - a) * t;
    }
}