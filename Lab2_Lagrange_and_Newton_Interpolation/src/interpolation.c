#include "../include/interpolation.h"
#include <stdio.h> // Dodane dla fprintf w razie błędu

// Interpolacja Lagrange'a
double lagrangeInterpolation(double x, double nodes[], double values[], int n) {
    double result = 0.0;
    
    for (int i = 0; i < n; i++) {
        double Li = 1.0;
        for (int j = 0; j < n; j++) {
            if (i != j) {
                Li *= (x - nodes[j]) / (nodes[i] - nodes[j]);
            }
        }
        result += values[i] * Li;
    }
    
    return result;
}

// Interpolacja Newtona
double newtonInterpolation(double x, double nodes[], double values[], int n) {
    // Tablica dla ilorazów różnicowych
    double divDiff[MAX_NODES][MAX_NODES];
    
    // Inicjalizacja pierwszej kolumny
    for (int i = 0; i < n; i++) {
        divDiff[i][0] = values[i];
    }
    
    // Obliczanie ilorazów różnicowych
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            divDiff[i][j] = (divDiff[i+1][j-1] - divDiff[i][j-1]) / (nodes[i+j] - nodes[i]);
        }
    }
    
    // Obliczanie wielomianu Newtona
    double result = divDiff[0][0];
    double prod = 1.0;
    
    for (int i = 1; i < n; i++) {
        prod *= (x - nodes[i-1]);
        result += divDiff[0][i] * prod;
    }
    
    return result;
}