#include "common.h"
#include "experiment.h"
#include "fileio.h"

int main() {
    printf("Starting Tridiagonal Solver Experiments (m=%.1f, k=%.1f)\n", M_PARAM, K_PARAM);

    int n_values_count = N_MAX - N_MIN + 1;
    int* n_sizes_for_exp = (int*)malloc(n_values_count * sizeof(int));
    CHECK_ALLOC(n_sizes_for_exp, "N sizes array for experiments");
    for (int i = 0; i < n_values_count; ++i) {
        n_sizes_for_exp[i] = N_MIN + i;
    }

    ExperimentResult* all_results = (ExperimentResult*)malloc(n_values_count * sizeof(ExperimentResult));
    CHECK_ALLOC(all_results, "Main results array");

    run_all_experiments(n_sizes_for_exp, n_values_count, all_results);

    char filename_base[100];
    sprintf(filename_base, "tridiag_m%.0f_k%.0f_results", M_PARAM, K_PARAM);

    save_results_to_csv(filename_base, all_results, n_values_count);

    char csv_full_path[128];
    sprintf(csv_full_path, "data/%s.csv", filename_base);

    char gnuplot_script_base[100];
    sprintf(gnuplot_script_base, "plot_tridiag_m%.0f_k%.0f", M_PARAM, K_PARAM);
    char plot_file_base[100];
    sprintf(plot_file_base, "tridiag_m%.0f_k%.0f", M_PARAM, K_PARAM);

    generate_gnuplot_script(csv_full_path, gnuplot_script_base, plot_file_base, N_MAX);

    char latex_table_base[100];
    sprintf(latex_table_base, "table_tridiag_m%.0f_k%.0f", M_PARAM, K_PARAM);
    generate_latex_table(latex_table_base, all_results, n_values_count, false); // Short table

    // For a long table, you can call it again with a different filename suffix and true
    // char latex_table_base_long[110];
    // sprintf(latex_table_base_long, "%s_long", latex_table_base);
    // generate_latex_table(latex_table_base_long, all_results, n_sizes_for_exp, n_values_count, true);


    free(n_sizes_for_exp);
    free(all_results);

    printf("\nProcessing complete.\n");
    printf("CSV data in: data/\n");
    printf("Gnuplot scripts in: scripts/ (run e.g., `gnuplot scripts/%s.gp`)\n", gnuplot_script_base);
    printf("Plots will be generated in: plots/\n");
    printf("LaTeX tables in: latex_out/\n");

    return 0;
}