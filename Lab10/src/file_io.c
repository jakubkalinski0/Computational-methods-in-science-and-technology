#include "../include/file_io.h"
#include <sys/stat.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#endif

void ensure_directory_exists(const char* path) {
    int ret;
#ifdef _WIN32
    ret = _mkdir(path);
#else
    ret = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    if (ret == 0) {
        // printf("Directory created: %s\n", path);
    } else if (errno == EEXIST) {
        // Directory already exists, which is fine.
    } else {
        fprintf(stderr, "Error creating directory %s: %s\n", path, strerror(errno));
    }
}

void open_csv_file(FILE** fp, const char* filename_base, const char* sub_dir) {
    char filepath[512];
    char full_dir[256];

    if (sub_dir && strlen(sub_dir) > 0) {
        sprintf(full_dir, "data/%s", sub_dir);
    } else {
        strcpy(full_dir, "data");
    }
    ensure_directory_exists(full_dir);

    sprintf(filepath, "%s/%s.csv", full_dir, filename_base);
    *fp = fopen(filepath, "w");
    if (*fp == NULL) {
        fprintf(stderr, "Error opening CSV file %s for writing.\n", filepath);
        return;
    }
    fprintf(*fp, "N,X0_Type,Criterion_Type,Rho,Iterations,Time_Solve_sec,Time_Per_Iter_sec,Final_Abs_Error_Max,Final_Criterion_Metric\n");
}

void append_to_csv_file(FILE* fp, const ExperimentData* data) {
    if (fp == NULL || data == NULL) return;
    fprintf(fp, "%d,%d,%d,%.16e,%d,%.10f,%.10e,%.16e,%.16e\n",
            data->n,
            (int)data->x0_type,
            (int)data->criterion_type,
            data->rho,
            data->iterations_taken,
            data->time_solve_sec,
            data->time_per_iteration_sec,
            data->final_abs_error,
            data->final_criterion_metric);
}

void close_csv_file(FILE* fp) {
    if (fp) fclose(fp);
}

void save_spectral_radius_to_csv(const char* filename_base, const int* n_values, const double* sr_values, int num_n_values) {
    char filepath[256];
    ensure_directory_exists("data");
    sprintf(filepath, "data/%s.csv", filename_base);
    FILE* fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "Error opening spectral radius CSV %s\n", filepath);
        return;
    }
    fprintf(fp, "N,SpectralRadius\n");
    for (int i = 0; i < num_n_values; i++) {
        if (isnan(sr_values[i])) {
            fprintf(fp, "%d,NaN\n", n_values[i]);
        } else {
            fprintf(fp, "%d,%.15f\n", n_values[i], sr_values[i]);
        }
    }
    fclose(fp);
    printf("Spectral radius data saved to %s\n", filepath);
}

const char* get_x0_type_string(InitialVectorType type) {
    switch (type) {
        case X0_ZEROS: return "Zeros";
        case X0_ONES: return "Ones";
        case X0_RANDOM_LOW: return "RandLow";
        case X0_ALTERNATING_HIGH: return "AltHigh";
        default: return "Unknown";
    }
}

const char* get_criterion_type_string(StopCriterionType type) {
    switch (type) {
        case INCREMENT_CRITERION: return "Increment";
        case RESIDUAL_CRITERION: return "Residual";
        default: return "Unknown";
    }
}


void generate_gnuplot_scripts_for_n(
    const char* csv_filename_base,
    int n_val,
    const InitialVectorType* x0_types, int num_x0_types,
    const StopCriterionType* criterion_types_arr, int num_criterion_types
) {
    char script_filepath[512];
    char plot_output_path[512];
    char csv_data_filepath[512];

    ensure_directory_exists("scripts");
    ensure_directory_exists("plots");

    char plot_subdir_for_n[256];
    sprintf(plot_subdir_for_n, "plots/N_%d", n_val);
    ensure_directory_exists(plot_subdir_for_n);

    sprintf(csv_data_filepath, "data/%s.csv", csv_filename_base);

    sprintf(script_filepath, "scripts/plot_N%d_iterations.gp", n_val);
    FILE* gp_iter = fopen(script_filepath, "w");
    if (!gp_iter) { fprintf(stderr, "Error creating Gnuplot script %s\n", script_filepath); return; }
    sprintf(plot_output_path, "%s/N%d_iterations.png", plot_subdir_for_n, n_val);
    fprintf(gp_iter, "# Gnuplot: Iterations vs Rho for N=%d from %s\n", n_val, csv_data_filepath);
    fprintf(gp_iter, "set terminal pngcairo enhanced size 1000,750 font 'Arial,10'\n");
    fprintf(gp_iter, "set output '%s'\n", plot_output_path);
    fprintf(gp_iter, "set title 'Jacobi: Iterations vs Tolerance (N=%d)'\n", n_val);
    fprintf(gp_iter, "set xlabel 'Tolerance (rho)'\n");
    fprintf(gp_iter, "set ylabel 'Number of Iterations'\n");
    fprintf(gp_iter, "set logscale x\n");
    fprintf(gp_iter, "set logscale y\n");
    fprintf(gp_iter, "set datafile separator ','\n");
    fprintf(gp_iter, "set key outside right top\n");
    fprintf(gp_iter, "set grid\n");
    fprintf(gp_iter, "plot \\\n");
    int plot_count = 0;
    for (int x0_idx = 0; x0_idx < num_x0_types; x0_idx++) {
        for (int crit_idx = 0; crit_idx < num_criterion_types; crit_idx++) {
            if (plot_count > 0) fprintf(gp_iter, ", \\\n");
            fprintf(gp_iter, "     '%s' using ($1==%d && $2==%d && $3==%d ? $4 : 1/0):5 with linespoints title sprintf(\"X0:%s Crit:%s\")",
                    csv_data_filepath, n_val, (int)x0_types[x0_idx], (int)criterion_types_arr[crit_idx],
                    get_x0_type_string(x0_types[x0_idx]), get_criterion_type_string(criterion_types_arr[crit_idx]));
            plot_count++;
        }
    }
    if (plot_count == 0) fprintf(gp_iter, "NaN title 'No data'\n");
    else fprintf(gp_iter, "\n");
    fclose(gp_iter);

    sprintf(script_filepath, "scripts/plot_N%d_abs_error.gp", n_val);
    FILE* gp_err = fopen(script_filepath, "w");
    if (!gp_err) { fprintf(stderr, "Error creating Gnuplot script %s\n", script_filepath); return; }
    sprintf(plot_output_path, "%s/N%d_abs_error.png", plot_subdir_for_n, n_val);
    fprintf(gp_err, "# Gnuplot: Final Absolute Error vs Rho for N=%d from %s\n", n_val, csv_data_filepath);
    fprintf(gp_err, "set terminal pngcairo enhanced size 1000,750 font 'Arial,10'\n");
    fprintf(gp_err, "set output '%s'\n", plot_output_path);
    fprintf(gp_err, "set title 'Jacobi: Max Absolute Error vs Tolerance (N=%d)'\n", n_val);
    fprintf(gp_err, "set xlabel 'Tolerance (rho)'\n");
    fprintf(gp_err, "set ylabel 'Max Absolute Error ||x_{comp} - x_{true}||_{max}'\n");
    fprintf(gp_err, "set logscale x\n");
    fprintf(gp_err, "set logscale y\n");
    fprintf(gp_err, "set datafile separator ','\n");
    fprintf(gp_err, "set key outside right top\n");
    fprintf(gp_err, "set grid\n");
    fprintf(gp_err, "plot \\\n");
    plot_count = 0;
    for (int x0_idx = 0; x0_idx < num_x0_types; x0_idx++) {
        for (int crit_idx = 0; crit_idx < num_criterion_types; crit_idx++) {
            if (plot_count > 0) fprintf(gp_err, ", \\\n");
            fprintf(gp_err, "     '%s' using ($1==%d && $2==%d && $3==%d ? $4 : 1/0):8 with linespoints title sprintf(\"X0:%s Crit:%s\")",
                    csv_data_filepath, n_val, (int)x0_types[x0_idx], (int)criterion_types_arr[crit_idx],
                    get_x0_type_string(x0_types[x0_idx]), get_criterion_type_string(criterion_types_arr[crit_idx]));
            plot_count++;
        }
    }
    if (plot_count == 0) fprintf(gp_err, "NaN title 'No data'\n");
    else fprintf(gp_err, "\n");
    fclose(gp_err);

    sprintf(script_filepath, "scripts/plot_N%d_time_per_iter.gp", n_val);
    FILE* gp_time = fopen(script_filepath, "w");
    if (!gp_time) { fprintf(stderr, "Error creating Gnuplot script %s\n", script_filepath); return; }
    sprintf(plot_output_path, "%s/N%d_time_per_iter.png", plot_subdir_for_n, n_val);
    fprintf(gp_time, "# Gnuplot: Avg Time per Iteration vs Rho for N=%d from %s\n", n_val, csv_data_filepath);
    fprintf(gp_time, "set terminal pngcairo enhanced size 1000,750 font 'Arial,10'\n");
    fprintf(gp_time, "set output '%s'\n", plot_output_path);
    fprintf(gp_time, "set title 'Jacobi: Avg. Time per Iteration vs Tolerance (N=%d)'\n", n_val);
    fprintf(gp_time, "set xlabel 'Tolerance (rho)'\n");
    fprintf(gp_time, "set ylabel 'Time per Iteration (s)'\n");
    fprintf(gp_time, "set logscale x\n");
    fprintf(gp_time, "set datafile separator ','\n");
    fprintf(gp_time, "set key outside right top\n");
    fprintf(gp_time, "set grid\n");
    fprintf(gp_time, "plot \\\n");
    plot_count = 0;
    for (int x0_idx = 0; x0_idx < num_x0_types; x0_idx++) {
        for (int crit_idx = 0; crit_idx < num_criterion_types; crit_idx++) {
            if (plot_count > 0) fprintf(gp_time, ", \\\n");
            fprintf(gp_time, "     '%s' using ($1==%d && $2==%d && $3==%d ? $4 : 1/0):7 with linespoints title sprintf(\"X0:%s Crit:%s\")",
                    csv_data_filepath, n_val, (int)x0_types[x0_idx], (int)criterion_types_arr[crit_idx],
                    get_x0_type_string(x0_types[x0_idx]), get_criterion_type_string(criterion_types_arr[crit_idx]));
            plot_count++;
        }
    }
    if (plot_count == 0) fprintf(gp_time, "NaN title 'No data'\n");
    else fprintf(gp_time, "\n");
    fclose(gp_time);

    if (n_val % 100 == 0 || n_val == 50 ) { // Log for selected N values
      printf("Generated Gnuplot scripts for N=%d in scripts/ and plots in %s/\n", n_val, plot_subdir_for_n);
    }
}

void generate_gnuplot_script_spectral_radius(const char* csv_filename_base) {
    char script_filepath[256];
    char csv_filepath[256];

    ensure_directory_exists("scripts");
    ensure_directory_exists("plots");

    sprintf(csv_filepath, "data/%s.csv", csv_filename_base);
    sprintf(script_filepath, "scripts/plot_%s.gp", csv_filename_base);

    FILE* gp = fopen(script_filepath, "w");
    if (!gp) {
        fprintf(stderr, "Error creating Gnuplot script %s\n", script_filepath);
        return;
    }
    fprintf(gp, "# Gnuplot script for spectral radius from %s\n", csv_filepath);
    fprintf(gp, "set terminal pngcairo enhanced size 1000,700 font 'Arial,10'\n");
    fprintf(gp, "set output 'plots/%s.png'\n", csv_filename_base);
    fprintf(gp, "set title 'Spectral Radius of Jacobi Iteration Matrix vs. System Size N'\n");
    fprintf(gp, "set xlabel 'System Size (N)'\n");
    fprintf(gp, "set ylabel 'Spectral Radius rho(T_J)'\n");
    fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set key top left\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set yrange [0:1.1]\n");
    fprintf(gp, "plot '%s' using 1:2 with linespoints pt 7 lc rgb 'blue' title 'Spectral Radius rho(T_J)'\n", csv_filepath);
    fprintf(gp, "\n");
    fclose(gp);
    printf("Gnuplot script for spectral radius generated: %s\n", script_filepath);
}

static void fprint_latex_sci_e(FILE* f, double val) {
    if (isinf(val)) fprintf(f, "$\\infty$");
    else if (isnan(val)) fprintf(f, "NaN");
    else if (val == 0.0 && signbit(val) == 0) fprintf(f, "0.0"); // Handle +0.0
    else if (val == 0.0 && signbit(val) != 0) fprintf(f, "-0.0"); // Handle -0.0 if necessary, though sci usually handles
    else fprintf(f, "%.2e", val);
}

void generate_latex_table_selected_results(
    const char* base_filename_tex,
    const ExperimentData* all_results,
    int total_results_count,
    const char* table_caption,
    int target_n,
    const double* target_rhos, int num_target_rhos
    ) {
    char filepath[512];
    ensure_directory_exists("latex_out");
    sprintf(filepath, "latex_out/%s_N%d.tex", base_filename_tex, target_n);

    FILE* f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "Error opening LaTeX file %s for writing.\n", filepath);
        return;
    }

    fprintf(f, "%% LaTeX table: %s (N=%d)\n", table_caption, target_n);
    fprintf(f, "\\begin{table}[H]\n");
    fprintf(f, "\\centering\n");
    fprintf(f, "\\caption{%s for N=%d \\label{tab:%s_N%d}}\n", table_caption, target_n, base_filename_tex, target_n);
    fprintf(f, "\\resizebox{\\textwidth}{!}{%%\n");
    fprintf(f, "\\begin{tabular}{lcrccc}\n");
    fprintf(f, "\\toprule\n");
    fprintf(f, "$x_0$ Type & Criterion & $\\rho$ & Iterations & Error ($||x-x_{true}||_{\\infty}$) & $T_{iter}$ (s) \\\\\n");
    fprintf(f, "\\midrule\n");

    int results_in_table = 0;
    for (int i = 0; i < total_results_count; i++) {
        const ExperimentData* r = &all_results[i];
        if (r->n == target_n) {
            int rho_match = 0;
            if (num_target_rhos == 0) {
                rho_match = 1;
            } else {
                for(int rho_idx = 0; rho_idx < num_target_rhos; rho_idx++) {
                    if (fabs(r->rho - target_rhos[rho_idx]) < (target_rhos[rho_idx] * 1e-9 + 1e-18) ) {
                        rho_match = 1;
                        break;
                    }
                }
            }
            if (!rho_match) continue;

            fprintf(f, "%s & %s & ",
                    get_x0_type_string(r->x0_type),
                    get_criterion_type_string(r->criterion_type));
            fprint_latex_sci_e(f, r->rho); fprintf(f, " & ");
            if (r->iterations_taken >= JACOBI_MAX_ITER && JACOBI_MAX_ITER > 0) {
                 fprintf(f, "$%d^*$ & ", r->iterations_taken);
            } else if (r->iterations_taken < 0 && r->iterations_taken != -1) { // Błąd solvera inny niż SR skip
                 fprintf(f, "Err(%d) & ", r->iterations_taken);
            } else if (r->iterations_taken == -1) { // Specjalnie dla SR skip
                 fprintf(f, "N/A (SR) & ");
            }
            else {
                 fprintf(f, "%d & ", r->iterations_taken);
            }
            fprint_latex_sci_e(f, r->final_abs_error); fprintf(f, " & ");
            fprint_latex_sci_e(f, r->time_per_iteration_sec);
            fprintf(f, " \\\\\n");
            results_in_table++;
        }
    }
    if (results_in_table == 0) {
        fprintf(f, "\\multicolumn{6}{c}{No data for N=%d with specified rho values.} \\\\\n", target_n);
    }

    fprintf(f, "\\bottomrule\n");
    fprintf(f, "\\end{tabular}\n");
    fprintf(f, "}\n");
    fprintf(f, "\\end{table}\n");
    fclose(f);
    if (target_n % 100 == 0 || target_n == 50) {
        printf("LaTeX table generated: %s\n", filepath);
    }
}


void generate_latex_table_spectral_radius(const char* base_filename_tex,
                                          const int* n_values,
                                          const double* sr_values,
                                          int num_n_values) {
    char filepath[256];
    ensure_directory_exists("latex_out");
    sprintf(filepath, "latex_out/%s.tex", base_filename_tex);

    FILE* f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "Error opening LaTeX file %s for writing.\n", filepath);
        return;
    }
    fprintf(f, "%% LaTeX table for Spectral Radius\n");
    fprintf(f, "\\begin{table}[H]\n");
    fprintf(f, "\\centering\n");
    fprintf(f, "\\caption{Spectral Radius $\\rho(T_J)$ of Jacobi Iteration Matrix vs. System Size N \\label{tab:%s}}\n", base_filename_tex);
    fprintf(f, "\\begin{tabular}{cc}\n");
    fprintf(f, "\\toprule\n");
    fprintf(f, "System Size (N) & Spectral Radius $\\rho(T_J)$ \\\\\n");
    fprintf(f, "\\midrule\n");

    for (int i = 0; i < num_n_values; i++) {
        if (isnan(sr_values[i])) {
            fprintf(f, "%d & NaN \\\\\n", n_values[i]);
        } else {
            fprintf(f, "%d & %.6f \\\\\n", n_values[i], sr_values[i]);
        }
    }

    fprintf(f, "\\bottomrule\n");
    fprintf(f, "\\end{tabular}\n");
    fprintf(f, "\\end{table}\n");
    fclose(f);
    printf("LaTeX spectral radius table generated: %s\n", filepath);
}