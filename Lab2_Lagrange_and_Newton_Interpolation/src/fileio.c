#include "../include/fileio.h"
#include <stdio.h>
#include <stdlib.h>

// Zapisywanie danych do pliku do wizualizacji
void saveDataToFile(const char* filename, double x[], double y[], int n) {
    char filepath[256];
    sprintf(filepath, "data/%s", filename);

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        printf("Błąd otwarcia pliku: %s\n", filepath);
        return;
    }

    for (int i = 0; i < n; i++) {
        fprintf(file, "%lf %lf\n", x[i], y[i]);
    }

    fclose(file);
}

// Zapisywanie węzłów do pliku
void saveNodesToFile(const char* filename, double nodes[], double values[], int n) {
    char filepath[256];
    sprintf(filepath, "data/%s", filename);

    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        printf("Błąd otwarcia pliku: %s\n", filepath);
        return;
    }

    for (int i = 0; i < n; i++) {
        fprintf(file, "%lf %lf\n", nodes[i], values[i]);
    }

    fclose(file);
}

void generateErrorPlotScript(int maxNodes,
                           double lagrange_uniform_errors[],
                           double lagrange_chebyshev_errors[],
                           double newton_uniform_errors[],
                           double newton_chebyshev_errors[]) {
    FILE *gnuplot_script = fopen("plot_errors.gp", "w");
    if (gnuplot_script == NULL) {
        printf("Nie można otworzyć pliku plot_errors.gp do zapisu\n");
        return;
    }

    // Podstawowe ustawienia wykresu
    fprintf(gnuplot_script, "set terminal pngcairo enhanced size 1200,800 font 'Arial,12'\n");
    fprintf(gnuplot_script, "set output 'plots/interpolation_errors.png'\n");
    fprintf(gnuplot_script, "set title 'Porównanie błędów interpolacji'\n");
    fprintf(gnuplot_script, "set xlabel 'Liczba węzłów'\n");
    fprintf(gnuplot_script, "set ylabel 'Błąd maksymalny'\n");
    fprintf(gnuplot_script, "set grid\n");
    fprintf(gnuplot_script, "set key top right\n");
    fprintf(gnuplot_script, "set logscale y\n");
//    fprintf(gnuplot_script, "set yrange [1e-10:1e10]\n"); // Przykładowy zakres, dostosuj jeśli trzeba

    // Upewnij się, że katalog plots istnieje
    fprintf(gnuplot_script, "!mkdir -p plots\n");

    // Dane do wykresu
    fprintf(gnuplot_script, "plot '-' using 1:2 with linespoints title 'Lagrange (równoodległe)', \\\n");
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints title 'Lagrange (Czebyszew)', \\\n");
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints title 'Newton (równoodległe)', \\\n");
    fprintf(gnuplot_script, "     '-' using 1:2 with linespoints title 'Newton (Czebyszew)', \n");

    // Zapisz dane dla każdej metody bezpośrednio w skrypcie
    for (int i = 0; i < maxNodes; i++) {
        fprintf(gnuplot_script, "%d %e\n", i+1, lagrange_uniform_errors[i]);
    }
    fprintf(gnuplot_script, "e\n");

    for (int i = 0; i < maxNodes; i++) {
        fprintf(gnuplot_script, "%d %e\n", i+1, lagrange_chebyshev_errors[i]);
    }
    fprintf(gnuplot_script, "e\n");

    for (int i = 0; i < maxNodes; i++) {
        fprintf(gnuplot_script, "%d %e\n", i+1, newton_uniform_errors[i]);
    }
    fprintf(gnuplot_script, "e\n");

    for (int i = 0; i < maxNodes; i++) {
        fprintf(gnuplot_script, "%d %e\n", i+1, newton_chebyshev_errors[i]);
    }
    fprintf(gnuplot_script, "e\n");

    fclose(gnuplot_script);
}


// Nowe funkcje do zapisu błędów do osobnych plików CSV
void saveLagrangeUniformErrorsToFile(int maxNodes, double errors[], double mse[]) {
    FILE *file = fopen("data/lagrange_uniform_errors.csv", "w");
    if (file != NULL) {
        fprintf(file, "Liczba wezlow,Blad Maksymalny,Sredni blad kwadratowy\n");
        for (int i = 0; i < maxNodes; i++) {
            fprintf(file, "%d,%.10e,%.10e\n", i+1, errors[i], mse[i]);
        }
        fclose(file);
    }
}

void saveLagrangeChebyshevErrorsToFile(int maxNodes, double errors[], double mse[]) {
    FILE *file = fopen("data/lagrange_chebyshev_errors.csv", "w");
    if (file != NULL) {
        fprintf(file, "Liczba wezlow,Blad Maksymalny,Sredni blad kwadratowy\n");
        for (int i = 0; i < maxNodes; i++) {
            fprintf(file, "%d,%.10e,%.10e\n", i+1, errors[i], mse[i]);
        }
        fclose(file);
    }
}

void saveNewtonUniformErrorsToFile(int maxNodes, double errors[], double mse[]) {
    FILE *file = fopen("data/newton_uniform_errors.csv", "w");
    if (file != NULL) {
        fprintf(file, "Liczba wezlow,Blad Maksymalny,Sredni blad kwadratowy\n");
        for (int i = 0; i < maxNodes; i++) {
            fprintf(file, "%d,%.10e,%.10e\n", i+1, errors[i], mse[i]);
        }
        fclose(file);
    }
}

void saveNewtonChebyshevErrorsToFile(int maxNodes, double errors[], double mse[]) {
    FILE *file = fopen("data/newton_chebyshev_errors.csv", "w");
    if (file != NULL) {
        fprintf(file, "Liczba wezlow,Blad Maksymalny,Sredni blad kwadratowy\n");
        for (int i = 0; i < maxNodes; i++) {
            fprintf(file, "%d,%.10e,%.10e\n", i+1, errors[i], mse[i]);
        }
        fclose(file);
    }
}

// Generowanie skryptu gnuplot dla wykresów interpolacji
void generateGnuplotScript(int maxNodes,
                         double lagrange_uniform_errors[],
                         double lagrange_chebyshev_errors[],
                         double newton_uniform_errors[],
                         double newton_chebyshev_errors[]) {

    FILE *gnuplot_script = fopen("plot_interpolation.gp", "w");
    if (gnuplot_script != NULL) {
        // Wspólne ustawienia dla wszystkich wykresów
        fprintf(gnuplot_script, "set terminal png size 1200,800\n");
        fprintf(gnuplot_script, "set grid\n");
        fprintf(gnuplot_script, "set key outside\n");
        fprintf(gnuplot_script, "set xlabel 'x'\n");
        fprintf(gnuplot_script, "set ylabel 'f(x)'\n");
        fprintf(gnuplot_script, "set xrange [-20:10]\n");
        fprintf(gnuplot_script, "set yrange [-15:15]\n");
        fprintf(gnuplot_script, "system 'mkdir -p plots data'\n");

        // Indywidualne wykresy dla każdej metody z węzłami
        fprintf(gnuplot_script, "# Wykresy funkcji interpolacyjnych z węzłami\n");

        // Iteracja po wszystkich liczbach węzłów
        for (int n = 1; n <= maxNodes; n++) {
            // Interpolacja Lagrange'a z węzłami równoodległymi
            fprintf(gnuplot_script, "set output 'plots/lagrange_uniform_with_nodes_n%d.png'\n", n);
            fprintf(gnuplot_script, "set title \"Interpolacja Lagrange'a (n=%d, węzły równoodległe)\"\n", n);
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 2 lw 3 lc rgb 'blue' title 'Funkcja oryginalna',\\\n");
            fprintf(gnuplot_script, "     'data/lagrange_uniform_n%d.dat' with lines lw 3 lc rgb 'red' title 'Interpolacja Lagrange''a',\\\n", n);
            fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Węzły interpolacji'\n", n);

            // Interpolacja Lagrange'a z węzłami Czebyszewa
            fprintf(gnuplot_script, "set output 'plots/lagrange_chebyshev_with_nodes_n%d.png'\n", n);
            fprintf(gnuplot_script, "set title \"Interpolacja Lagrange'a (n=%d, węzły Czebyszewa)\"\n", n);
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 2 lw 3 lc rgb 'blue' title 'Funkcja oryginalna',\\\n");
            fprintf(gnuplot_script, "     'data/lagrange_chebyshev_n%d.dat' with lines lw 3 lc rgb 'red' title 'Interpolacja Lagrange''a',\\\n", n);
            fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Węzły interpolacji'\n", n);

            // Interpolacja Newtona z węzłami równoodległymi
            fprintf(gnuplot_script, "set output 'plots/newton_uniform_with_nodes_n%d.png'\n", n);
            fprintf(gnuplot_script, "set title \"Interpolacja Newtona (n=%d, węzły równoodległe)\"\n", n);
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 2 lw 3 lc rgb 'blue' title 'Funkcja oryginalna',\\\n");
            fprintf(gnuplot_script, "     'data/newton_uniform_n%d.dat' with lines lw 3 lc rgb 'red' title 'Interpolacja Newtona',\\\n", n);
            fprintf(gnuplot_script, "     'data/uniform_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Węzły interpolacji'\n", n);

            // Interpolacja Newtona z węzłami Czebyszewa
            fprintf(gnuplot_script, "set output 'plots/newton_chebyshev_with_nodes_n%d.png'\n", n);
            fprintf(gnuplot_script, "set title \"Interpolacja Newtona (n=%d, węzły Czebyszewa)\"\n", n);
            fprintf(gnuplot_script, "plot 'data/original_function.dat' with lines dashtype 2 lw 3 lc rgb 'blue' title 'Funkcja oryginalna',\\\n");
            fprintf(gnuplot_script, "     'data/newton_chebyshev_n%d.dat' with lines lw 3 lc rgb 'red' title 'Interpolacja Newtona',\\\n", n);
            fprintf(gnuplot_script, "     'data/chebyshev_nodes_n%d.dat' with points pt 7 ps 1.5 lc rgb 'black' title 'Węzły interpolacji'\n", n);
        }

        fclose(gnuplot_script);
        printf("\nUtworzono skrypt gnuplot: plot_interpolation.gp\n");
    }
}