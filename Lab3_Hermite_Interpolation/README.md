# Lab 3: Hermite Interpolation Analysis

## Overview

This project implements and analyzes **Hermite polynomial interpolation**. The goal is to approximate a given function, `f(x) = sin(k * x / PI) * exp(-m * x / PI)`, over the interval `[a, b]`. Unlike Lagrange or Newton interpolation which only match function values at nodes, Hermite interpolation matches both the **function values (`f(x)`)** and the **first derivative values (`f'(x)`)** at each distinct node.

The analysis investigates the impact of:
1.  **Node Distribution:** Comparing results using equally spaced (**uniform**) nodes versus **Chebyshev** nodes (roots of Chebyshev polynomials mapped to the interval).
2.  **Number of Distinct Nodes (n):** Observing how the accuracy of the interpolation changes as the number of distinct nodes (`n`) increases. Note that for `n` distinct nodes, the resulting Hermite polynomial has a degree of `2n - 1`.

The project calculates the interpolated values using a generalized divided difference approach, measures the approximation accuracy using maximum absolute error and mean squared error, visualizes the results, and aims to identify phenomena like the Runge effect (oscillations near the interval ends, especially with uniform nodes and high degrees).

## Problem Description (Original Polish Task)

Dla jednej z poniższych funkcji (podanej w zadaniu indywidualnym) wyznacz dla zagadnienia Hermite'a wielomian interpolujący w postaci Hermite'a. (wystarczy zaimplementować algorytm, pozwalający na wyznaczenie wielomianu interpolacyjnego z wykorzystaniem pierwszych pochodnych).

Interpolację przeprowadź dla różnej liczby węzłów (np. n = 3, 4, 5, 7, 10, 15, 20). Dla każdego przypadku interpolacji porównaj wyniki otrzymane dla różnego rozmieszczenia węzłów: równoodległe oraz Czebyszewa (zera wielomianu Czebyszewa).

Oceń dokładność, z jaką wielomian przybliża zadaną funkcję.

Poszukaj wielomianu, który najlepiej przybliża zadaną funkcję.

Wyszukaj stopień wielomianu, dla którego można zauważyć efekt Runge’go (dla równomiernego rozmieszczenia węzłów). Porównaj z wyznaczonym wielomianem dla węzłów Czebyszewa.

Funkcja którą ja otrzymałem to:
`f(x) = sin(k * x / PI) * exp(-m * x / PI)`,
gdzie `k=4`, `m=0.4`, `x in [-2*PI^2, PI^2]`

Uwaga: Zalecane jest rysowanie wykresów funkcji, wielomianów interpolujących, ... , czyli graficzne ilustrowanie przeprowadzonych eksperymentów numerycznych. W sprawozdaniu należy umieścić wykresy jedynie dla wybranych przypadków!

*(**Translation:** For the assigned function `f(x) = sin(k*x/PI) * exp(-m*x/PI)` where `k=4`, `m=0.4`, `x in [-2*PI^2, PI^2]`, determine the interpolating polynomial for the Hermite problem in Hermite form (implementing an algorithm that allows determining the interpolating polynomial using first derivatives is sufficient). Perform interpolation for various numbers of nodes (e.g., n=3, 4, 5, 7, 10, 15, 20). For each case, compare results obtained with different node distributions: uniform and Chebyshev (zeros of the Chebyshev polynomial). Evaluate the accuracy of the polynomial approximation. Find the polynomial that best approximates the function. Identify the polynomial degree where the Runge phenomenon becomes noticeable (for uniform nodes) and compare it with the polynomial obtained using Chebyshev nodes. Note: It is recommended to draw graphs of the function, interpolating polynomials, etc., i.e., graphically illustrate the numerical experiments performed. Include graphs only for selected cases in the report!)*

## Key Features

*   Implements **Hermite interpolation** using generalized divided differences, matching both function values (`f(x)`) and first derivative values (`f'(x)`) at nodes.
*   Includes the function `f(x) = sin(k*x/PI) * exp(-m*x/PI)` and its first derivative `df(x)`.
*   Generates interpolation nodes using **uniform spacing** and **Chebyshev distribution**.
*   Interpolates the function over the interval `[-2*PI^2, PI^2]` (`a` to `b`).
*   Iterates through a user-defined range of *distinct* node counts (`n`).
*   Calculates **Maximum Absolute Error** and **Mean Squared Error (MSE)** between the true function and the Hermite interpolating polynomial.
*   Saves detailed results to data files:
    *   Original function points (`.dat`).
    *   Interpolation nodes (x, y) for each `n` and type (`.dat`).
    *   Interpolated function points for each configuration (`.dat`).
    *   Error summaries (Max Error, MSE vs. `n`) for each configuration (`.csv`).
*   Automatically generates **Gnuplot scripts** (`.gp`) for visualizing:
    *   Individual plots comparing the original function, the Hermite interpolated function, and the nodes for each `n` and node type (Uniform/Chebyshev).
    *   A summary plot comparing the maximum absolute error trend versus the number of nodes (`n`) for Hermite interpolation with Uniform vs. Chebyshev nodes.
*   Provides a **Makefile** for easy compilation, execution, data/script generation, plotting, and cleanup.

## 📁 Project Structure
- 📁 **Lab3_Hermite_Interpolation/** – Main
    - 📁 **include/** – Header files (.h)
        - 📄 `common.h`
        - 📄 `error.h`
        - 📄 `fileio.h`
        - 📄 `function.h`
        - 📄 `interpolation.h`
        - 📄 `nodes.h`
    - 📁 **src/** – Source code files (.c)
        - 📄 `error.c`
        - 📄 `fileio.c`
        - 📄 `function.c`
        - 📄 `interpolation.c`
        - 📄 `main.c`
        - 📄 `nodes.c`
    - 📁 **bin/** – Compiled executable (created by `make`)
    - 📁 **obj/** – Object files (.o) (created by `make`)
    - 📁 **data/** – Output data files (.dat, .csv) (created by `make run` / `make plots`)
    - 📁 **scripts/** – Gnuplot scripts (.gp) generated by `make run` / `make plots`
    - 📁 **plots/** – Plot images (.png) generated by `make plots`
    - 🛠️ **Makefile** – Build script
    - 📘 **README.md** – Project description (this file)
    - 📄 **Report_Lab3_Hermite_Interpolation.pdf** – Detailed analysis (in Polish)


## Requirements

*   **C Compiler:** A C compiler supporting C11 or later (e.g., GCC, Clang).
*   **Make:** The `make` build automation tool.
*   **Gnuplot:** The Gnuplot utility for generating plots from the scripts.

## Building and Running

The project uses a Makefile for easy management. Open your terminal in the project's root directory and use the following commands:

1.  **Compile the program:**
    ```bash
    make all
    # or simply:
    make
    ```
    This compiles the source code and creates the executable `bin/interpolation`.

2.  **Compile and Run (Generate Data & Scripts):**
    ```bash
    make run
    ```
    This compiles (if needed) and runs the C program. You will be prompted to enter the maximum number of *distinct* nodes (`n`). This generates data files (`.dat`, `.csv`) in `data/` and Gnuplot scripts (`.gp`) in `scripts/`.

3.  **Compile, Run, and Generate Plots:**
    ```bash
    make plots
    ```
    This performs the `make run` steps and then automatically executes the Gnuplot scripts to generate PNG plot images in the `plots/` directory.

4.  **Display Help:**
    ```bash
    make help
    ```
    Shows a summary of available Makefile commands.

## Output Files

After running `make plots`, you will find the following generated files:

*   **`data/` Directory:**
    *   `original_function.dat`: Points representing the true function `f(x)`.
    *   `uniform_nodes_n*.dat`: Coordinates of uniform interpolation nodes for `n` distinct nodes.
    *   `chebyshev_nodes_n*.dat`: Coordinates of Chebyshev interpolation nodes for `n` distinct nodes.
    *   `hermite_uniform_n*.dat`: Interpolated points using Hermite/Uniform for `n` distinct nodes.
    *   `hermite_chebyshev_n*.dat`: Interpolated points using Hermite/Chebyshev for `n` distinct nodes.
    *   `hermite_uniform_errors.csv`: Max error and MSE vs. `n` for Hermite/Uniform.
    *   `hermite_chebyshev_errors.csv`: Max error and MSE vs. `n` for Hermite/Chebyshev.
*   **`scripts/` Directory:**
    *   `plot_interpolation.gp`: Gnuplot script to generate individual Hermite interpolation plots for each `n`.
    *   `plot_errors.gp`: Gnuplot script to generate the summary error comparison plot (Hermite Uniform vs. Chebyshev).
*   **`plots/` Directory:**
    *   `hermite_uniform_with_nodes_n*.png`, `hermite_chebyshev_with_nodes_n*.png`: Individual plots showing original vs. Hermite interpolated function and nodes for each configuration and `n`.
    *   `interpolation_errors.png`: Summary plot showing maximum absolute error vs. `n` for Hermite/Uniform and Hermite/Chebyshev.

## Analysis and Interpretation

A detailed analysis of the results, comparing the node distributions, accuracy, observing the behavior as `n` increases (including potential Runge effect for uniform nodes), and discussing the effectiveness of Hermite interpolation for this function, can be found in the accompanying report file:

**`Report_Lab3_Hermite_Interpolation.pdf`**

Please note that the analysis within that document is written in **Polish**.

## Cleanup

*   **Remove generated files (but keep directories):**
    ```bash
    make clean
    ```
    This removes object files (`*.o`), the executable, data files (`*.dat`, `*.csv`), Gnuplot scripts (`*.gp`), and plot images (`*.png`).

*   **Remove generated files AND directories:**
    ```bash
    make distclean
    ```
    This does everything `make clean` does and also removes the `obj/`, `bin/`, `data/`, `scripts/`, and `plots/` directories.