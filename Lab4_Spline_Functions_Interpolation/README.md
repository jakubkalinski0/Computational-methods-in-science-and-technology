# Lab 4: Spline Functions Interpolation Analysis (Quadratic & Cubic)

## Overview

This project implements and analyzes **Quadratic (degree 2)** and **Cubic (degree 3) Spline Interpolation**. The goal is to approximate a given function, $f(x) = \sin(k \cdot x / \pi) \cdot \exp(-m \cdot x / \pi)$, over the interval $[a, b]$, using piecewise polynomials connected smoothly at specified nodes (knots).

Unlike single high-degree polynomial interpolation (like Lagrange or Newton), splines use multiple lower-degree polynomial segments. This approach often avoids the wild oscillations seen in high-degree polynomial interpolation (Runge's phenomenon), especially with uniformly spaced nodes.

The analysis investigates:
1.  **Spline Degree:** Comparing results from Quadratic ($C^1$ smooth) vs. Cubic ($C^2$ smooth) splines.
2.  **Boundary Conditions (BCs):** Evaluating the impact of different end conditions required to uniquely define the spline:
    *   **Quadratic:** Clamped Start (derivative fixed at $x_0=a$) vs. Clamped End (derivative fixed at $x_{n-1}=b$).
    *   **Cubic:** Natural (zero second derivative at ends) vs. Clamped (first derivative fixed at $x_0=a$ and $x_{n-1}=b$).
3.  **Number of Nodes (n):** Observing how accuracy changes as the number of uniformly spaced nodes ($n$) increases (corresponding to $n-1$ intervals).

The project calculates spline coefficients, evaluates the splines, measures approximation accuracy (Maximum Absolute Error, Mean Squared Error), visualizes results, and compares the different spline types and boundary conditions.

## Problem Description (Original Polish Task)

*Original task focused on Hermite, this project adapts it for Splines as requested.*

Dla funkcji $f(x) = \sin(k \cdot x / \pi) \cdot \exp(-m \cdot x / \pi)$, gdzie $k=4$, $m=0.4$, $x \in [-2\pi^2, \pi^2]$, wyznaczyć interpolacyjną **funkcję sklejaną drugiego stopnia** oraz **trzeciego stopnia**. Dla obu rodzajów funkcji (2-go i 3-go stopnia) należy wykonać obliczenia dla co najmniej dwóch różnych **warunków brzegowych**. Określić dokładność interpolacji – dla różnej liczby węzłów (przedziałów) i dla różnych warunków brzegowych. Porównać interpolację funkcjami sklejanymi drugiego i trzeciego stopnia. Graficznie zilustrować interesujące przypadki. Opisać dokładnie przyjęte warunki brzegowe.

*(**Translation:** For the function $f(x) = \sin(k \cdot x / \pi) \cdot \exp(-m \cdot x / \pi)$ where $k=4$, $m=0.4$, $x \in [-2\pi^2, \pi^2]$, determine the interpolating **quadratic spline** and **cubic spline**. For both types (2nd and 3rd degree), perform calculations for at least two different **boundary conditions**. Determine the interpolation accuracy – for varying numbers of nodes (intervals) and different boundary conditions. Compare quadratic and cubic spline interpolation. Graphically illustrate interesting cases. Describe the chosen boundary conditions precisely.)*

## Spline Interpolation Explained

### Concept

A spline function, denoted by $S(x)$, approximates a given function $f(x)$ over an interval $[a, b]$. The interval is partitioned by $n$ points called **nodes** or **knots**: $a = x_0 < x_1 < \dots < x_{n-1} = b$. The function values at these nodes are $y_i = f(x_i)$.

The spline $S(x)$ is constructed from **piecewise polynomials**, $S_i(x)$, where each $S_i(x)$ is defined on the subinterval $[x_i, x_{i+1}]$ for $i = 0, \dots, n-2$. These polynomial pieces are joined together at the interior nodes ($x_1, \dots, x_{n-2}$) such that the resulting function $S(x)$ satisfies certain smoothness conditions.

Let $h_i = x_{i+1} - x_i$ denote the length of the i-th subinterval.

### Quadratic Splines (Degree 2)

*   On each subinterval $[x_i, x_{i+1}]$ ($i = 0, \dots, n-2$), the spline piece $S_i(x)$ is a quadratic polynomial:
    $S_i(x) = a_i + b_i(x - x_i) + c_i(x - x_i)^2$
*   **Smoothness Conditions:** $S(x)$ must be continuous ($C^0$) and have a continuous first derivative ($C^1$) across the entire interval $[a, b]$. This implies:
    *   Interpolation: $S(x_i) = y_i$ for all nodes $i = 0, \dots, n-1$.
        *   This requires $S_i(x_i) = y_i$ and $S_i(x_{i+1}) = y_{i+1}$ for $i = 0, \dots, n-2$.
    *   Continuity of the first derivative at interior nodes: $S'_{i-1}(x_i) = S'_i(x_i)$ for $i = 1, \dots, n-2$.
*   **Degrees of Freedom:** There are $3(n-1)$ unknown coefficients ($a_i, b_i, c_i$). The conditions provide $3n - 4$ constraints. Thus, **1 additional boundary condition** is needed to uniquely determine the spline.
*   **Implementation Approach:** Let $s_i = S'(x_i)$ be the unknown first derivative (slope) of the spline at node $x_i$ ($i = 0, \dots, n-1$). The continuity conditions lead to the recurrence relation:
    $s_i + s_{i-1} = \frac{2(y_i - y_{i-1})}{h_{i-1}}$ for $i = 1, \dots, n-1$.
*   **Boundary Conditions Implemented:**
    1.  **Clamped Start:** Specify the slope at the start: $s_0 = f'(a)$. Use the recurrence relation to compute $s_1, s_2, \dots, s_{n-1}$ sequentially.
    2.  **Clamped End:** Specify the slope at the end: $s_{n-1} = f'(b)$. Use the recurrence relation rearranged as $s_{i-1} = \frac{2(y_i - y_{i-1})}{h_{i-1}} - s_i$ to compute $s_{n-2}, \dots, s_0$ backward.

### Cubic Splines (Degree 3)

*   On each subinterval $[x_i, x_{i+1}]$ ($i = 0, \dots, n-2$), the spline piece $S_i(x)$ is a cubic polynomial:
    $S_i(x) = a_i + b_i(x - x_i) + c_i(x - x_i)^2 + d_i(x - x_i)^3$
*   **Smoothness Conditions:** $S(x)$ must be continuous ($C^0$), have a continuous first derivative ($C^1$), and a continuous second derivative ($C^2$) across the entire interval $[a, b]$. This implies:
    *   Interpolation: $S(x_i) = y_i$ for all nodes $i = 0, \dots, n-1$.
    *   Continuity of the first derivative at interior nodes: $S'_{i-1}(x_i) = S'_i(x_i)$ for $i = 1, \dots, n-2$.
    *   Continuity of the second derivative at interior nodes: $S''_{i-1}(x_i) = S''_i(x_i)$ for $i = 1, \dots, n-2$.
*   **Degrees of Freedom:** There are $4(n-1)$ unknown coefficients. The conditions provide $4n - 6$ constraints. Thus, **2 additional boundary conditions** are needed.
*   **Implementation Approach:** Let $M_i = S''(x_i)$ be the unknown second derivative of the spline at node $x_i$ ($i = 0, \dots, n-1$). The continuity conditions ($C^1$ and $C^2$) lead to a **tridiagonal system** of linear equations for the unknowns $M_0, \dots, M_{n-1}$:
    $h_{i-1} M_{i-1} + 2(h_{i-1} + h_i) M_i + h_i M_{i+1} = 6 \left[ \frac{y_{i+1} - y_i}{h_i} - \frac{y_i - y_{i-1}}{h_{i-1}} \right]$
    This equation holds for the interior nodes $i = 1, \dots, n-2$. The first ($i=0$) and last ($i=n-1$) equations are determined by the boundary conditions.
*   **Boundary Conditions Implemented:**
    1.  **Natural Spline:** Assumes zero curvature at the endpoints:
        $M_0 = 0$
        $M_{n-1} = 0$
        These directly provide the first and last equations for the tridiagonal system.
    2.  **Clamped Spline:** Specifies the first derivative (slope) at the endpoints, usually matching the derivative of the function $f(x)$:
        $S'(x_0) = f'(a)$
        $S'(x_{n-1}) = f'(b)$
        These conditions lead to the following equations for the first ($i=0$) and last ($i=n-1$) rows of the tridiagonal system:
        *   $2h_0 M_0 + h_0 M_1 = 6 \left[ \frac{y_1 - y_0}{h_0} - f'(a) \right]$
        *   $h_{n-2} M_{n-2} + 2h_{n-2} M_{n-1} = 6 \left[ f'(b) - \frac{y_{n-1} - y_{n-2}}{h_{n-2}} \right]$

## Key Features

*   Implements **Quadratic Spline** interpolation with Clamped Start/End BCs.
*   Implements **Cubic Spline** interpolation with Natural and Clamped BCs.
*   Uses the function $f(x) = \sin(k \cdot x / \pi) \cdot \exp(-m \cdot x / \pi)$ and its first derivative $df(x)$.
*   Uses **uniformly spaced** nodes over the interval $[-2\pi^2, \pi^2]$ ($a$ to $b$).
*   Iterates through a user-defined range of node counts ($n \ge 3$).
*   Calculates **Maximum Absolute Error** and **Mean Squared Error (MSE)**.
*   Includes an efficient **tridiagonal system solver** for cubic splines.
*   Saves detailed results to data files (`.dat`, `.csv`).
*   Automatically generates **Gnuplot scripts** (`.gp`) for visualization:
    *   Individual plots comparing original function, spline, and nodes for each $n$, spline type, and BC.
    *   A summary plot comparing the maximum absolute error trends vs. $n$ for all four spline configurations.
*   Provides a **Makefile** for easy compilation, execution, plotting, and cleanup.

## 📁 Project Structure
- 📁 **Lab4_Spline_Functions_Interpolation/** – Main
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
    - 📁 **bin/** – Compiled executable
    - 📁 **obj/** – Object files (.o)
    - 📁 **data/** – Output data files (.dat, .csv)
    - 📁 **scripts/** – Gnuplot scripts (.gp)
    - 📁 **plots/** – Plot images (.png)
    - 🛠️ **Makefile**
    - 📘 **README.md** (This file)
    - 📄 **Report_Lab4_Spline_Functions_Interpolation.pdf** – Detailed analysis (in Polish - *to be created*)

## Requirements

*   **C Compiler:** C11 or later (e.g., GCC, Clang).
*   **Make:** Build automation tool.
*   **Gnuplot:** Plotting utility.

## Building and Running

Use the Makefile in the project root directory:

1.  **Compile:**
    ```bash
    make
    ```
2.  **Compile and Run (Generate Data & Scripts):**
    ```bash
    make run
    ```
    *(Prompts for max number of nodes $n$, suggest $\ge 3$)*
3.  **Compile, Run, and Generate Plots:**
    ```bash
    make plots
    ```
4.  **Display Help:**
    ```bash
    make help
    ```

## Output Files

After `make plots`, check these directories:

*   **`data/`:**
    *   `original_function.dat`
    *   `uniform_nodes_n*.dat`
    *   `quad_clamp_start_n*.dat`, `quad_clamp_end_n*.dat`
    *   `cubic_natural_n*.dat`, `cubic_clamped_n*.dat`
    *   `quadratic_clamp_start_errors.csv`, `quadratic_clamp_end_errors.csv`
    *   `cubic_natural_errors.csv`, `cubic_clamped_errors.csv`
*   **`scripts/`:**
    *   `plot_spline_interpolation.gp`
    *   `plot_spline_errors.gp`
*   **`plots/`:**
    *   `quad_*.png`, `cubic_*.png`: Individual interpolation plots.
    *   `spline_interpolation_errors.png`: Summary error plot.

## Analysis and Interpretation

A detailed analysis comparing quadratic vs. cubic splines, the effects of different boundary conditions, convergence rates, and overall suitability for approximating the given function should be included in a separate report (e.g., `Report_Lab4_Spline_Functions_Interpolation.pdf`). Key observations likely include:
*   Cubic splines generally provide better accuracy than quadratic splines for the same number of nodes due to higher smoothness ($C^2$ vs $C^1$).
*   Clamped boundary conditions often yield better accuracy than natural boundary conditions, especially if the true function's end slopes are non-zero, as they incorporate more information about the function. Natural BCs might introduce larger errors near the boundaries.
*   Splines avoid the Runge phenomenon observed with high-degree polynomial interpolation using uniform nodes. Error should decrease steadily as $n$ increases.
*   The error convergence rate is expected to be $O(h^3)$ for quadratic splines and $O(h^4)$ for cubic splines (where $h$ is the step size), though this depends on the function's smoothness and boundary conditions. This should be visible on the log-log error plot.

## Cleanup

*   **Remove generated files:**
    ```bash
    make clean
    ```
*   **Remove generated files AND directories:**
    ```bash
    make distclean
    ```