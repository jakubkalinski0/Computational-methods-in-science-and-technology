#include "../include/common.h"
#include "../include/function.h"
#include "../include/nodes.h"
#include "../include/interpolation.h"
#include "../include/error.h"
#include "../include/fileio.h"
#include <stdio.h>

int main() {
    int maxNodes;
    printf("Podaj maksymalną liczbę węzłów (1-%d): ", MAX_NODES);
    scanf("%d", &maxNodes);

    if (maxNodes < 1 || maxNodes > MAX_NODES) {
        printf("Nieprawidłowa liczba węzłów. Powinna być w zakresie 1-%d\n", MAX_NODES);
        return 1;
    }

    // Liczba punktów do rysowania funkcji
    const int numPoints = 1000;
    double x[numPoints];
    double y_true[numPoints];

    // Stworzenie gęstej siatki punktów do wizualizacji funkcji
    double step = (b - a) / (numPoints - 1);
    for (int i = 0; i < numPoints; i++) {
        x[i] = a + i * step;
        y_true[i] = f(x[i]);
    }

    // Zapisanie oryginalnej funkcji do pliku
    saveDataToFile("original_function.dat", x, y_true, numPoints);

    // Tablice do przechowywania błędów (max error)
    double lagrange_uniform_errors[maxNodes];
    double lagrange_chebyshev_errors[maxNodes];
    double newton_uniform_errors[maxNodes];
    double newton_chebyshev_errors[maxNodes];
    double hermite_uniform_errors[maxNodes];
    double hermite_chebyshev_errors[maxNodes];

    // Tablice do przechowywania średnich błędów kwadratowych
    double lagrange_uniform_mse[maxNodes];
    double lagrange_chebyshev_mse[maxNodes];
    double newton_uniform_mse[maxNodes];
    double newton_chebyshev_mse[maxNodes];
    double hermite_uniform_mse[maxNodes];
    double hermite_chebyshev_mse[maxNodes];

    // Tablice do przechowywania węzłów i wartości
    double nodes[MAX_NODES];
    double values[MAX_NODES];
    double derivatives[MAX_NODES];

    // Tablica na wyniki interpolacji
    double y_interp[numPoints];

    printf("\nAnaliza interpolacji dla funkcji f(x) = sin(%.1fx/π) · e^(-%.1fx/π) w przedziale [%.2f, %.2f]\n",
           k, m, a, b);
    printf("=========================================================================\n");

    // Iteracja po różnych liczbach węzłów (od 1 do maxNodes)
    for (int n = 1; n <= maxNodes; n++) {
        char filename[100];

        printf("\nLiczba węzłów: %d\n", n);
        printf("--------------------------\n");

        // 1. Węzły równoodległe
        uniformNodes(nodes, n);

        // Obliczenie wartości funkcji
        for (int j = 0; j < n; j++) {
            values[j] = f(nodes[j]);
        }

        // Zapisanie węzłów do pliku
        sprintf(filename, "uniform_nodes_n%d.dat", n);
        saveNodesToFile(filename, nodes, values, n);

        // Interpolacja Lagrange'a dla węzłów równoodległych
        for (int j = 0; j < numPoints; j++) {
            y_interp[j] = lagrangeInterpolation(x[j], nodes, values, n);
        }
        sprintf(filename, "lagrange_uniform_n%d.dat", n);
        saveDataToFile(filename, x, y_interp, numPoints);
        ErrorResult error = calculateError(y_true, y_interp, numPoints);
        lagrange_uniform_errors[n-1] = error.max_error;
        lagrange_uniform_mse[n-1] = error.mean_squared_error;

        // Interpolacja Newtona dla węzłów równoodległych
        for (int j = 0; j < numPoints; j++) {
            y_interp[j] = newtonInterpolation(x[j], nodes, values, n);
        }
        sprintf(filename, "newton_uniform_n%d.dat", n);
        saveDataToFile(filename, x, y_interp, numPoints);
        error = calculateError(y_true, y_interp, numPoints);
        newton_uniform_errors[n-1] = error.max_error;
        newton_uniform_mse[n-1] = error.mean_squared_error;

        // 2. Węzły Czebyszewa
        chebyshevNodes(nodes, n);

        // Obliczenie wartości funkcji i pochodnych w węzłach
        for (int j = 0; j < n; j++) {
            values[j] = f(nodes[j]);
        }

        // Zapisanie węzłów do pliku
        sprintf(filename, "chebyshev_nodes_n%d.dat", n);
        saveNodesToFile(filename, nodes, values, n);

        // Interpolacja Lagrange'a dla węzłów Czebyszewa
        for (int j = 0; j < numPoints; j++) {
            y_interp[j] = lagrangeInterpolation(x[j], nodes, values, n);
        }
        sprintf(filename, "lagrange_chebyshev_n%d.dat", n);
        saveDataToFile(filename, x, y_interp, numPoints);
        error = calculateError(y_true, y_interp, numPoints);
        lagrange_chebyshev_errors[n-1] = error.max_error;
        lagrange_chebyshev_mse[n-1] = error.mean_squared_error;

        // Interpolacja Newtona dla węzłów Czebyszewa
        for (int j = 0; j < numPoints; j++) {
            y_interp[j] = newtonInterpolation(x[j], nodes, values, n);
        }
        sprintf(filename, "newton_chebyshev_n%d.dat", n);
        saveDataToFile(filename, x, y_interp, numPoints);
        error = calculateError(y_true, y_interp, numPoints);
        newton_chebyshev_errors[n-1] = error.max_error;
        newton_chebyshev_mse[n-1] = error.mean_squared_error;

        // Wyświetlenie błędów
        printf("Błędy maksymalne:\n");
        printf("  Lagrange (równoodległe): %.3e\n", lagrange_uniform_errors[n-1]);
        printf("  Lagrange (Czebyszewa):   %.3e\n", lagrange_chebyshev_errors[n-1]);
        printf("  Newton (równoodległe):   %.3e\n", newton_uniform_errors[n-1]);
        printf("  Newton (Czebyszewa):     %.3e\n", newton_chebyshev_errors[n-1]);

        printf("\nŚrednie błędy kwadratowe:\n");
        printf("  Lagrange (równoodległe): %.3e\n", lagrange_uniform_mse[n-1]);
        printf("  Lagrange (Czebyszewa):   %.3e\n", lagrange_chebyshev_mse[n-1]);
        printf("  Newton (równoodległe):   %.3e\n", newton_uniform_mse[n-1]);
        printf("  Newton (Czebyszewa):     %.3e\n", newton_chebyshev_mse[n-1]);

        // Sprawdzenie efektu Runge'go
        if (n >= 10 && lagrange_uniform_errors[n-1] > 10.0 * lagrange_chebyshev_errors[n-1]) {
            printf("\nUWAGA: Wykryto efekt Runge'go dla n=%d (błąd dla węzłów równoodległych jest ponad 10x większy)\n", n);
        }
    }

    // Zapisanie błędów do pliku
    // Zapisanie błędów do osobnych plików CSV
    saveLagrangeUniformErrorsToFile(maxNodes, lagrange_uniform_errors, lagrange_uniform_mse);
    saveLagrangeChebyshevErrorsToFile(maxNodes, lagrange_chebyshev_errors, lagrange_chebyshev_mse);
    saveNewtonUniformErrorsToFile(maxNodes, newton_uniform_errors, newton_uniform_mse);
    saveNewtonChebyshevErrorsToFile(maxNodes, newton_chebyshev_errors, newton_chebyshev_mse);

    // Generowanie skryptów gnuplot
    generateGnuplotScript(maxNodes,
                         lagrange_uniform_errors, lagrange_chebyshev_errors,
                         newton_uniform_errors, newton_chebyshev_errors);

    generateErrorPlotScript(maxNodes,
                          lagrange_uniform_errors, lagrange_chebyshev_errors,
                          newton_uniform_errors, newton_chebyshev_errors);

    printf("\n=========================================================================\n");
    printf("Wyniki zostały zapisane do plików w folderze data/.\n");
    printf("Aby wygenerować wykresy, wykonaj polecenia:\n");
    printf("  gnuplot plot_interpolation.gp\n");
    printf("  gnuplot plot_errors.gp\n");
    printf("Wygenerowane wykresy zostaną zapisane w folderze plots/.\n");

    return 0;
}