#include "common.h" // To dołączy memory_tracker.h (który dołączy stdlib.h przed makrami)
#include "experiment.h"
#include "fileio.h"

int main() {
    // Inicjalizacja trackera pamięci globalnie na początku programu.
    // run_all_experiments również robi init, ale to jest dobre miejsce.
    mem_tracker_init();

    printf("Starting Tridiagonal Solver Experiments (m=%.1f, k=%.1f)\n", M_PARAM, K_PARAM);

    int n_values_count = 0;
    if (N_MAX >= N_MIN) {
        n_values_count = N_MAX - N_MIN + 1;
    }

    int* n_sizes_for_exp = NULL;
    if (n_values_count > 0) {
        // Wywołanie malloc tutaj użyje my_malloc_proxy, jeśli MEMORY_TRACKING_ENABLED
        n_sizes_for_exp = (int*)malloc(n_values_count * sizeof(int));
        CHECK_ALLOC(n_sizes_for_exp, "N sizes array for experiments");
        for (int i = 0; i < n_values_count; ++i) {
            n_sizes_for_exp[i] = N_MIN + i;
        }
    }

    ExperimentResult* all_results = NULL;
    if (n_values_count > 0) {
         all_results = (ExperimentResult*)malloc(n_values_count * sizeof(ExperimentResult));
         CHECK_ALLOC(all_results, "Main results array");
    }

    if (n_values_count > 0) {
        run_all_experiments(n_sizes_for_exp, n_values_count, all_results);
    } else {
        printf("No N values to run experiments for (N_MIN > N_MAX or N_MAX too small).\n");
    }

    char filename_base[100];
    // Upewnij się, że nazwa pliku jest unikalna, jeśli testujesz różne wersje
    sprintf(filename_base, "tridiag_m%.0f_k%.0f_results_dynmem_v2", M_PARAM, K_PARAM);

    if (n_values_count > 0) {
        save_results_to_csv(filename_base, all_results, n_values_count);

        char csv_full_path[128];
        sprintf(csv_full_path, "data/%s.csv", filename_base);

        char gnuplot_script_base[100];
        sprintf(gnuplot_script_base, "plot_tridiag_m%.0f_k%.0f_dynmem_v2", M_PARAM, K_PARAM);
        char plot_file_base[100];
        sprintf(plot_file_base, "tridiag_m%.0f_k%.0f_dynmem_v2", M_PARAM, K_PARAM);

        generate_gnuplot_scripts(csv_full_path, gnuplot_script_base, plot_file_base, N_MAX);

        char latex_table_base[100];
        sprintf(latex_table_base, "table_tridiag_m%.0f_k%.0f_dynmem_v2", M_PARAM, K_PARAM);
        generate_latex_table(latex_table_base, all_results, n_values_count, false);

        printf("\nProcessing complete.\n");
        printf("CSV data in: data/\n");
        printf("Gnuplot scripts in: scripts/ (run e.g., `gnuplot scripts/%s_error.gp`)\n", gnuplot_script_base);
        printf("Plots will be generated in: plots/\n");
        printf("LaTeX tables in: latex_out/\n");

    } else {
        printf("\nProcessing complete (no experiments run).\n");
    }


    if (n_sizes_for_exp) free(n_sizes_for_exp);
    if (all_results) free(all_results);


    mem_tracker_print_stats("PROGRAM_END");
    if (g_current_memory_usage != 0) {
        fprintf(stderr, "FATAL_MEM_LEAK: Program ended with %zu bytes still allocated!\n", g_current_memory_usage);
        return 1;
    }
    if (n_values_count > 0 && g_total_allocations != g_total_deallocations) {
         fprintf(stderr, "FATAL_MEM_LEAK: Program ended with alloc/dealloc mismatch (%zu vs %zu)!\n", g_total_allocations, g_total_deallocations);
        return 1;
    }

    return 0;
}