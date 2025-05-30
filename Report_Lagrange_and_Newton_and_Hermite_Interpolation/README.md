# Lab 2 & 3 report: Polynomial Interpolation Analysis (Lagrange, Newton, Hermite)

## Overview

This project implements and compares polynomial interpolation techniques, specifically the **Lagrange**, **Newton**, and **Hermite** methods. The goal is to approximate a given function, `f(x) = sin(k * x / PI) * exp(-m * x / PI)`, over the interval `[a, b]`. Hermite interpolation additionally uses the first derivative `f'(x)` at the nodes.

The analysis investigates the impact of:
1.  **Interpolation Method:** Comparing Lagrange's polynomial form, Newton's divided difference form, and Hermite's generalized divided difference form (which matches both function values and first derivatives).
2.  **Node Distribution:** Comparing results using equally spaced (**uniform**) nodes versus **Chebyshev** nodes (roots of Chebyshev polynomials mapped to the interval).
3.  **Number of Nodes (n):** Observing how the accuracy of the interpolation changes as the number of distinct nodes (`n`) increases. (Note: for Hermite, the polynomial degree is `2n-1`).

The project calculates the interpolated values, measures the approximation accuracy using maximum absolute error and mean squared error, visualizes the results, and aims to identify phenomena like the Runge effect (oscillations near the interval ends, especially with uniform nodes and high degrees).

*This project extends the original assignment by incorporating Hermite interpolation.*

## Problem Description (Original Polish Task)

Dla jednej z poniższych funkcji (podanej w zadaniu indywidualnym) wyznacz dla zagadnienia
Lagrange’a wielomian interpolujący w postaci Lagrange'a, Newton'a oraz Hermite'a.

Interpolację przeprowadź dla różnej liczby węzłów (np. n = 3, 4, 5, 7, 10, 15, 20). Dla każdego
przypadku interpolacji porównaj wyniki otrzymane dla różnego rozmieszczenia węzłów:
równoodległe oraz Czebyszewa*.

Oceń dokładność, z jaką wielomian przybliża zadaną funkcję.

Poszukaj wielomianu, który najlepiej przybliża zadaną funkcję.

Wyszukaj stopień wielomianu, dla którego można zauważyć efekt Runge’go (dla równomiernego
rozmieszczenia węzłów). Porównaj z wyznaczonym wielomianem dla węzłów Czebyszewa.

Funkcja którą ja otrzymałem to:
`f(x) = sin(k * x / PI) * exp(-m * x / PI)`,
gdzie `k=4`, `m=0.4`, `x in [-2*PI^2, PI^2]`

*(**Translation:** For the assigned function `f(x) = sin(k*x/PI) * exp(-m*x/PI)` where `k=4`, `m=0.4`, `x in [-2*PI^2, PI^2]`, determine the interpolating polynomial in both Lagrange, Newton and Hermite forms. Perform interpolation for various numbers of nodes (e.g., n=3, 4, 5, 7, 10, 15, 20). For each case, compare results obtained with different node distributions: uniform and Chebyshev. Evaluate the accuracy of the polynomial approximation. Find the polynomial that best approximates the function. Identify the polynomial degree where the Runge phenomenon becomes noticeable (for uniform nodes) and compare it with the polynomial obtained using Chebyshev nodes.)*

## Key Features

*   Implements **Lagrange interpolation**.
*   Implements **Newton interpolation** using divided differences.
*   Implements **Hermite interpolation** using generalized divided differences, matching function values and first derivatives.
*   Calculates the function `f(x) = sin(k*x/PI) * exp(-m*x/PI)` and its first derivative `df(x)`.
*   Generates interpolation nodes using **uniform spacing** and **Chebyshev distribution**.
*   Interpolates the function over the interval `[-2*PI^2, PI^2]`.
*   Iterates through a user-defined range of node counts (`n`).
*   Calculates **Maximum Absolute Error** and **Mean Squared Error (MSE)** between the true function and the interpolating polynomial for all methods and node types.
*   Saves detailed results to data files:
    *   Original function points (`.dat`).
    *   Interpolation nodes (`.dat`).
    *   Interpolated function points for each configuration (`.dat`).
    *   Error summaries (Max Error, MSE vs. `n`) for each configuration (`.csv`).
*   Automatically generates **Gnuplot scripts** (`.gp`) for visualizing:
    *   Individual plots comparing the original function, interpolated function, and nodes for each `n`, method (Lagrange, Newton, Hermite), and node type (Uniform, Chebyshev). Includes plots comparing all methods for a given node type and `n`.
    *   A summary plot comparing the maximum absolute error trend versus the number of nodes (`n`) for all **six** configurations (Lagrange/Newton/Hermite x Uniform/Chebyshev).
*   Provides a **Makefile** for easy compilation, execution, data/script generation, plotting, and cleanup.

## 📁 Project Structure
- 📁 **ProjectRoot/** – Main Directory
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
    - 📄 **Report_Lagrange_and_Newton_and_Hermite_Interpolation.pdf** – Detailed analysis (in Polish)

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
    This compiles (if needed) and runs the C program. You will be prompted to enter the maximum number of nodes. This generates data files (`.dat`, `.csv`) in `data/` and Gnuplot scripts (`.gp`) in `scripts/`, including results for Lagrange, Newton, and Hermite interpolation.

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
    *   `uniform_nodes_n*.dat`: Coordinates of uniform interpolation nodes for `n` nodes.
    *   `chebyshev_nodes_n*.dat`: Coordinates of Chebyshev interpolation nodes for `n` nodes.
    *   `lagrange_uniform_n*.dat`: Interpolated points using Lagrange/Uniform for `n` nodes.
    *   `lagrange_chebyshev_n*.dat`: Interpolated points using Lagrange/Chebyshev for `n` nodes.
    *   `newton_uniform_n*.dat`: Interpolated points using Newton/Uniform for `n` nodes.
    *   `newton_chebyshev_n*.dat`: Interpolated points using Newton/Chebyshev for `n` nodes.
    *   `hermite_uniform_n*.dat`: Interpolated points using Hermite/Uniform for `n` nodes.
    *   `hermite_chebyshev_n*.dat`: Interpolated points using Hermite/Chebyshev for `n` nodes.
    *   `lagrange_uniform_errors.csv`: Max error and MSE vs. `n` for Lagrange/Uniform.
    *   `lagrange_chebyshev_errors.csv`: Max error and MSE vs. `n` for Lagrange/Chebyshev.
    *   `newton_uniform_errors.csv`: Max error and MSE vs. `n` for Newton/Uniform.
    *   `newton_chebyshev_errors.csv`: Max error and MSE vs. `n` for Newton/Chebyshev.
    *   `hermite_uniform_errors.csv`: Max error and MSE vs. `n` for Hermite/Uniform.
    *   `hermite_chebyshev_errors.csv`: Max error and MSE vs. `n` for Hermite/Chebyshev.
*   **`scripts/` Directory:**
    *   `plot_interpolation.gp`: Gnuplot script to generate individual interpolation plots for each `n` (including Lagrange, Newton, Hermite, and combined plots).
    *   `plot_errors.gp`: Gnuplot script to generate the summary error comparison plot (6 curves).
*   **`plots/` Directory:**
    *   `lagrange_uniform_with_nodes_n*.png`, `lagrange_chebyshev_with_nodes_n*.png`
    *   `newton_uniform_with_nodes_n*.png`, `newton_chebyshev_with_nodes_n*.png`
    *   `hermite_uniform_with_nodes_n*.png`, `hermite_chebyshev_with_nodes_n*.png`
    *   `all_uniform_with_nodes_n*.png`, `all_chebyshev_with_nodes_n*.png`: Individual plots showing original vs. interpolated functions (multiple methods) and nodes for each configuration and `n`.
    *   `interpolation_errors.png`: Summary plot showing maximum absolute error vs. `n` for all six configurations.

## Analysis and Interpretation

A detailed analysis of the results, comparing the methods (Lagrange, Newton, Hermite), node distributions, accuracy, and discussing phenomena like the Runge effect, should be conducted. The accompanying report file may contain analysis primarily focused on Lagrange and Newton:

**`Report_Lab2_Lagrange_and_Newton_Interpolation.pdf`**

Please note that the analysis within that document is written in **Polish** and may need to be extended or supplemented to fully cover the Hermite interpolation results generated by this updated code.

## Cleanup

*   **Remove generated files (but keep directories):**
    ```bash
    make clean
    ```
    This removes object files (`*.o`), the executable, CSV files (`*.csv`), Gnuplot scripts (`*.gp`), data files (`*.dat`), and plot images (`*.png`).

*   **Remove generated files AND directories:**
    ```bash
    make distclean
    ```
    This does everything `make clean` does and also removes the `obj/`, `bin/`, `data/`, `scripts/`, and `plots/` directories.