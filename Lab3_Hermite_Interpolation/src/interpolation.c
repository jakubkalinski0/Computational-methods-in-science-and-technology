#include "../include/interpolation.h"
#include <stdio.h> // Dodane dla fprintf w razie błędu
#include <math.h>  // Dodane dla fabs

// --- POPRAWIONA INTERPOLACJA HERMITE'A ---
double hermiteInterpolation(double x, double nodes[], double values[], double derivatives[], int n) {
    int m = 2 * n; // Liczba warunków interpolacyjnych (stopień wielomianu <= m-1)
    if (m == 0) return 0.0; // Jeśli nie ma węzłów
    if (m > MAX_NODES * 2) {
         fprintf(stderr, "Error: Too many nodes for Hermite interpolation buffer (%d > %d)\n", n, MAX_NODES);
         return NAN; // Not a Number
    }

    double z[MAX_NODES * 2];          // Tablica zdublowanych węzłów z_0, z_1, ..., z_{m-1}
    double fz[MAX_NODES * 2][MAX_NODES * 2]; // Tablica ilorazów różnicowych f[z_i, ..., z_{i+j}] = fz[i][j]

    // 1. Przygotowanie tablicy węzłów z[] i inicjalizacja zerowej kolumny fz[][0]
    for (int i = 0; i < n; i++) {
        z[2*i]     = nodes[i];
        z[2*i+1]   = nodes[i];
        fz[2*i][0]   = values[i]; // f[z_{2i}] = f(x_i)
        fz[2*i+1][0] = values[i]; // f[z_{2i+1}] = f(x_i)
    }

    // 2. Obliczenie tablicy ilorazów różnicowych fz[][] (kolumna po kolumnie)
    for (int j = 1; j < m; j++) { // j = rząd różnicy (kolumna)
        for (int i = 0; i < m - j; i++) { // i = indeks wiersza
            double denominator = z[i+j] - z[i];

            if (fabs(denominator) < 1e-15) {
                // Ten przypadek występuje, gdy z[i+j] == z[i].
                // Dla j=1 oznacza to z[i+1]==z[i], co jest prawdą dla i = 2k.
                // Wtedy iloraz różnicowy f[z_{2k}, z_{2k+1}] jest z definicji równy f'(x_k).
                if (j == 1 && (i % 2 == 0)) {
                    fz[i][j] = derivatives[i / 2]; // f[z_{2k}, z_{2k+1}] = f'(x_k)
                } else {
                    // Ten przypadek (dzielenie przez zero dla j > 1 lub j=1, i nieparzyste)
                    // nie powinien wystąpić przy standardowej interpolacji Hermite'a z różnymi węzłami x_i
                    // i użyciem tylko pierwszej pochodnej. Może wskazywać na błąd lub problem numeryczny.
                    fprintf(stderr, "Warning: Unexpected zero denominator in Hermite divided difference at i=%d, j=%d. z[%d]=%g, z[%d]=%g\n", i, j, i+j, z[i+j], i, z[i]);
                    fz[i][j] = 0.0; // Lub NAN - może to prowadzić do błędnego wyniku
                }
            } else {
                // Standardowa formuła ilorazu różnicowego
                fz[i][j] = (fz[i+1][j-1] - fz[i][j-1]) / denominator;
            }
        }
    }

    // 3. Obliczenie wartości wielomianu Hermite'a w postaci Newtona
    // H(x) = f[z_0] + f[z_0,z_1](x-z_0) + f[z_0,z_1,z_2](x-z_0)(x-z_1) + ...
    // Współczynniki to górny wiersz tabeli ilorazów: fz[0][0], fz[0][1], fz[0][2], ...
    double result = fz[0][0];      // Pierwszy składnik f[z_0]
    double product_term = 1.0;   // Będzie przechowywać (x-z_0)...(x-z_{k-1})

    for (int k = 1; k < m; k++) {
        product_term *= (x - z[k-1]);       // Aktualizuj iloczyn (x-z_0)...(x-z_{k-1})
        result += fz[0][k] * product_term; // Dodaj składnik f[z_0,...,z_k] * (iloczyn)
    }

    return result;
}
// --- KONIEC POPRAWIONEJ INTERPOLACJI HERMITE'A ---