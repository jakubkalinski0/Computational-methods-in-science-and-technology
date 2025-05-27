#include "fileio.h"
#include <sys/stat.h> // For mkdir (POSIX)
#include <errno.h>    // For errno
#include <string.h>   // Potrzebne dla strcmp
#include <math.h>     // Potrzebne dla isinf, isnan

#ifdef _WIN32
#include <direct.h> // For _mkdir (Windows)
#endif

void ensure_directory_exists(const char* path) {
    int ret;
#ifdef _WIN32
    ret = _mkdir(path);
#else
    // 0775 provides rwx for owner/group, rx for others
    ret = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    if (ret == 0) {
        // printf("Katalog utworzony: %s\n", path); // Opcjonalnie: odkomentuj dla pełnego logowania
    } else if (errno == EEXIST) {
        // Katalog już istnieje, co jest w porządku.
    } else {
        fprintf(stderr, "Błąd tworzenia katalogu %s: %s\n", path, strerror(errno));
        // Zdecyduj, czy to błąd krytyczny dla aplikacji
    }
}


void save_results_to_csv(
    const char* base_filename, // np. "results_A_I"
    const ExperimentResult* results_float,
    const ExperimentResult* results_double,
    const int* sizes,
    int num_sizes) {

    char filepath[256];
    ensure_directory_exists("data");
    sprintf(filepath, "data/%s.csv", base_filename);

    FILE *f = fopen(filepath, "w");
    if (f == NULL) {
        fprintf(stderr, "Błąd otwierania pliku %s do zapisu.\n", filepath);
        return;
    }

    fprintf(f, "Size,MaxAbsError_f32,CondNum_f32,TimeSolve_f32,TimeCond_f32,MaxAbsError_f64,CondNum_f64,TimeSolve_f64,TimeCond_f64\n");

    for (int i = 0; i < num_sizes; i++) {
        fprintf(f, "%d,", sizes[i]);
        fprintf(f, "%.6e,%.6e,%.6f,%.6f,",
                results_float[i].max_abs_error, results_float[i].condition_number,
                results_float[i].time_solve_sec, results_float[i].time_cond_sec);
        fprintf(f, "%.6e,%.6e,%.6f,%.6f\n",
                results_double[i].max_abs_error, results_double[i].condition_number,
                results_double[i].time_solve_sec, results_double[i].time_cond_sec);
    }
    fclose(f);
    printf("Wyniki zapisano do %s\n", filepath);
}


// --- Funkcja generująca skrypt Gnuplot dla Macierzy A_I ---
void generate_gnuplot_script_A_I(
    const char* csv_filename, // np. "data/results_A_I.csv"
    const char* plot_dir,     // np. "plots"
    const char* script_dir) { // np. "scripts"

    char script_filepath[256];
    char plot_base_filepath[256];
    const char* matrix_name = "A_I";

    ensure_directory_exists(plot_dir);
    ensure_directory_exists(script_dir);

    sprintf(script_filepath, "%s/plot_%s.gp", script_dir, matrix_name);
    sprintf(plot_base_filepath, "%s/%s", plot_dir, matrix_name);

    FILE *gp = fopen(script_filepath, "w");
    if (gp == NULL) {
        fprintf(stderr, "Błąd tworzenia skryptu Gnuplot %s.\n", script_filepath);
        return;
    }

    fprintf(gp, "# Plik: %s\n", script_filepath);
    fprintf(gp, "# Skrypt Gnuplot dla wyników macierzy %s\n\n", matrix_name);

    fprintf(gp, "set terminal pngcairo enhanced size 1024,768 font 'Arial,10'\n");
    fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set key top left spacing 1.2\n"); // Legenda w prawym górnym rogu dla A_I
    fprintf(gp, "set grid\n\n");

    fprintf(gp, "# Wartość zastępująca zero na wykresie logarytmicznym błędów\n");
    const char* error_zero_replacement_val_str = "1e-18"; // Dla A_I błędy mogą być bardzo różne
    fprintf(gp, "error_zero_replacement = %s\n", error_zero_replacement_val_str);
    fprintf(gp, "replace_error_zero(col_val) = (col_val == 0.0 ? error_zero_replacement : col_val)\n\n");

    const char* plot_style_f32 = "with linespoints pt 7 lc rgb 'blue'";
    const char* plot_style_f64 = "with linespoints pt 6 lc rgb 'red'";

    fprintf(gp, "# --- Wykres maksymalnego błędu absolutnego ---\n");
    fprintf(gp, "set output '%s_error.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Max Absolute Error vs Size (Matrix %s)'\n", matrix_name);
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set xrange [1:21]\n");
    fprintf(gp, "set ylabel 'Max Absolute Error (log scale)'\n");
    fprintf(gp, "set logscale y\n");
    fprintf(gp, "set format y '10^{%%L}'\n");
    fprintf(gp, "set yrange [error_zero_replacement/10 : *]\n\n");
    fprintf(gp, "plot '%s' using 1:(replace_error_zero(column(2))) %s title 'Error (float32)', \\\n", csv_filename, plot_style_f32);
    fprintf(gp, "     '' using 1:(replace_error_zero(column(6))) %s title 'Error (float64)'\n\n\n", plot_style_f64);

    fprintf(gp, "# --- Wykres współczynnika uwarunkowania ---\n");
    fprintf(gp, "set output '%s_cond.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Condition Number vs Size (Matrix %s)'\n", matrix_name);
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set xrange [1:21]\n");
    fprintf(gp, "set ylabel 'Condition Number (log scale)'\n");
    fprintf(gp, "set logscale y\n"); // Upewnij się, że jest log dla tego wykresu
    fprintf(gp, "set format y '10^{%%L}'\n"); // Upewnij się, że format jest log dla tego wykresu
    fprintf(gp, "set yrange [*:*]\n\n");
    fprintf(gp, "plot '%s' using 1:3 %s title 'Cond Num (float32)', \\\n", csv_filename, plot_style_f32);
    fprintf(gp, "     '' u 1:7 %s title 'Cond Num (float64)'\n\n\n", plot_style_f64);

    fprintf(gp, "# --- Wykres czasu rozwiązywania ---\n");
    fprintf(gp, "set output '%s_time_solve.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Solve Time vs Size (Matrix %s)'\n", matrix_name);
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set xrange [1:21]\n");
    fprintf(gp, "set nologscale y\n");
    fprintf(gp, "set format y '%%.6f'\n"); // Format dla małych czasów A_I
    fprintf(gp, "set ylabel 'Time (seconds)'\n");
    fprintf(gp, "set yrange [0:*]\n");
    fprintf(gp, "set ytics auto\n\n"); // Specyficzne ytics dla A_I
    fprintf(gp, "plot '%s' using 1:4 %s title 'Time Solve (float32)', \\\n", csv_filename, plot_style_f32);
    fprintf(gp, "     '' u 1:8 %s title 'Time Solve (float64)'\n\n\n", plot_style_f64);

    fprintf(gp, "# --- Wykres czasu obliczania współczynnika uwarunkowania ---\n");
    fprintf(gp, "set output '%s_time_cond.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Condition Number Calc Time vs Size (Matrix %s)'\n", matrix_name);
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set xrange [1:21]\n");
    fprintf(gp, "set nologscale y\n"); // Upewnij się
    fprintf(gp, "set format y '%%.6f'\n"); // Upewnij się
    fprintf(gp, "set ylabel 'Time (seconds)'\n"); // Upewnij się
    fprintf(gp, "set yrange [0:*]\n"); // Upewnij się
    fprintf(gp, "set ytics auto\n\n"); // Specyficzne ytics dla A_I
    fprintf(gp, "plot '%s' using 1:5 %s title 'Time Cond (float32)', \\\n", csv_filename, plot_style_f32);
    fprintf(gp, "     '' u 1:9 %s title 'Time Cond (float64)'\n\n\n", plot_style_f64);

    fprintf(gp, "print \"Generated plots for %s.\"\n", matrix_name);
    fclose(gp);
    printf("Wygenerowano skrypt Gnuplot: %s\n", script_filepath);
}

// --- Funkcja generująca skrypt Gnuplot dla Macierzy A_II ---
void generate_gnuplot_script_A_II(
    const char* csv_filename, // np. "data/results_A_II.csv"
    const char* plot_dir,     // np. "plots"
    const char* script_dir) { // np. "scripts"

    char script_filepath[256];
    char plot_base_filepath[256];
    const char* matrix_name = "A_II";

    ensure_directory_exists(plot_dir);
    ensure_directory_exists(script_dir);

    sprintf(script_filepath, "%s/plot_%s.gp", script_dir, matrix_name);
    sprintf(plot_base_filepath, "%s/%s", plot_dir, matrix_name);

    FILE *gp = fopen(script_filepath, "w");
    if (gp == NULL) {
        fprintf(stderr, "Błąd tworzenia skryptu Gnuplot %s.\n", script_filepath);
        return;
    }

    fprintf(gp, "# Plik: %s\n", script_filepath);
    fprintf(gp, "# Skrypt Gnuplot dla wyników macierzy %s\n\n", matrix_name);

    fprintf(gp, "set terminal pngcairo enhanced size 1024,768 font 'Arial,10'\n");
    fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set key top left spacing 1.2\n"); // Legenda w lewym górnym rogu dla A_II (lub inna preferencja)
    fprintf(gp, "set grid\n\n");

    fprintf(gp, "# Wartość zastępująca zero na wykresie logarytmicznym błędów\n");
    const char* error_zero_replacement_val_str = "1e-18"; // Dla A_II błędy są zwykle małe
    fprintf(gp, "error_zero_replacement = %s\n", error_zero_replacement_val_str);
    fprintf(gp, "replace_error_zero(col_val) = (col_val == 0.0 ? error_zero_replacement : col_val)\n\n");

    const char* plot_style_f32 = "with lines lc rgb 'blue'"; // Dla A_II bez punktów
    const char* plot_style_f64 = "with lines lc rgb 'red'";  // Dla A_II bez punktów

    fprintf(gp, "# --- Wykres maksymalnego błędu absolutnego ---\n");
    fprintf(gp, "set output '%s_error.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Max Absolute Error vs Size (Matrix %s)'\n", matrix_name);
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set xrange [1:201]\n");
    fprintf(gp, "set ylabel 'Max Absolute Error (log scale)'\n");
    fprintf(gp, "set logscale y\n");
    fprintf(gp, "set format y '10^{%%L}'\n");
    fprintf(gp, "set yrange [error_zero_replacement/10 : *]\n\n");
    fprintf(gp, "plot '%s' using 1:(replace_error_zero(column(2))) %s title 'Error (float32)', \\\n", csv_filename, plot_style_f32);
    fprintf(gp, "     '' using 1:(replace_error_zero(column(6))) %s title 'Error (float64)'\n\n\n", plot_style_f64);

    fprintf(gp, "# --- Wykres współczynnika uwarunkowania ---\n");
    fprintf(gp, "set output '%s_cond.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Condition Number vs Size (Matrix %s)'\n", matrix_name);
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set xrange [1:201]\n");
    fprintf(gp, "set ylabel 'Condition Number (log scale)'\n");
    fprintf(gp, "set logscale y\n");
    fprintf(gp, "set format y '10^{%%L}'\n");
    fprintf(gp, "set yrange [*:*]\n\n");
    fprintf(gp, "plot '%s' using 1:3 %s title 'Cond Num (float32)', \\\n", csv_filename, plot_style_f32);
    fprintf(gp, "     '' u 1:7 %s title 'Cond Num (float64)'\n\n\n", plot_style_f64);

    fprintf(gp, "# --- Wykres czasu rozwiązywania ---\n");
    fprintf(gp, "set output '%s_time_solve.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Solve Time vs Size (Matrix %s)'\n", matrix_name);
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set xrange [1:201]\n");
    fprintf(gp, "set nologscale y\n");
    fprintf(gp, "set format y '%%.3f'\n"); // Format dla czasów A_II (mogą być większe)
    fprintf(gp, "set ylabel 'Time (seconds)'\n");
    fprintf(gp, "set yrange [-0.001:*]\n");
    fprintf(gp, "set ytics auto\n\n"); // Automatyczne ytics dla A_II
    fprintf(gp, "plot '%s' using 1:4 %s title 'Time Solve (float32)', \\\n", csv_filename, plot_style_f32);
    fprintf(gp, "     '' u 1:8 %s title 'Time Solve (float64)'\n\n\n", plot_style_f64);

    fprintf(gp, "# --- Wykres czasu obliczania współczynnika uwarunkowania ---\n");
    fprintf(gp, "set output '%s_time_cond.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Condition Number Calc Time vs Size (Matrix %s)'\n", matrix_name);
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set xrange [1:201]\n");
    fprintf(gp, "set nologscale y\n");
    fprintf(gp, "set format y '%%.3f'\n");
    fprintf(gp, "set ylabel 'Time (seconds)'\n");
    fprintf(gp, "set yrange [-0.002:*]\n");
    fprintf(gp, "set ytics auto\n\n"); // Automatyczne ytics dla A_II
    fprintf(gp, "plot '%s' using 1:5 %s title 'Time Cond (float32)', \\\n", csv_filename, plot_style_f32);
    fprintf(gp, "     '' u 1:9 %s title 'Time Cond (float64)'\n\n\n", plot_style_f64);

    fprintf(gp, "print \"Generated plots for %s.\"\n", matrix_name);
    fclose(gp);
    printf("Wygenerowano skrypt Gnuplot: %s\n", script_filepath);
}

void generate_gnuplot_script_comparison(
    const char* csv_A_I_filename,  // "data/results_A_I.csv"
    const char* csv_A_II_filename, // "data/results_A_II.csv"
    const char* plot_dir,
    const char* script_dir,
    int max_n_for_comparison) {

    char script_filepath[256];
    ensure_directory_exists(plot_dir);
    ensure_directory_exists(script_dir);
    sprintf(script_filepath, "%s/plot_cond_comparison.gp", script_dir);

    FILE *gp = fopen(script_filepath, "w");
    if (gp == NULL) {
        fprintf(stderr, "Błąd tworzenia skryptu Gnuplot %s.\n", script_filepath);
        return;
    }

    fprintf(gp, "set terminal pngcairo enhanced size 1024,768 font 'Arial,10'\n");
    fprintf(gp, "set output '%s/cond_comparison.png'\n", plot_dir);
    fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set title 'Condition Number Comparison (float64, N <= %d)'\n", max_n_for_comparison);
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set ylabel 'Condition Number (log scale)'\n");
    fprintf(gp, "set logscale y\n");
    fprintf(gp, "set format y '10^{%%L}'\n");
    fprintf(gp, "set key top left spacing 1.2\n");
    fprintf(gp, "set grid\n");
    fprintf(gp, "set xrange [1:%d]\n", max_n_for_comparison+1);

    int last_row_index_for_comparison = max_n_for_comparison - 1;

    fprintf(gp, "plot '%s' using 1:7 every ::0::%d with linespoints pt 7 lc rgb 'orange' title 'Cond Num A_I (float64)', \\\n",
            csv_A_I_filename, last_row_index_for_comparison );
    fprintf(gp, "     '%s' using 1:7 every ::0::%d with linespoints pt 6 lc rgb 'green' title 'Cond Num A_II (float64)'\n",
            csv_A_II_filename, last_row_index_for_comparison );

    fclose(gp);
    printf("Wygenerowano skrypt Gnuplot porównawczy: %s\n", script_filepath);
}

static void fprint_latex_sci(FILE* f, double val) {
    if (isinf(val)) fprintf(f, "\\infty");
    else if (isnan(val)) fprintf(f, "\\text{NaN}");
    else fprintf(f, "%.2e", val);
}
static void fprint_latex_fixed(FILE* f, double val) {
     if (isinf(val)) fprintf(f, "\\infty");
    else if (isnan(val)) fprintf(f, "\\text{NaN}");
    else fprintf(f, "%.4f", val);
}


void generate_latex_table_individual(
    const char* base_filename_tex,
    const ExperimentResult* results_float,
    const ExperimentResult* results_double,
    const int* sizes,
    int num_sizes,
    const char* matrix_caption_name,
    bool use_longtable) {

    char filepath[256];
    ensure_directory_exists("latex_out");
    sprintf(filepath, "latex_out/%s.tex", base_filename_tex);

    FILE* f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "Błąd otwierania pliku LaTeX %s do zapisu.\n", filepath);
        return;
    }

    fprintf(f, "%% Wygenerowana tabela LaTeX\n");
    if (use_longtable) {
        fprintf(f, "\\begin{longtable}{ccccccccc}\n");
        fprintf(f, "\\caption{Wyniki dla macierzy %s \\label{tab:%s}} \\\\\n", matrix_caption_name, base_filename_tex);
        fprintf(f, "\\toprule\n");
        fprintf(f, "N & Błąd ($\\epsilon_{32}$) & Błąd ($\\epsilon_{64}$) & Wskaźnik ($\\kappa_{32}$) & Wskaźnik ($\\kappa_{64}$) & Czas rozkł. ($t_{s32}$) & Czas rozkł. ($t_{s64}$) & Czas wsk. ($t_{\\kappa32}$) & Czas wsk. ($t_{\\kappa64}$) \\\\\n");
        fprintf(f, "\\midrule\n");
        fprintf(f, "\\endfirsthead\n");
        fprintf(f, "\\caption[]{-- ciąg dalszy.} \\\\\n");
        fprintf(f, "\\toprule\n");
        fprintf(f, "N & Błąd ($\\epsilon_{32}$) & Błąd ($\\epsilon_{64}$) & Wskaźnik ($\\kappa_{32}$) & Wskaźnik ($\\kappa_{64}$) & Czas rozkł. ($t_{s32}$) & Czas rozkł. ($t_{s64}$) & Czas wsk. ($t_{\\kappa32}$) & Czas wsk. ($t_{\\kappa64}$) \\\\\n");
        fprintf(f, "\\midrule\n");
        fprintf(f, "\\endhead\n");
        fprintf(f, "\\midrule\n");
        fprintf(f, "\\multicolumn{9}{r}{\\textit{ciąg dalszy na następnej stronie}} \\\\\n");
        fprintf(f, "\\endfoot\n");
        fprintf(f, "\\bottomrule\n");
        fprintf(f, "\\endlastfoot\n");
    } else {
        fprintf(f, "\\begin{table}[htbp]\n");
        fprintf(f, "\\centering\n");
        fprintf(f, "\\caption{Wyniki dla macierzy %s \\label{tab:%s}}\n", matrix_caption_name, base_filename_tex);
        fprintf(f, "\\begin{tabular}{ccccccccc}\n");
        fprintf(f, "\\toprule\n");
        fprintf(f, "N & Błąd ($\\epsilon_{32}$) & Błąd ($\\epsilon_{64}$) & Wskaźnik ($\\kappa_{32}$) & Wskaźnik ($\\kappa_{64}$) & Czas rozkł. ($t_{s32}$) & Czas rozkł. ($t_{s64}$) & Czas wsk. ($t_{\\kappa32}$) & Czas wsk. ($t_{\\kappa64}$) \\\\\n");
        fprintf(f, "\\midrule\n");
    }

    for (int i = 0; i < num_sizes; i++) {
        fprintf(f, "%d & ", sizes[i]);
        fprint_latex_sci(f, results_float[i].max_abs_error); fprintf(f, " & ");
        fprint_latex_sci(f, results_double[i].max_abs_error); fprintf(f, " & ");
        fprint_latex_sci(f, results_float[i].condition_number); fprintf(f, " & ");
        fprint_latex_sci(f, results_double[i].condition_number); fprintf(f, " & ");
        fprint_latex_fixed(f, results_float[i].time_solve_sec); fprintf(f, " & ");
        fprint_latex_fixed(f, results_double[i].time_solve_sec); fprintf(f, " & ");
        fprint_latex_fixed(f, results_float[i].time_cond_sec); fprintf(f, " & ");
        fprint_latex_fixed(f, results_double[i].time_cond_sec);
        fprintf(f, " \\\\\n");
    }

    if (use_longtable) {
        fprintf(f, "\\end{longtable}\n");
    } else {
        fprintf(f, "\\bottomrule\n");
        fprintf(f, "\\end{tabular}\n");
        fprintf(f, "\\end{table}\n");
    }
    fclose(f);
    printf("Wygenerowano tabelę LaTeX: %s\n", filepath);
}


void generate_latex_table_comparison(
    const char* base_filename_tex,
    const ExperimentResult* results_A_I_double,
    const ExperimentResult* results_A_II_double,
    const int* sizes_A_I, int num_sizes_A_I,
    const int* sizes_A_II, int num_sizes_A_II,
    int max_n_for_comparison) {

    char filepath[256];
    ensure_directory_exists("latex_out");
    sprintf(filepath, "latex_out/%s.tex", base_filename_tex);

    FILE* f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "Błąd otwierania pliku LaTeX %s do zapisu.\n", filepath);
        return;
    }

    fprintf(f, "%% Wygenerowana tabela LaTeX porównawcza\n");
    fprintf(f, "\\begin{table}[htbp]\n");
    fprintf(f, "\\centering\n");
    fprintf(f, "\\caption{Porównanie współczynników uwarunkowania $\\kappa(A)$ (float64) dla $N \\le %d$ \\label{tab:%s}}\n", max_n_for_comparison, base_filename_tex);
    fprintf(f, "\\begin{tabular}{ccc}\n");
    fprintf(f, "\\toprule\n");
    fprintf(f, "N & $\\kappa(A_I)$ (float64) & $\\kappa(A_{II})$ (float64) \\\\\n");
    fprintf(f, "\\midrule\n");

    for (int i = 0; i < num_sizes_A_I; i++) {
        int current_size_A_I = sizes_A_I[i];
        if (current_size_A_I > max_n_for_comparison) break;

        fprintf(f, "%d & ", current_size_A_I);
        fprint_latex_sci(f, results_A_I_double[i].condition_number);
        fprintf(f, " & ");

        bool found_A_II = false;
        for (int j = 0; j < num_sizes_A_II; j++) {
            if (sizes_A_II[j] == current_size_A_I) {
                fprint_latex_sci(f, results_A_II_double[j].condition_number);
                found_A_II = true;
                break;
            }
        }
        if (!found_A_II) {
            fprintf(f, "---");
        }
        fprintf(f, " \\\\\n");
    }

    fprintf(f, "\\bottomrule\n");
    fprintf(f, "\\end{tabular}\n");
    fprintf(f, "\\end{table}\n");

    fclose(f);
    printf("Wygenerowano tabelę LaTeX porównawczą: %s\n", filepath);
}