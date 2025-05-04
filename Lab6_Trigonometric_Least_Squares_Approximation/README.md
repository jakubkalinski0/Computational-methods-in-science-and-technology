# Lab 5: Least Squares Polynomial Approximation Analysis

## Overview

This project implements and analyzes **least-squares polynomial approximation**. The goal is to approximate a given function, $f(x) = \sin(k \cdot x / \pi) \cdot \exp(-m \cdot x / \pi)$, over the interval $[a, b]$. The least-squares method finds the polynomial $P_m(x)$ of a specified degree $m$ that minimizes the sum of squared differences between the polynomial's values and the true function values at a set of discrete sample points $(x_i, y_i)$.

The analysis investigates the impact of:
1.  **Number of Sample Points ($n$):** Observing how the accuracy of the approximation changes as the number of points used for fitting increases. Uniformly spaced points are used for sampling in this implementation.
2.  **Polynomial Degree ($m$):** Examining how the approximation accuracy varies with the degree of the fitting polynomial, ensuring $m < n$.

The project calculates the polynomial coefficients by setting up and solving the **normal equations** (derived from the least-squares minimization problem). It measures the approximation accuracy using maximum absolute error and mean squared error (MSE), generates data for various $(n, m)$ combinations, and visualizes the results using both individual comparison plots (via Gnuplot) and error heatmaps (via Python) to understand the interplay between $n$, $m$, accuracy, and numerical stability.

## Problem Description (Original Polish Task)

*(The following is the task description extracted from the provided report PDF)*

> **1 Treść i przebieg zadania**
>
> **1.1 Cel ćwiczenia**
>
> Celem ćwiczenia było wyznaczenie przybliżenia zadanej funkcji, wykorzystując **aproksymację średnio-kwadratową wielomianami algebraicznymi** oraz analizę dokładności tego przybliżenia w zależności od liczby punktów dyskretyzacji ($n$) i stopnia wielomianu aproksymującego ($m$).
>
> **1.2 Zadana funkcja i przedział**
>
> Analizowaną funkcją jest:
> $$f(x) = \sin \left( \frac{kx}{\pi} \right) e^{-\frac{mx}{\pi}} \quad \text{gdzie } k = 4.0, \text{ oraz } m = 0.4$$
> Funkcja ta była badana w przedziale $[a, b]$, gdzie $a = -2\pi^2$ oraz $b = \pi^2$. Wartości stałych $k, m, a, b$ oraz $\pi$ są zdefiniowane w pliku `function.c` i dostępne globalnie w projekcie poprzez `common.h`.
>
> **1.3 Przebieg ćwiczenia**
>
> Ćwiczenie obejmowało następujące kroki zrealizowane w środowisku C, z wykorzystaniem skryptu Pythona i Gnuplot do wizualizacji:
>
> 1.  **Implementacja podstawowych modułów (C):** Stworzono funkcje do obliczania wartości funkcji $f(x)$ (`function.c`), generowania węzłów (`nodes.c`), obliczania błędów (`error.c`), operacji wejścia/wyjścia (`fileio.c`) oraz rozwiązywania układów równań liniowych (`linear_algebra.c`).
> 2.  **Dyskretyzacja funkcji (C):** Dla różnych wartości $n$ (liczby punktów dyskretyzacji), od $n = 2$ do zadanej przez użytkownika wartości $N_{max}$, wyznaczono $n$ punktów $(x_i, y_i)$ w przedziale $[a, b]$, gdzie $y_i = f(x_i)$. Zgodnie z implementacją (`nodes.c`, `main.c`), zastosowano **równomierne rozmieszczenie** punktów $x_i$ (generowane przez funkcję `uniformNodes`). Wygenerowane punkty $(x_i, y_i)$ dla każdego $n$ zapisano do osobnych plików `.dat` w katalogu `data/`.
> 3.  **Aproksymacja średniokwadratowa (C):** Dla każdej liczby punktów $n$ oraz dla różnych stopni wielomianu $m$ (gdzie $0 \le m < n$ i $m \le M_{max}$ zadanego przez użytkownika), wyznaczono wielomian algebraiczny $P_m(x) = a_0 + a_1 x + \dots + a_m x^m$, który minimalizuje sumę kwadratów błędów w punktach dyskretyzacji:
      >     $$ S(a_0, \dots, a_m) = \sum_{i=0}^{n-1} (P_m(x_i) - y_i)^2 \rightarrow \min $$

*(**Translation Summary:** The goal is to approximate the function $f(x) = \sin(kx/\pi) \exp(-mx/\pi)$ (with $k=4, m=0.4$) on the interval $[-2\pi^2, \pi^2]$ using least-squares approximation with algebraic polynomials. The analysis focuses on the accuracy depending on the number of discretization points ($n$) and the polynomial degree ($m$). The process involves implementing C modules for the function, node generation (uniform), error calculation, I/O, and solving linear systems. The function is discretized using $n$ uniformly spaced points. For each $n$ and various valid degrees $m$ ($0 \le m < n$), the algebraic polynomial $P_m(x)$ minimizing the sum of squared errors $S$ at the discretization points is determined. Visualization uses Python and Gnuplot.)*

## Key Features

*   Implements **least-squares polynomial approximation** by setting up and solving the **normal equations** (using Gaussian elimination with pivoting).
*   Includes the function $f(x) = \sin(k \cdot x / \pi) \cdot \exp(-m \cdot x / \pi)$ and its first derivative `df(x)`.
*   Generates **uniformly spaced sample points** within the interval $[-2\pi^2, \pi^2]$ ($a$ to $b$) for fitting.
*   Iterates through user-defined ranges of **sample points ($n$)** and **polynomial degrees ($m$)**.
*   Calculates **Maximum Absolute Error** and **Mean Squared Error (MSE)** between the true function $f(x)$ and the approximating polynomial $P_m(x)$ on a dense grid.
*   Saves detailed results to data files:
    *   Original function points for plotting (`.dat`).
    *   Sample points $(x_i, y_i)$ used for fitting for each $n$ (`.dat`).
    *   Approximated polynomial points for each $(n, m)$ configuration (`.dat`).
    *   Error summary (`N, M, MaxError, MSE`) for **heatmap generation** (`.csv`).
*   Automatically generates a **single Gnuplot script** (`.gp`) to visualize **individual plots** comparing $f(x)$, $P_m(x)$, and sample points for each valid $(n, m)$ pair.
*   Includes a **Python script** (`plot_heatmaps.py`) using pandas, matplotlib, and seaborn to generate **heatmap plots** visualizing error trends (Max Error/MSE vs. $n$ and $m$) from the CSV data.
*   Provides a **Makefile** for easy compilation, execution, data generation, plotting (Python and Gnuplot), and cleanup.

## 📁 Project Structure
- 📁 **Lab5_Least_Squares_Polynomial_Approximation/** – Main
    - 📁 **include/** – Header files (.h)
        - 📄 `common.h`
        - 📄 `error.h`
        - 📄 `fileio.h`
        - 📄 `function.h`
        - 📄 `approximation.h`
        - 📄 `linear_algebra.h`
        - 📄 `nodes.h`
    - 📁 **src/** – Source code files (.c)
        - 📄 `error.c`
        - 📄 `fileio.c`
        - 📄 `function.c`
        - 📄 `approximation.c`
        - 📄 `linear_algebra.c`
        - 📄 `main.c`
        - 📄 `nodes.c`
        - 📄 `plot_heatmaps.py`
    - 📁 **bin/** – Compiled executable
    - 📁 **obj/** – Object files (.o)
    - 📁 **data/** – Output data files (.dat, .csv)
    - 📁 **scripts/** – Gnuplot scripts (.gp)
    - 📁 **plots/** – Plot images (.png)
    - 🛠️ **Makefile**
    - 📘 **README.md** (This file)
    - 📄 **Report_Lab5_Least_Squares_Polynomial_Approximation.pdf** – Detailed analysis (in Polish)

## Requirements

*   **C Compiler:** A C compiler supporting C11 or later (e.g., GCC, Clang).
*   **Make:** The `make` build automation tool.
*   **Gnuplot:** The Gnuplot utility for generating the individual comparison plots.
*   **Python 3:** Required for generating heatmap plots.
    *   **Python Libraries:** pandas, matplotlib, seaborn, numpy. Install using pip:
        ```bash
        pip install pandas matplotlib seaborn numpy
        # Or, if using a virtual environment:
        # python -m venv .venv
        # source .venv/bin/activate  # On Linux/macOS
        # .\.venv\Scripts\activate  # On Windows
        # pip install pandas matplotlib seaborn numpy
        ```

## Building and Running

The project uses a Makefile for easy management. Open your terminal in the project's root directory (`Lab5_.../`) and use the following commands:

1.  **Compile the C program:**
    ```bash
    make all
    # or simply:
    make
    ```
    This compiles the C source code and creates the executable `bin/approximation_app`.

2.  **Compile and Run C code (Generate Data & Gnuplot Script):**
    ```bash
    make run
    ```
    This compiles (if needed) and runs the C program (`bin/approximation_app`). You will be prompted to enter the maximum number of sample points ($N_{max}$) and the maximum polynomial degree ($M_{max}$). This command generates:
    *   The heatmap data file (`data/approximation_heatmap_errors.csv`).
    *   `.dat` files for the original function, sample points, and approximated polynomial values in `data/`.
    *   The Gnuplot script (`scripts/plot_all_approximations.gp`) for individual plots.

3.  **Generate Heatmap Plots (Python):**
    ```bash
    make py_plots
    ```
    This executes the Python script `src/plot_heatmaps.py`, which reads the CSV data (requires `make run` first) and generates heatmap images (e.g., PDF, SVG, or PNG depending on the script's configuration) in the `plots/` directory.

4.  **Generate Individual Plots (Gnuplot):**
    ```bash
    make generate_individual_plots
    ```
    This executes the Gnuplot script `scripts/plot_all_approximations.gp` (requires `make run` first) to generate numerous individual comparison PNG plots in the `plots/` directory.

5.  **Compile, Run C, Generate All Plots (Python & Gnuplot):**
    ```bash
    make plots
    ```
    This is the most common command. It performs `make run` (compiles C, runs C to generate data and the Gnuplot script), then `make py_plots` (runs Python for heatmaps), and finally `make generate_individual_plots` (runs Gnuplot for individual plots).

6.  **Display Help:**
    ```bash
    make help
    ```
    Shows a summary of available Makefile commands.

## Output Files

After running `make plots`, you will find the following generated files:

*   **`data/` Directory:**
    *   `approximation_heatmap_errors.csv`: Error data (N, M, MaxError, MSE) for heatmap generation.
    *   `original_function_plot.dat`: Points representing the true function $f(x)$.
    *   `sample_points_n*.dat`: Coordinates of uniform sample points $(x_i, y_i)$ for each $n$.
    *   `approximation_degree*_points*.dat`: Points representing the approximated polynomial $P_m(x)$ for each $(n, m)$.
*   **`scripts/` Directory:**
    *   `plot_all_approximations.gp`: Gnuplot script to generate all individual approximation plots.
*   **`plots/` Directory:**
    *   `py_*.pdf`, `py_*.svg`, `py_*.png`: Heatmap plots generated by the Python script (actual extension depends on `OUTPUT_FORMAT` in `plot_heatmaps.py`). Includes standard and potentially annotated versions.
    *   `approximation_m*_n*.png`: Individual plots comparing $f(x)$, $P_m(x)$, and sample points for each $(n, m)$, generated by Gnuplot.

## Analysis and Interpretation

A detailed analysis of the results, discussing the influence of $n$ and $m$ on approximation accuracy, identifying optimal parameter ranges, analyzing the effects of ill-conditioning of the Gram matrix (especially for $m$ close to $n$), interpreting the heatmaps, and examining specific cases (like sampling near function zeros), can be found in the accompanying report file:

**`Report_Lab5_Least_Squares_Polynomial_Approximation.pdf`**

Please note that the analysis within that document is written in **Polish**.

## Cleanup

*   **Remove generated files (but keep directories):**
    ```bash
    make clean
    ```
    This removes object files (`*.o`), the executable, data files (`*.dat`, `*.csv`), the Gnuplot script (`*.gp`), and plot images (`*.png`, `*.pdf`, `*.svg`, etc.).

*   **Remove generated files AND directories:**
    ```bash
    make distclean
    ```
    This does everything `make clean` does and also removes the `obj/`, `bin/`, `data/`, `scripts/`, and `plots/` directories.