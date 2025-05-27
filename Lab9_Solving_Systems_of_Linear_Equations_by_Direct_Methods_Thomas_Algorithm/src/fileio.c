#include "fileio.h"

static void fprint_latex_sci(FILE* f, double val) {
    if (isinf(val)) fprintf(f, "$\\infty$");
    else if (isnan(val)) fprintf(f, "\\texttt{NaN}");
    else fprintf(f, "%.2e", val);
}
static void fprint_latex_fixed(FILE* f, double val, int precision) {
    char fmt[10];
    sprintf(fmt, "%%.%df", precision);
    if (isinf(val)) fprintf(f, "$\\infty$");
    else if (isnan(val)) fprintf(f, "\\texttt{NaN}");
    else fprintf(f, fmt, val);
}

void save_results_to_csv(
    const char* csv_filename_base,
    const ExperimentResult* results_array,
    const int* n_sizes_arr,
    int num_n_values) {

    char filepath[256];
    ensure_dir_exists("data");
    sprintf(filepath, "data/%s.csv", csv_filename_base);

    FILE *f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "ERROR: Opening file %s for CSV writing.\n", filepath);
        return;
    }

    fprintf(f, "# Results: Tridiagonal Matrix (m=%.1f, k=%.1f)\n", M_PARAM, K_PARAM);
    fprintf(f, "N_Size,Err_G_f32,Time_G_f32,Err_T_f32,Time_T_f32,");
    fprintf(f, "Err_G_f64,Time_G_f64,Err_T_f64,Time_T_f64,");
    fprintf(f, "Mem_G_f32_KB,Mem_T_f32_KB,Mem_G_f64_KB,Mem_T_f64_KB\n");

    for (int i = 0; i < num_n_values; i++) {
        const ExperimentResult* res = &results_array[i];
        fprintf(f, "%d,", res->size_n);
        fprintf(f, "%.6e,%.6f,%.6e,%.6f,",
                res->err_gauss_f32, res->time_gauss_f32_sec,
                res->err_thomas_f32, res->time_thomas_f32_sec);
        fprintf(f, "%.6e,%.6f,%.6e,%.6f,",
                res->err_gauss_f64, res->time_gauss_f64_sec,
                res->err_thomas_f64, res->time_thomas_f64_sec);
        fprintf(f, "%.3f,%.3f,%.3f,%.3f\n",
                res->mem_gauss_f32_kb, res->mem_thomas_f32_kb,
                res->mem_gauss_f64_kb, res->mem_thomas_f64_kb);
    }
    fclose(f);
    printf("Results saved to %s\n", filepath);
}


void generate_gnuplot_script(
    const char* csv_filepath,
    const char* gnuplot_script_filename_base,
    const char* plot_filename_base,
    int max_n_for_plots) {

    char script_path[256];
    char plot_base[256];

    ensure_dir_exists("scripts");
    ensure_dir_exists("plots");

    sprintf(script_path, "scripts/%s.gp", gnuplot_script_filename_base);
    sprintf(plot_base, "plots/%s", plot_filename_base);

    FILE *gp = fopen(script_path, "w");
    if (!gp) {
        fprintf(stderr, "ERROR: Creating Gnuplot script %s.\n", script_path);
        return;
    }

    fprintf(gp, "# Gnuplot script for Tridiagonal Matrix (m=%.1f, k=%.1f)\n", M_PARAM, K_PARAM);
    fprintf(gp, "set terminal pngcairo enhanced size 1280,960 font 'Arial,10'\n");
    fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set key top left spacing 1.2 font ',9'\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set xrange [1:%d]\n\n", max_n_for_plots);

    fprintf(gp, "error_zero_replacement = 1e-18\n");
    fprintf(gp, "replace_infnan(col_val) = (isinf(col_val) || isnan(col_val) ? 1/0.0 : col_val)\n");
    fprintf(gp, "replace_error_val(col_val) = (col_val == 0.0 ? error_zero_replacement : (isinf(col_val) || isnan(col_val) ? 1e18 : col_val) )\n\n");


    fprintf(gp, "set output '%s_error.png'\n", plot_base);
    fprintf(gp, "set title 'Max Abs Error vs Size (Tridiagonal, m=%.1f, k=%.1f)'\n", M_PARAM, K_PARAM);
    fprintf(gp, "set ylabel 'Max Absolute Error (log scale)'\n");
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set logscale y\n");
    fprintf(gp, "set format y '10^{%%L}'\n");
    fprintf(gp, "set yrange [error_zero_replacement/10 : 1e20]\n");
    fprintf(gp, "plot '%s' skip 1 u 1:(replace_error_val(column(2))) w lp pt 7 lc 'blue' t 'Gauss Err (f32)', \\\n", csv_filepath);
    fprintf(gp, "     '' skip 1 u 1:(replace_error_val(column(4))) w lp pt 6 lc 'cyan' t 'Thomas Err (f32)', \\\n");
    fprintf(gp, "     '' skip 1 u 1:(replace_error_val(column(6))) w lp pt 5 lc 'red' t 'Gauss Err (f64)', \\\n");
    fprintf(gp, "     '' skip 1 u 1:(replace_error_val(column(8))) w lp pt 4 lc 'magenta' t 'Thomas Err (f64)'\n\n");

    fprintf(gp, "set output '%s_time.png'\n", plot_base);
    fprintf(gp, "set title 'Solve Time vs Size (Tridiagonal, m=%.1f, k=%.1f)'\n", M_PARAM, K_PARAM);
    fprintf(gp, "set ylabel 'Time (seconds, log scale)'\n");
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set logscale y\n");
    fprintf(gp, "set format y '10^{%%L}'\n");
    fprintf(gp, "set yrange [1e-6 : *]\n");
    fprintf(gp, "plot '%s' skip 1 u 1:(replace_infnan(column(3))) w lp pt 7 lc 'blue' t 'Gauss Time (f32)', \\\n", csv_filepath);
    fprintf(gp, "     '' skip 1 u 1:(replace_infnan(column(5))) w lp pt 6 lc 'cyan' t 'Thomas Time (f32)', \\\n");
    fprintf(gp, "     '' skip 1 u 1:(replace_infnan(column(7))) w lp pt 5 lc 'red' t 'Gauss Time (f64)', \\\n");
    fprintf(gp, "     '' skip 1 u 1:(replace_infnan(column(9))) w lp pt 4 lc 'magenta' t 'Thomas Time (f64)'\n\n");

    fprintf(gp, "set output '%s_memory.png'\n", plot_base);
    fprintf(gp, "set title 'Theoretical Matrix A Storage vs Size (Tridiagonal, m=%.1f, k=%.1f)'\n", M_PARAM, K_PARAM);
    fprintf(gp, "set ylabel 'Memory (KB, log scale)'\n");
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set logscale y\n");
    fprintf(gp, "set format y '10^{%%L}'\n");
    fprintf(gp, "set yrange [*:*]\n");
    fprintf(gp, "plot '%s' skip 1 u 1:(replace_infnan(column(10))) w l lc 'blue' t 'Gauss Mem (f32)', \\\n", csv_filepath);
    fprintf(gp, "     '' skip 1 u 1:(replace_infnan(column(11))) w l lc 'cyan' t 'Thomas Mem (f32)', \\\n");
    fprintf(gp, "     '' skip 1 u 1:(replace_infnan(column(12))) w l lc 'red' t 'Gauss Mem (f64)', \\\n");
    fprintf(gp, "     '' skip 1 u 1:(replace_infnan(column(13))) w l lc 'magenta' t 'Thomas Mem (f64)'\n\n");

    fclose(gp);
    printf("Generated Gnuplot script: %s\n", script_path);
}


void generate_latex_table(
    const char* latex_filename_base,
    const ExperimentResult* results_array,
    const int* n_sizes_arr,
    int num_n_values,
    bool use_longtable) {

    char filepath[256];
    ensure_dir_exists("latex_out");
    sprintf(filepath, "latex_out/%s.tex", latex_filename_base);

    FILE* f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "ERROR: Opening LaTeX file %s for writing.\n", filepath);
        return;
    }

    fprintf(f, "%% LaTeX table for Tridiagonal Matrix (m=%.1f, k=%.1f)\n", M_PARAM, K_PARAM);
    const char* table_env = use_longtable ? "longtable" : "tabular";
    const char* caption_label_format = use_longtable ?
        "\\caption{Wyniki dla macierzy trójdiagonalnej ($m=%.1f, k=%.1f$) \\label{tab:%s}} \\\\" :
        "\\caption{Wybrane wyniki dla macierzy trójdiagonalnej ($m=%.1f, k=%.1f$) \\label{tab:%s}}";

    char caption_buffer[200];
    sprintf(caption_buffer, caption_label_format, M_PARAM, K_PARAM, latex_filename_base);


    if (use_longtable) {
        fprintf(f, "\\begin{longtable}{c|cc|cc||cc|cc}\n");
    } else {
        fprintf(f, "\\begin{table}[htbp]\n\\centering\n");
        fprintf(f, "\\resizebox{\\textwidth}{!}{\n");
        fprintf(f, "\\begin{tabular}{c|cc|cc||cc|cc}\n");
    }

    fprintf(f, "%s\n", caption_buffer);
    fprintf(f, "\\toprule\n");
    fprintf(f, "& \\multicolumn{4}{c||}{Precyzja float32} & \\multicolumn{4}{c}{Precyzja float64} \\\\\n");
    fprintf(f, "\\cmidrule(lr){2-5} \\cmidrule(lr){6-9}\n");
    fprintf(f, "N & \\multicolumn{2}{c|}{Błąd Maks.} & \\multicolumn{2}{c||}{Czas [s]} & \\multicolumn{2}{c|}{Błąd Maks.} & \\multicolumn{2}{c}{Czas [s]} \\\\\n");
    fprintf(f, "& Gauss & Thomas & Gauss & Thomas & Gauss & Thomas & Gauss & Thomas \\\\\n");
    fprintf(f, "\\midrule\n");

    if (use_longtable) {
        fprintf(f, "\\endfirsthead\n");
        fprintf(f, "\\caption[]{-- ciąg dalszy.} \\\\\n\\toprule\n");
        fprintf(f, "& \\multicolumn{4}{c||}{Precyzja float32} & \\multicolumn{4}{c}{Precyzja float64} \\\\\n");
        fprintf(f, "\\cmidrule(lr){2-5} \\cmidrule(lr){6-9}\n");
        fprintf(f, "N & \\multicolumn{2}{c|}{Błąd Maks.} & \\multicolumn{2}{c||}{Czas [s]} & \\multicolumn{2}{c|}{Błąd Maks.} & \\multicolumn{2}{c}{Czas [s]} \\\\\n");
        fprintf(f, "& Gauss & Thomas & Gauss & Thomas & Gauss & Thomas & Gauss & Thomas \\\\\n");
        fprintf(f, "\\midrule\n\\endhead\n");
        fprintf(f, "\\midrule\n\\multicolumn{9}{r}{\\textit{ciąg dalszy na następnej stronie}} \\\\\n\\endfoot\n");
        fprintf(f, "\\bottomrule\n\\endlastfoot\n");
    }

    int print_Ns_short[] = {2, 10, 50, 100, 200, 300, 400, 500, -1};
    int current_target_idx = 0;

    for (int i = 0; i < num_n_values; i++) {
        const ExperimentResult* res = &results_array[i];
        bool should_print = use_longtable;
        if (!use_longtable) {
            if (print_Ns_short[current_target_idx] != -1 && print_Ns_short[current_target_idx] == res->size_n) {
                should_print = true;
                current_target_idx++;
            }
        }

        if (should_print) {
            fprintf(f, "%d & ", res->size_n);
            fprint_latex_sci(f, res->err_gauss_f32); fprintf(f, " & ");
            fprint_latex_sci(f, res->err_thomas_f32); fprintf(f, " & ");
            fprint_latex_fixed(f, res->time_gauss_f32_sec, 4); fprintf(f, " & ");
            fprint_latex_fixed(f, res->time_thomas_f32_sec, 4); fprintf(f, " & ");

            fprint_latex_sci(f, res->err_gauss_f64); fprintf(f, " & ");
            fprint_latex_sci(f, res->err_thomas_f64); fprintf(f, " & ");
            fprint_latex_fixed(f, res->time_gauss_f64_sec, 4); fprintf(f, " & ");
            fprint_latex_fixed(f, res->time_thomas_f64_sec, 4);
            fprintf(f, " \\\\\n");
        }
    }

    if (use_longtable) {
        // \end{longtable} is part of \endlastfoot
    } else {
        fprintf(f, "\\bottomrule\n\\end{tabular}\n}\n\\end{table}\n");
    }
    fclose(f);
    printf("Generated LaTeX table: %s\n", filepath);
}