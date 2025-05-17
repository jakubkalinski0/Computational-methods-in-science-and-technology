# Lab 6: Least Squares Trigonometric Approximation Analysis (Direct Method)

## Overview

This project implements and analyzes **least-squares trigonometric approximation** using **direct summation formulas**. The goal is to approximate a given function, $f(x) = \sin(k \cdot x / \pi) \cdot \exp(-m \cdot x / \pi)$, over the interval $[a, b]$. The method finds the trigonometric sum $T_m(x)$ up to a specified maximum harmonic order $m$ that approximates the function based on its values at $n$ discrete sample points $(x_i, y_i)$.

The trigonometric sum has the form:
$$T_m(x) = \frac{a_0}{2} + \sum_{k=1}^{m} [a_k \cos(k \omega x) + b_k \sin(k \omega x)]$$
where $m$ is the maximum harmonic order, and $\omega = 2\pi / (b-a)$ is the fundamental angular frequency adapted to the interval $[a, b]$.

The coefficients $a_k$ and $b_k$ are calculated using direct summation formulas, which are discrete approximations of the standard Fourier integral formulas:
$$ a_k \approx \frac{2}{n} \sum_{i=0}^{n-1} y_i \cos(k \omega x_i) \quad (k=0, \dots, m) $$
$$ b_k \approx \frac{2}{n} \sum_{i=0}^{n-1} y_i \sin(k \omega x_i) \quad (k=1, \dots, m) $$
This approach relies on the properties associated with the Discrete Fourier Transform (DFT) and typically requires the sample points $x_i$ to be uniformly distributed for the results to accurately represent the least-squares best fit. Crucially, it requires the condition $m < n/2$ (maximum harmonic less than half the number of points) to avoid aliasing and ensure stability, based on the Nyquist-Shannon sampling theorem.

The analysis investigates the impact of:
1.  **Number of Sample Points ($n$):** How accuracy changes as $n$ increases (using uniformly spaced points).
2.  **Maximum Harmonic Order ($m$):** How accuracy varies with the complexity of the trigonometric sum (controlled by $m$, requiring $m < n/2$).

Accuracy is measured using maximum absolute error and mean squared error (MSE). Data is generated for valid $(n, m)$ combinations, and results are visualized using individual comparison plots (Gnuplot) and error heatmaps (Python).

## Problem Description (Adapted Task)

The goal is to approximate the function $f(x) = \sin(kx/\pi) \exp(-mx/\pi)$ (with $k=4, m=0.4$) on the interval $[-2\pi^2, \pi^2]$ using **least-squares trigonometric approximation via direct summation formulas**. The analysis focuses on the accuracy depending on the number of discretization points ($n$) and the maximum harmonic order ($m$).

The process involves:
1.  Implementing C modules for the function, node generation (uniform), error calculation, I/O.
2.  Implementing trigonometric coefficient calculation in C using the **direct summation formulas**.
3.  Discretizing the function using $n$ uniformly spaced points $(x_i, y_i)$.
4.  For each $n$ and various valid harmonic orders $m$ (where $0 \le m < n/2$), calculating the coefficients $a_k, b_k$ using the summation formulas.
5.  Calculating approximation errors (Max Absolute, MSE) on a dense grid.
6.  Visualizing results using Python (heatmaps) and Gnuplot (individual plots).

## Key Features

*   Implements **trigonometric approximation using direct summation formulas**, closely related to DFT.
*   Uses the basis $\{1, \cos(k \omega x), \sin(k \omega x)\}$ up to $k=m$, where $\omega=2\pi/(b-a)$.
*   Includes the target function $f(x) = \sin(k \cdot x / \pi) \cdot \exp(-m \cdot x / \pi)$.
*   Generates **uniformly spaced sample points** within the interval $[a, b]$.
*   Iterates through user-defined ranges of **sample points ($n$)** and **max harmonic order ($m$)**.
*   Enforces the condition **$m < n/2$** for calculations based on sampling theory.
*   Calculates **Maximum Absolute Error** and **Mean Squared Error (MSE)**.
*   Saves detailed results:
    *   Original function points (`.dat`).
    *   Sample points $(x_i, y_i)$ for each $n$ (`.dat`).
    *   Approximating trigonometric sum $T_m(x)$ points for each valid $(n, m)$ (`.dat`).
    *   Error summary (`N, m, MaxError, MSE`) for heatmaps (`.csv`).
*   Generates a **Gnuplot script** (`.gp`) for **individual plots** comparing $f(x)$, $T_m(x)$, and sample points for valid $(n, m)$ pairs.
*   Includes a **Python script** (`plot_heatmaps.py`) using pandas, matplotlib, seaborn to generate **heatmap plots** visualizing error trends (Max Error/MSE vs. $n$ and $m$).
*   Provides a **Makefile** for easy compilation, execution, data generation, plotting, and cleanup.

## 📁 Project Structure
- 📁 **Lab6_Trigonometric_Approximation_Direct/** – Main
    - 📁 **include/** – Header files (.h)
    - 📁 **src/** – Source code files (.c), Python script (.py)
    - 📁 **bin/** – Compiled executable (`trig_approx_direct_app`)
    - 📁 **obj/** – Object files (.o)
    - 📁 **data/** – Output data files (.dat, `trig_approx_direct_heatmap_errors.csv`)
    - 📁 **scripts/** – Gnuplot scripts (`plot_all_trig_approximations.gp`)
    - 📁 **plots/** – Plot images (.png, .svg, .pdf)
    - 🛠️ **Makefile**
    - 📘 **README.md** (This file)
    - *(Optional: Report file)*

## Requirements

*   **C Compiler:** GCC, Clang, etc. (C11 support).
*   **Make:** Build tool.
*   **Gnuplot:** For individual plots (version >= 5.2 recommended for `fileexists()`).
*   **Python 3:** For heatmaps.
    *   **Libraries:** pandas, matplotlib, seaborn, numpy (`pip install pandas matplotlib seaborn numpy`). Use a virtual environment if preferred.

## Building and Running

Use the Makefile in the project root directory:

1.  **Compile:** `make all` or `make` (creates `bin/trig_approx_direct_app`)
2.  **Run C code (Generate Data & Gnuplot Script):** `make run`
    *   Prompts for max $n$ and max $m$.
    *   Generates `data/trig_approx_direct_heatmap_errors.csv`, `data/*.dat`, `scripts/*.gp`. Calculations are only performed for $m < n/2$.
3.  **Generate Heatmap Plots (Python):** `make py_plots`
    *   Runs `src/plot_heatmaps.py` (ensure it reads the correct CSV and uses 'm' labels). Requires data from `make run`.
    *   Generates plots in `plots/`.
4.  **Generate Individual Plots (Gnuplot):** `make generate_individual_plots`
    *   Runs Gnuplot script from `scripts/`. Requires script/data from `make run`.
    *   Generates PNG plots in `plots/` only for $m < n/2$.
5.  **All Steps (Compile, Run C, Plot Python & Gnuplot):** `make plots`
6.  **Help:** `make help` (shows available commands)

## Output Files

*   **`data/`:**
    *   `trig_approx_direct_heatmap_errors.csv`: Heatmap data (N, m, Errors). Includes NAN for $m \ge n/2$.
    *   `original_function_plot.dat`: True function points.
    *   `sample_points_n*.dat`: Sample points used.
    *   `trig_approx_m*_points*.dat`: Approximating sum points (only generated for $m < n/2$).
*   **`scripts/`:**
    *   `plot_all_trig_approximations.gp`: Gnuplot script for individual plots (plots only valid $m < n/2$ cases).
*   **`plots/`:**
    *   `trig_direct_*.svg/pdf/png`, `annotated_trig_direct_*.svg/pdf/png`: Heatmaps from Python.
    *   `trig_approx_m*_n*.png`: Individual plots from Gnuplot (only for $m < n/2$).

## Analysis and Interpretation

Analyze the generated heatmaps and individual plots to understand:
*   How approximation error (Max Abs, MSE) changes with increasing $n$ (number of samples) for a fixed $m$ (max harmonic).
*   How error changes with increasing $m$ for a fixed $n$, noting the hard cutoff at $m = \lfloor (n-1)/2 \rfloor$ due to the $m < n/2$ condition.
*   The effectiveness of the direct summation method for this function and uniform sampling.
*   Compare error behavior to results obtained using the Normal Equations method (if available) - expect similar trends but potentially different behavior near the stability limit.

*(Consider writing a separate report document for detailed analysis)*

## Cleanup

*   **Remove generated files:** `make clean`
*   **Remove generated files and directories:** `make distclean`