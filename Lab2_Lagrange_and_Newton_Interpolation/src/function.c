#include "../include/function.h"
#include <math.h>

// Parametry funkcji
const double k = 4.0;
const double m = 0.4;
const double PI = 3.14159265358979323846;
const double a = -2.0 * PI * PI; // Początek przedziału
const double b = PI * PI;        // Koniec przedziału

// Funkcja do interpolacji
double f(double x) {
    return sin(k * x / PI) * exp(-m * x / PI);
}