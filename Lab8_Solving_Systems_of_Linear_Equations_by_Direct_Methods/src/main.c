#include "common.h"
#include "matrix_utils.h"
#include "experiment.h"
#include "fileio.h" // Changed from output_gen.h
#include <stdio.h>

int main() {
    // --- Setup for Matrix A_I ---
    int sizes_A_I[MAX_N_I - 1]; // N from 2 to MAX_N_I (inclusive)
    int num_sizes_A_I = 0;
    for (int n = 2; n <= MAX_N_I; n++) {
        sizes_A_I[num_sizes_A_I++] = n;
    }
    ExperimentResult* results_A_I_float = (ExperimentResult*)malloc(num_sizes_A_I * sizeof(ExperimentResult));
    ExperimentResult* results_A_I_double = (ExperimentResult*)malloc(num_sizes_A_I * sizeof(ExperimentResult));
    CHECK_ALLOC(results_A_I_float, "results_A_I_float");
    CHECK_ALLOC(results_A_I_double, "results_A_I_double");

    run_experiments_for_matrix("A_I", generate_matrix_I, sizes_A_I, num_sizes_A_I, results_A_I_float, results_A_I_double);
    save_results_to_csv("results_A_I", results_A_I_float, results_A_I_double, sizes_A_I, num_sizes_A_I);
    generate_gnuplot_script_individual("data/results_A_I.csv", "A_I", "plots", "scripts", false);
    generate_latex_table_individual("table_A_I", results_A_I_float, results_A_I_double, sizes_A_I, num_sizes_A_I, "$A_I$", false);

    // --- Setup for Matrix A_II ---
    int sizes_A_II[MAX_M_II -1]; // N from 2 to MAX_M_II (inclusive)
    int num_sizes_A_II = 0;
    for (int m = 2; m <= MAX_M_II; m++) {
        sizes_A_II[num_sizes_A_II++] = m;
    }

    ExperimentResult* results_A_II_float = NULL;
    ExperimentResult* results_A_II_double = NULL;

    if (num_sizes_A_II > 0) {
        results_A_II_float = (ExperimentResult*)malloc(num_sizes_A_II * sizeof(ExperimentResult));
        results_A_II_double = (ExperimentResult*)malloc(num_sizes_A_II * sizeof(ExperimentResult));
        CHECK_ALLOC(results_A_II_float, "results_A_II_float");
        CHECK_ALLOC(results_A_II_double, "results_A_II_double");

        run_experiments_for_matrix("A_II", generate_matrix_II, sizes_A_II, num_sizes_A_II, results_A_II_float, results_A_II_double);
        save_results_to_csv("results_A_II", results_A_II_float, results_A_II_double, sizes_A_II, num_sizes_A_II);
        generate_gnuplot_script_individual("data/results_A_II.csv", "A_II", "plots", "scripts", true); // is_A_II = true
        generate_latex_table_individual("table_A_II", results_A_II_float, results_A_II_double, sizes_A_II, num_sizes_A_II, "$A_{II}$", true); // use_longtable = true

        // --- Comparison Plot and Table ---
        generate_gnuplot_script_comparison("data/results_A_I.csv", "data/results_A_II.csv", "plots", "scripts", MAX_N_I);
        generate_latex_table_comparison("table_cond_compare",
                                        results_A_I_double, results_A_II_double,
                                        sizes_A_I, num_sizes_A_I,
                                        sizes_A_II, num_sizes_A_II,
                                        MAX_N_I);
    } else {
        printf("Skipping A_II experiments as MAX_M_II (%d) is too small for any sizes.\n", MAX_M_II);
    }


    // --- Free allocated memory ---
    free(results_A_I_float);
    free(results_A_I_double);
    if (results_A_II_float) free(results_A_II_float);
    if (results_A_II_double) free(results_A_II_double);

    printf("\nAll experiments complete. Output generated in data/, plots/, scripts/, latex_out/ directories.\n");
    printf("To generate plots, run Gnuplot on scripts in 'scripts/' directory (e.g., gnuplot scripts/plot_A_I.gp).\n");
    printf("Alternatively, use 'make plots' or 'make tables'.\n");

    return 0;
}