/**
 * @file main.c
 * @brief Main program driver for finding roots of f(x) = x^n + x^m using Newton's and Secant methods.
 *        Analyzes convergence and iteration counts across ranges of starting points
 *        and precision values (rho), for different stopping criteria.
 *        Generates data (CSV for tables/plots) and basic Gnuplot scripts.
 *        Detailed visualization (heatmaps) is handled by an external Python script.
 *        Uses simplified loops with epsilon comparison for endpoint and float equality.
 */
#include "../include/common.h"
#include "../include/function.h"
#include "../include/root_finding.h"
#include "../include/fileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <float.h>

#define NUM_PLOT_POINTS 500
#define START_POINT_STEP 0.1
#define NUM_PRECISIONS 7

const double PRECISION_VALUES[NUM_PRECISIONS] = {
    1e-2, 1e-3, 1e-4, 1e-5, 1e-7, 1e-10, 1e-15
};

const char* STOP_CRITERION_NAMES[] = {
    "Stop_dX",
    "Stop_fX",
    "Stop_Both"
};
#define NUM_STOP_CRITERIA (sizeof(STOP_CRITERION_NAMES)/sizeof(STOP_CRITERION_NAMES[0]))


/**
 * @brief Main entry point for the root-finding analysis program.
 */
int main() {
    printf("--- Root Finding Analysis for f(x) = x^%.0f + x^%.0f on [%.2f, %.2f] ---\n", N_param, M_param, a, b);
    printf("Methods: Newton, Secant\n");
    printf("Stopping Criteria to be tested: |x_next - x_curr| < rho; |f(x_next)| < rho; Both\n");
    printf("Generates CSV data and triggers Python script for plots.\n\n");

    printf("Generating Gnuplot scripts to plot f(x)...\n");
    const char* function_data_file_name_only = "function_data.dat";
    char function_data_full_path[256];
    snprintf(function_data_full_path, sizeof(function_data_full_path), "data/%s", function_data_file_name_only);

    generateFunctionPlotScript("plot_function.gp", "function_plot.png", NUM_PLOT_POINTS);
    generateFunctionPlotScriptZoomed("plot_function_zoomed.gp", "function_plot_zoomed.png", function_data_full_path);
    printf("Run Gnuplot scripts in 'scripts/' directory to generate function plots.\n\n");

    const char *results_filename = "root_finding_results.csv";
    FILE *csv_file = openResultCsvFile(results_filename);
    if (csv_file == NULL) {
        return 1;
    }
    printf("Opened data/%s for writing results.\n", results_filename);

    printf("\nStarting root-finding analysis...\n");
    printf("===================================================================\n");

    double epsilon = START_POINT_STEP * 0.01;

    for (unsigned int crit_idx = 0; crit_idx < NUM_STOP_CRITERIA; ++crit_idx) {
        StopCriterionType current_criterion_type = (StopCriterionType)crit_idx;
        const char* current_criterion_name = STOP_CRITERION_NAMES[crit_idx];
        printf("Processing for Stop Criterion: %s\n", current_criterion_name);

        for (int p_idx = 0; p_idx < NUM_PRECISIONS; ++p_idx) {
            double current_precision = PRECISION_VALUES[p_idx];
            printf("  Processing Precision rho = %.1e (Criterion: %s)\n", current_precision, current_criterion_name);

            printf("    Running Newton's Method...\n");
            for (double x0_newton = a; x0_newton <= b + epsilon; x0_newton += START_POINT_STEP) {
                double current_x0_n = x0_newton;
                if (x0_newton > b && x0_newton < b + epsilon) current_x0_n = b;
                if (current_x0_n > b + epsilon) break;
                RootResult newton_res = newtonMethod(current_x0_n, current_precision, MAX_ITERATIONS, current_criterion_type);
                appendNewtonResultToCsv(csv_file, current_criterion_name, current_x0_n, current_precision, newton_res);
                if (fabs(current_x0_n - b) < DBL_EPSILON) break;
            }

            // --- Secant Method - Case 1: x0 = a (fixed), x1 iterates ---
            printf("    Running Secant Method (x0 = a = %.2f fixed, x1 iterates)...\n", a); // ZMIANA OPISU
            double x0_fixed_a = a; // x0 jest teraz stałe 'a'
            // Iterujemy x1 od a + krok do b
            for (double x1_iter = a + START_POINT_STEP; x1_iter <= b + epsilon; x1_iter += START_POINT_STEP) {
                double current_x1 = x1_iter;
                if (x1_iter > b && x1_iter < b + epsilon) current_x1 = b;
                if (current_x1 > b + epsilon) break;

                // Unikaj x0 == x1
                if (fabs(x0_fixed_a - current_x1) < DBL_EPSILON * fmax(1.0, fmax(fabs(x0_fixed_a), fabs(current_x1)))) continue;

                // Wywołanie: secantMethod(stałe_x0, iterowane_x1, ...)
                RootResult secant_res_a = secantMethod(x0_fixed_a, current_x1, current_precision, MAX_ITERATIONS, current_criterion_type);
                appendSecantResultToCsv(csv_file, current_criterion_name, x0_fixed_a, current_x1, current_precision, secant_res_a);

                if (fabs(current_x1 - b) < DBL_EPSILON) break;
            }

            // --- Secant Method - Case 2: x0 = b (fixed), x1 iterates ---
            printf("    Running Secant Method (x0 = b = %.2f fixed, x1 iterates)...\n", b); // ZMIANA OPISU
            double x0_fixed_b = b; // x0 jest teraz stałe 'b'
            // Iterujemy x1 od a do b - krok (aby nie było x1=b, gdy x0=b)
            for (double x1_iter = a; x1_iter <= b - START_POINT_STEP + epsilon; x1_iter += START_POINT_STEP) {
                double current_x1 = x1_iter;
                 // Clamp x1, ale upewnij się, że nie jest równe x0_fixed_b, jeśli x0_fixed_b jest na końcu przedziału
                if (x1_iter > (b - START_POINT_STEP) && x1_iter < (b - START_POINT_STEP) + epsilon) {
                     current_x1 = (b - START_POINT_STEP); // Teoretycznie, można by iść do b, ale wtedy x0==x1
                }
                if (current_x1 > (b - START_POINT_STEP) + epsilon && !(fabs(x0_fixed_b-b) < DBL_EPSILON && fabs(current_x1-b) < DBL_EPSILON) ) break;


                // Unikaj x0 == x1
                if (fabs(x0_fixed_b - current_x1) < DBL_EPSILON * fmax(1.0, fmax(fabs(x0_fixed_b), fabs(current_x1)))) continue;

                // Wywołanie: secantMethod(stałe_x0, iterowane_x1, ...)
                RootResult secant_res_b = secantMethod(x0_fixed_b, current_x1, current_precision, MAX_ITERATIONS, current_criterion_type);
                appendSecantResultToCsv(csv_file, current_criterion_name, x0_fixed_b, current_x1, current_precision, secant_res_b);

                // Sprawdź, czy current_x1 osiągnął prawie b (lub b-krok)
                if (fabs(current_x1 - (b - START_POINT_STEP)) < DBL_EPSILON && (b - START_POINT_STEP) >= a ) break;
                if ( (b - START_POINT_STEP) < a && fabs(current_x1 - a) < DBL_EPSILON) break; // Jeśli b-krok jest poza 'a'
            }
             printf("\n");
        }
        printf("Finished processing for Stop Criterion: %s\n\n", current_criterion_name);
    }

    printf("===================================================================\n");
    printf("Completed root-finding analysis.\n");
    fclose(csv_file);
    printf("Results saved to data/%s\n", results_filename);
    printf("\nIteration heatmap plots will be generated by the Python script for each stopping criterion.\n");
    printf("\n===================================================================\n");
    printf("Analysis and data generation complete.\n");
    printf("Data file saved in: data/\n");
    printf("Gnuplot scripts for function plots saved in: scripts/\n");
    printf("\nTo generate plots:\n");
    printf("1. Ensure Python3 with pandas, matplotlib, seaborn, and numpy is installed.\n");
    printf("   (Activate your virtual environment if used: source .venv/bin/activate)\n");
    printf("2. Run the Python script for tables and heatmaps:\n");
    printf("   python3 src/plot_results.py\n");
    printf("3. Optionally, run Gnuplot for the function plots:\n");
    printf("   gnuplot scripts/plot_function.gp\n");
    printf("   gnuplot scripts/plot_function_zoomed.gp\n");
    printf("4. Alternatively, use the Makefile:\n");
    printf("   make plots   (runs C code, then Python script, then Gnuplot for both function plots)\n");
    printf("   make gnuplot_func_plot (runs only the Gnuplot main function plot)\n");
    printf("   make gnuplot_func_plot_zoomed (runs only the ZOOMED Gnuplot function plot)\n");
    printf("Generated plots (.png, .svg, etc.) will be saved in: plots/\n");
    printf("Generated tables (.txt, .tex) will be saved in: tables/ and tables_latex_format/\n");
    printf("===================================================================\n");
    return 0;
}