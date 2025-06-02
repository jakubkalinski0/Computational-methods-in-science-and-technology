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
    int num_n_values) {

    char filepath[256];
    ensure_dir_exists("data");
    sprintf(filepath, "data/%s.csv", csv_filename_base);

    FILE *f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "ERROR: Opening file %s for CSV writing.\n", filepath);
        return;
    }

    fprintf(f, "Tridiagonal Matrix (m=%.1f, k=%.1f)\n", M_PARAM, K_PARAM);
    fprintf(f, "N_Size,");
    fprintf(f, "Err_G_f32,Time_G_f32,Mem_G_f32_KB,"); // Gauss f32
    fprintf(f, "Err_TB_f32,Time_TB_f32,Mem_TB_f32_KB,"); // Thomas Banded f32
    fprintf(f, "Err_TF_f32,Time_TF_f32,Mem_TF_f32_KB,"); // Thomas Full f32
    fprintf(f, "Err_G_f64,Time_G_f64,Mem_G_f64_KB,");   // Gauss f64
    fprintf(f, "Err_TB_f64,Time_TB_f64,Mem_TB_f64_KB,"); // Thomas Banded f64
    fprintf(f, "Err_TF_f64,Time_TF_f64,Mem_TF_f64_KB\n"); // Thomas Full f64

    for (int i = 0; i < num_n_values; i++) {
        const ExperimentResult* res = &results_array[i];
        fprintf(f, "%d,", res->size_n);
        // float32
        fprintf(f, "%.6e,%.6f,%.3f,",
                res->err_gauss_f32, res->time_gauss_f32_sec, res->mem_gauss_f32_kb);
        fprintf(f, "%.6e,%.6f,%.3f,",
                res->err_thomas_banded_f32, res->time_thomas_banded_f32_sec, res->mem_thomas_banded_f32_kb);
        fprintf(f, "%.6e,%.6f,%.3f,",
                res->err_thomas_full_f32, res->time_thomas_full_f32_sec, res->mem_thomas_full_f32_kb);
        // float64
        fprintf(f, "%.6e,%.6f,%.3f,",
                res->err_gauss_f64, res->time_gauss_f64_sec, res->mem_gauss_f64_kb);
        fprintf(f, "%.6e,%.6f,%.3f,",
                res->err_thomas_banded_f64, res->time_thomas_banded_f64_sec, res->mem_thomas_banded_f64_kb);
        fprintf(f, "%.6e,%.6f,%.3f\n",
                res->err_thomas_full_f64, res->time_thomas_full_f64_sec, res->mem_thomas_full_f64_kb);
    }
    fclose(f);
    printf("Results saved to %s\n", filepath);
}

// Helper function to generate common Gnuplot settings
static void generate_gnuplot_common_settings(FILE* gp, const char* csv_filepath, int max_n_for_plots) {
    fprintf(gp, "# Gnuplot script for Tridiagonal Matrix (m=%.1f, k=%.1f)\n", M_PARAM, K_PARAM);
    fprintf(gp, "set terminal pngcairo enhanced size 1280,960 font 'Arial,10'\n");
    fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set decimalsign '.' # Ensure correct decimal separator for CSV\n");
    fprintf(gp, "set key outside top center horizontal\n"); // horizontal for more legend items
    fprintf(gp, "set grid\n");
    fprintf(gp, "set xrange [1:%d]\n\n", max_n_for_plots);
    fprintf(gp, "datafile = '%s'\n\n", csv_filepath);
}


void generate_gnuplot_scripts(
    const char* csv_filepath,
    const char* gnuplot_script_filename_base,
    const char* plot_filename_base,
    int max_n_for_plots) {

    char script_path_error[256], script_path_time[256], script_path_memory[256];
    char plot_file_error[256], plot_file_time[256], plot_file_memory[256];

    ensure_dir_exists("scripts");
    ensure_dir_exists("plots");

    sprintf(script_path_error, "scripts/%s_error.gp", gnuplot_script_filename_base);
    sprintf(script_path_time, "scripts/%s_time.gp", gnuplot_script_filename_base);
    sprintf(script_path_memory, "scripts/%s_memory.gp", gnuplot_script_filename_base);

    sprintf(plot_file_error, "plots/%s_error.png", plot_filename_base);
    sprintf(plot_file_time, "plots/%s_time.png", plot_filename_base);
    sprintf(plot_file_memory, "plots/%s_memory.png", plot_filename_base);

    // CSV Column indices (1-based for Gnuplot, after N_Size which is col 1)
    // N_Size (1)
    // Err_G_f32 (2), Time_G_f32 (3), Mem_G_f32_KB (4)
    // Err_TB_f32 (5), Time_TB_f32 (6), Mem_TB_f32_KB (7)
    // Err_TF_f32 (8), Time_TF_f32 (9), Mem_TF_f32_KB (10)
    // Err_G_f64 (11), Time_G_f64 (12), Mem_G_f64_KB (13)
    // Err_TB_f64 (14), Time_TB_f64 (15), Mem_TB_f64_KB (16)
    // Err_TF_f64 (17), Time_TF_f64 (18), Mem_TF_f64_KB (19)

    // --- Error Plot Script ---
    FILE *gp_err = fopen(script_path_error, "w");
    if (!gp_err) {
        fprintf(stderr, "ERROR: Creating Gnuplot script %s.\n", script_path_error);
    } else {
        generate_gnuplot_common_settings(gp_err, csv_filepath, max_n_for_plots);
        const char* error_zero_replacement_val_str = "1e-18";
        fprintf(gp_err, "error_zero_replacement = %s\n", error_zero_replacement_val_str);
        fprintf(gp_err, "replace_error_val(col_val) = (col_val == 0.0 ? error_zero_replacement : col_val)\n\n");

        fprintf(gp_err, "set output '%s'\n", plot_file_error);
        fprintf(gp_err, "set title 'Max Abs Error vs Size (Tridiagonal, m=%.1f, k=%.1f)'\n", M_PARAM, K_PARAM);
        fprintf(gp_err, "set ylabel 'Max Absolute Error (log scale)'\n");
        fprintf(gp_err, "set xlabel 'Matrix Size (N)'\n");
        fprintf(gp_err, "set logscale y\n");
        fprintf(gp_err, "set format y '10^{%%L}'\n");
        fprintf(gp_err, "set yrange [error_zero_replacement/10 : 1e20]\n"); // Adjusted yrange if needed
        fprintf(gp_err, "plot datafile skip 1 u 1:(replace_error_val(column(2))) w p pt 7 ps 0.8 lc 'blue' t 'Gauss Err (f32)', \\\n");
        fprintf(gp_err, "     '' skip 1 u 1:(replace_error_val(column(5))) w p pt 6 ps 0.8 lc 'cyan' t 'Thomas Banded Err (f32)', \\\n");
        fprintf(gp_err, "     '' skip 1 u 1:(replace_error_val(column(8))) w p pt 2 ps 0.8 lc 'dark-turquoise' t 'Thomas Full Err (f32)', \\\n");
        fprintf(gp_err, "     '' skip 1 u 1:(replace_error_val(column(11))) w p pt 5 ps 0.8 lc 'red' t 'Gauss Err (f64)', \\\n");
        fprintf(gp_err, "     '' skip 1 u 1:(replace_error_val(column(14))) w p pt 4 ps 0.8 lc 'magenta' t 'Thomas Banded Err (f64)', \\\n");
        fprintf(gp_err, "     '' skip 1 u 1:(replace_error_val(column(17))) w p pt 1 ps 0.8 lc 'purple' t 'Thomas Full Err (f64)'\n\n");
        fclose(gp_err);
        printf("Generated Gnuplot script: %s\n", script_path_error);
    }

    // --- Time Plot Script (Log Scale) ---
    FILE *gp_time = fopen(script_path_time, "w");
    if (!gp_time) {
        fprintf(stderr, "ERROR: Creating Gnuplot script %s.\n", script_path_time);
    } else {
        generate_gnuplot_common_settings(gp_time, csv_filepath, max_n_for_plots);
        fprintf(gp_time, "min_plot_log_val = 1e-7 # Value to substitute for non-positive values\n\n");

        fprintf(gp_time, "set output '%s'\n", plot_file_time);
        fprintf(gp_time, "set title 'Solve Time vs Size (Tridiagonal, m=%.1f, k=%.1f)'\n", M_PARAM, K_PARAM);
        fprintf(gp_time, "set ylabel 'Time (seconds, log scale)'\n");
        fprintf(gp_time, "set xlabel 'Matrix Size (N)'\n");
        fprintf(gp_time, "set logscale y\n");
        fprintf(gp_time, "set format y '10^{%%L}'\n");
        fprintf(gp_time, "set yrange [min_plot_log_val : 1e1]\n"); // Adjusted yrange if needed

        fprintf(gp_time, "plot datafile skip 1 u 1:(column(3) > 0 ? column(3) : min_plot_log_val) w p pt 7 ps 0.8 lc 'blue' t 'Gauss Time (f32)', \\\n");
        fprintf(gp_time, "     '' skip 1 u 1:(column(6) > 0 ? column(6) : min_plot_log_val) w p pt 6 ps 0.8 lc 'cyan' t 'Thomas Banded Time (f32)', \\\n");
        fprintf(gp_time, "     '' skip 1 u 1:(column(9) > 0 ? column(9) : min_plot_log_val) w p pt 2 ps 0.8 lc 'dark-turquoise' t 'Thomas Full Time (f32)', \\\n");
        fprintf(gp_time, "     '' skip 1 u 1:(column(12) > 0 ? column(12) : min_plot_log_val) w p pt 5 ps 0.8 lc 'red' t 'Gauss Time (f64)', \\\n");
        fprintf(gp_time, "     '' skip 1 u 1:(column(15) > 0 ? column(15) : min_plot_log_val) w p pt 4 ps 0.8 lc 'magenta' t 'Thomas Banded Time (f64)', \\\n");
        fprintf(gp_time, "     '' skip 1 u 1:(column(18) > 0 ? column(18) : min_plot_log_val) w p pt 1 ps 0.8 lc 'purple' t 'Thomas Full Time (f64)'\n\n");
        fclose(gp_time);
        printf("Generated Gnuplot script: %s\n", script_path_time);
    }

    // --- Memory Plot Script (Log Scale) ---
    FILE *gp_mem = fopen(script_path_memory, "w");
    if (!gp_mem) {
        fprintf(stderr, "ERROR: Creating Gnuplot script %s.\n", script_path_memory);
    } else {
        generate_gnuplot_common_settings(gp_mem, csv_filepath, max_n_for_plots);
        fprintf(gp_mem, "min_plot_log_val = 0.001 \n\n");

        fprintf(gp_mem, "set output '%s'\n", plot_file_memory);
        fprintf(gp_mem, "set title 'Estimated Peak Memory Usage vs Size (Tridiagonal, m=%.1f, k=%.1f)'\n", M_PARAM, K_PARAM);
        fprintf(gp_mem, "set ylabel 'Memory (KB, log scale)'\n");
        fprintf(gp_mem, "set xlabel 'Matrix Size (N)'\n");
        fprintf(gp_mem, "set logscale y\n");
        fprintf(gp_mem, "set format y '10^{%%L}'\n");
        fprintf(gp_mem, "set yrange [0.1 : 50000]\n"); // Adjusted yrange, N_MAX=500, f64 Gauss N*N is ~2MB, other N*N copy is ~2MB, so ~4MB = 4000KB. 50000KB gives margin.

        fprintf(gp_mem, "plot datafile skip 1 u 1:(column(4) > 0 ? column(4) : min_plot_log_val) w p pt 7 ps 0.8 lc 'blue' t 'Gauss Mem (f32)', \\\n");
        fprintf(gp_mem, "     '' skip 1 u 1:(column(7) > 0 ? column(7) : min_plot_log_val) w p pt 6 ps 0.8 lc 'cyan' t 'Thomas Banded Mem (f32)', \\\n");
        fprintf(gp_mem, "     '' skip 1 u 1:(column(10) > 0 ? column(10) : min_plot_log_val) w p pt 2 ps 0.8 lc 'dark-turquoise' t 'Thomas Full Mem (f32)', \\\n");
        fprintf(gp_mem, "     '' skip 1 u 1:(column(13) > 0 ? column(13) : min_plot_log_val) w p pt 5 ps 0.8 lc 'red' t 'Gauss Mem (f64)', \\\n");
        fprintf(gp_mem, "     '' skip 1 u 1:(column(16) > 0 ? column(16) : min_plot_log_val) w p pt 4 ps 0.8 lc 'magenta' t 'Thomas Banded Mem (f64)', \\\n");
        fprintf(gp_mem, "     '' skip 1 u 1:(column(19) > 0 ? column(19) : min_plot_log_val) w p pt 1 ps 0.8 lc 'purple' t 'Thomas Full Mem (f64)'\n\n");
        fclose(gp_mem);
        printf("Generated Gnuplot script: %s\n", script_path_memory);
    }
}

void generate_latex_table(
    const char* latex_filename_base,
    const ExperimentResult* results_array,
    int num_n_values,
    bool use_longtable) {

    char filepath[256];
    ensure_dir_exists("latex_out");
    // One table for float32, one for float64 for brevity on page
    sprintf(filepath, "latex_out/%s_f32.txt", latex_filename_base);
    FILE* f_f32 = fopen(filepath, "w");
    sprintf(filepath, "latex_out/%s_f64.txt", latex_filename_base);
    FILE* f_f64 = fopen(filepath, "w");

    if (!f_f32 || !f_f64) {
        fprintf(stderr, "ERROR: Opening LaTeX file(s) for writing.\n");
        if(f_f32) fclose(f_f32);
        if(f_f64) fclose(f_f64);
        return;
    }

    const char* table_env_start = use_longtable ? "\\begin{longtable}" : "\\begin{tabular}";
    const char* table_env_end = use_longtable ? "\\end{longtable}" : "\\end{tabular}";
    const char* caption_label_format_f32 = use_longtable ?
        "\\caption{Wyniki dla macierzy trójdiagonalnej ($m=%.1f, k=%.1f$), precyzja float32 \\label{tab:%s_f32}} \\\\" :
        "\\caption{Wybrane wyniki dla macierzy trójdiagonalnej ($m=%.1f, k=%.1f$), precyzja float32 \\label{tab:%s_f32}}";
    const char* caption_label_format_f64 = use_longtable ?
        "\\caption{Wyniki dla macierzy trójdiagonalnej ($m=%.1f, k=%.1f$), precyzja float64 \\label{tab:%s_f64}} \\\\" :
        "\\caption{Wybrane wyniki dla macierzy trójdiagonalnej ($m=%.1f, k=%.1f$), precyzja float64 \\label{tab:%s_f64}}";

    char caption_buffer[250];

    // --- Float32 Table ---
    fprintf(f_f32, "%% LaTeX table for Tridiagonal Matrix (m=%.1f, k=%.1f), float32\n", M_PARAM, K_PARAM);
    if (!use_longtable) { fprintf(f_f32, "\\begin{table}[htbp]\n\\centering\n\\resizebox{\\textwidth}{!}{\n");}
    fprintf(f_f32, "%s{c|ccc|ccc|ccc}\n", table_env_start);

    sprintf(caption_buffer, caption_label_format_f32, M_PARAM, K_PARAM, latex_filename_base);
    fprintf(f_f32, "%s\n", caption_buffer);
    fprintf(f_f32, "\\toprule\n");
    fprintf(f_f32, " & \\multicolumn{3}{c|}{Gauss (Full A)} & \\multicolumn{3}{c|}{Thomas (Banded A)} & \\multicolumn{3}{c}{Thomas (Full A)} \\\\\n");
    fprintf(f_f32, "\\cmidrule(lr){2-4} \\cmidrule(lr){5-7} \\cmidrule(lr){8-10}\n");
    fprintf(f_f32, "N & Błąd & Czas [s] & Pam. [KB] & Błąd & Czas [s] & Pam. [KB] & Błąd & Czas [s] & Pam. [KB] \\\\\n");
    fprintf(f_f32, "\\midrule\n");

    if (use_longtable) {
        fprintf(f_f32, "\\endfirsthead\n");
        fprintf(f_f32, "\\caption[]{-- ciąg dalszy (float32).} \\\\\n\\toprule\n");
        fprintf(f_f32, " & \\multicolumn{3}{c|}{Gauss (Full A)} & \\multicolumn{3}{c|}{Thomas (Banded A)} & \\multicolumn{3}{c}{Thomas (Full A)} \\\\\n");
        fprintf(f_f32, "\\cmidrule(lr){2-4} \\cmidrule(lr){5-7} \\cmidrule(lr){8-10}\n");
        fprintf(f_f32, "N & Błąd & Czas [s] & Pam. [KB] & Błąd & Czas [s] & Pam. [KB] & Błąd & Czas [s] & Pam. [KB] \\\\\n");
        fprintf(f_f32, "\\midrule\n\\endhead\n");
        fprintf(f_f32, "\\midrule\n\\multicolumn{10}{r}{\\textit{ciąg dalszy na następnej stronie}} \\\\\n\\endfoot\n");
        fprintf(f_f32, "\\bottomrule\n\\endlastfoot\n");
    }

    // --- Float64 Table ---
    fprintf(f_f64, "%% LaTeX table for Tridiagonal Matrix (m=%.1f, k=%.1f), float64\n", M_PARAM, K_PARAM);
     if (!use_longtable) { fprintf(f_f64, "\\begin{table}[htbp]\n\\centering\n\\resizebox{\\textwidth}{!}{\n");}
    fprintf(f_f64, "%s{c|ccc|ccc|ccc}\n", table_env_start);

    sprintf(caption_buffer, caption_label_format_f64, M_PARAM, K_PARAM, latex_filename_base);
    fprintf(f_f64, "%s\n", caption_buffer);
    fprintf(f_f64, "\\toprule\n");
    fprintf(f_f64, " & \\multicolumn{3}{c|}{Gauss (Full A)} & \\multicolumn{3}{c|}{Thomas (Banded A)} & \\multicolumn{3}{c}{Thomas (Full A)} \\\\\n");
    fprintf(f_f64, "\\cmidrule(lr){2-4} \\cmidrule(lr){5-7} \\cmidrule(lr){8-10}\n");
    fprintf(f_f64, "N & Błąd & Czas [s] & Pam. [KB] & Błąd & Czas [s] & Pam. [KB] & Błąd & Czas [s] & Pam. [KB] \\\\\n");
    fprintf(f_f64, "\\midrule\n");
    if (use_longtable) {
        fprintf(f_f64, "\\endfirsthead\n");
        fprintf(f_f64, "\\caption[]{-- ciąg dalszy (float64).} \\\\\n\\toprule\n");
        fprintf(f_f64, " & \\multicolumn{3}{c|}{Gauss (Full A)} & \\multicolumn{3}{c|}{Thomas (Banded A)} & \\multicolumn{3}{c}{Thomas (Full A)} \\\\\n");
        fprintf(f_f64, "\\cmidrule(lr){2-4} \\cmidrule(lr){5-7} \\cmidrule(lr){8-10}\n");
        fprintf(f_f64, "N & Błąd & Czas [s] & Pam. [KB] & Błąd & Czas [s] & Pam. [KB] & Błąd & Czas [s] & Pam. [KB] \\\\\n");
        fprintf(f_f64, "\\midrule\n\\endhead\n");
        fprintf(f_f64, "\\midrule\n\\multicolumn{10}{r}{\\textit{ciąg dalszy na następnej stronie}} \\\\\n\\endfoot\n");
        fprintf(f_f64, "\\bottomrule\n\\endlastfoot\n");
    }

    int print_Ns_short[] = {2, 10, 50, 100, 200, 300, 400, 500, -1}; // For non-longtable
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
            // Float32 data
            fprintf(f_f32, "%d & ", res->size_n);
            fprint_latex_sci(f_f32, res->err_gauss_f32); fprintf(f_f32, " & ");
            fprint_latex_fixed(f_f32, res->time_gauss_f32_sec, 4); fprintf(f_f32, " & ");
            fprint_latex_fixed(f_f32, res->mem_gauss_f32_kb, 2); fprintf(f_f32, " & ");

            fprint_latex_sci(f_f32, res->err_thomas_banded_f32); fprintf(f_f32, " & ");
            fprint_latex_fixed(f_f32, res->time_thomas_banded_f32_sec, 4); fprintf(f_f32, " & ");
            fprint_latex_fixed(f_f32, res->mem_thomas_banded_f32_kb, 2); fprintf(f_f32, " & ");

            fprint_latex_sci(f_f32, res->err_thomas_full_f32); fprintf(f_f32, " & ");
            fprint_latex_fixed(f_f32, res->time_thomas_full_f32_sec, 4); fprintf(f_f32, " & ");
            fprint_latex_fixed(f_f32, res->mem_thomas_full_f32_kb, 2);
            fprintf(f_f32, " \\\\\n");

            // Float64 data
            fprintf(f_f64, "%d & ", res->size_n);
            fprint_latex_sci(f_f64, res->err_gauss_f64); fprintf(f_f64, " & ");
            fprint_latex_fixed(f_f64, res->time_gauss_f64_sec, 4); fprintf(f_f64, " & ");
            fprint_latex_fixed(f_f64, res->mem_gauss_f64_kb, 2); fprintf(f_f64, " & ");

            fprint_latex_sci(f_f64, res->err_thomas_banded_f64); fprintf(f_f64, " & ");
            fprint_latex_fixed(f_f64, res->time_thomas_banded_f64_sec, 4); fprintf(f_f64, " & ");
            fprint_latex_fixed(f_f64, res->mem_thomas_banded_f64_kb, 2); fprintf(f_f64, " & ");

            fprint_latex_sci(f_f64, res->err_thomas_full_f64); fprintf(f_f64, " & ");
            fprint_latex_fixed(f_f64, res->time_thomas_full_f64_sec, 4); fprintf(f_f64, " & ");
            fprint_latex_fixed(f_f64, res->mem_thomas_full_f64_kb, 2);
            fprintf(f_f64, " \\\\\n");
        }
    }

    if (!use_longtable) {
        fprintf(f_f32, "\\bottomrule\n%s\n}\n\\end{table}\n", table_env_end);
        fprintf(f_f64, "\\bottomrule\n%s\n}\n\\end{table}\n", table_env_end);
    } else {
        // \end{longtable} is part of \endlastfoot for f_f32
        // and needs to be explicitly added if not included in endlastfoot
        // Actually, it IS part of \endlastfoot, so nothing needed here
    }

    fclose(f_f32);
    fclose(f_f64);
    printf("Generated LaTeX tables: latex_out/%s_f32.txt and latex_out/%s_f64.txt\n", latex_filename_base, latex_filename_base);
}