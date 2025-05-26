#include "fileio.h" // Changed from output_gen.h
#include <sys/stat.h> // For mkdir (POSIX)
#include <errno.h>    // For errno

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
        // printf("Directory created: %s\n", path); // Optional: uncomment for verbose output
    } else if (errno == EEXIST) {
        // Directory already exists, which is fine.
    } else {
        fprintf(stderr, "Error creating directory %s: %s\n", path, strerror(errno));
        // Decide if this is a fatal error for your application
    }
}


void save_results_to_csv(
    const char* base_filename, // e.g., "results_A_I"
    const ExperimentResult* results_float,
    const ExperimentResult* results_double,
    const int* sizes,
    int num_sizes) {

    char filepath[256];
    ensure_directory_exists("data");
    sprintf(filepath, "data/%s.csv", base_filename);

    FILE *f = fopen(filepath, "w");
    if (f == NULL) {
        fprintf(stderr, "Error opening file %s for writing.\n", filepath);
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
    printf("Results saved to %s\n", filepath);
}


void generate_gnuplot_script_individual(
    const char* csv_filename, // Full path to CSV, e.g., "data/results_A_I.csv"
    const char* matrix_name,  // e.g., "A_I" or "A_II"
    const char* plot_dir,     // e.g., "plots"
    const char* script_dir,   // e.g., "scripts"
    bool is_A_II) {

    char script_filepath[256];
    char plot_base_filepath[256];

    ensure_directory_exists(plot_dir);
    ensure_directory_exists(script_dir);

    sprintf(script_filepath, "%s/plot_%s.gp", script_dir, matrix_name);
    // Plot files will be named like "plots/A_I_error.png", "plots/A_I_cond.png", etc.
    sprintf(plot_base_filepath, "%s/%s", plot_dir, matrix_name);


    FILE *gp = fopen(script_filepath, "w");
    if (gp == NULL) {
        fprintf(stderr, "Error creating Gnuplot script %s.\n", script_filepath);
        return;
    }

    fprintf(gp, "set terminal pngcairo enhanced size 1024,768 font 'Arial,10'\n");
    fprintf(gp, "set datafile separator ','\n");
    fprintf(gp, "set key top right spacing 1.2\n");
    fprintf(gp, "set grid\n\n");

    const char* marker_f32 = is_A_II ? "with lines lc rgb 'blue'" : "with linespoints pt 7 lc rgb 'blue'"; // circle
    const char* marker_f64 = is_A_II ? "with lines lc rgb 'red'" : "with linespoints pt 6 lc rgb 'red'";   // square


    // Max Absolute Error Plot
    fprintf(gp, "set output '%s_error.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Max Absolute Error vs Size (Matrix %s)'\n", matrix_name);
    fprintf(gp, "set xlabel 'Matrix Size (N)'\n");
    fprintf(gp, "set ylabel 'Max Absolute Error (log scale)'\n");
    fprintf(gp, "set logscale y\n");
    fprintf(gp, "set format y '10^{%%L}'\n");
    fprintf(gp, "plot '%s' using 1:2 %s title 'Error (float32)', \\\n", csv_filename, marker_f32);
    fprintf(gp, "     '' u 1:6 %s title 'Error (float64)'\n\n", marker_f64);

    // Condition Number Plot
    fprintf(gp, "set output '%s_cond.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Condition Number vs Size (Matrix %s)'\n", matrix_name);
    fprintf(gp, "set ylabel 'Condition Number (log scale)'\n");
    // logscale y and format y are already set
    fprintf(gp, "plot '%s' using 1:3 %s title 'Cond Num (float32)', \\\n", csv_filename, marker_f32);
    fprintf(gp, "     '' u 1:7 %s title 'Cond Num (float64)'\n\n", marker_f64);

    // Solve Time Plot
    fprintf(gp, "set output '%s_time_solve.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Solve Time vs Size (Matrix %s)'\n", matrix_name);
    fprintf(gp, "set nologscale y\n"); // Reset y-axis scale
    fprintf(gp, "set format y '%%.4f'\n");
    fprintf(gp, "set ylabel 'Time (seconds)'\n");
    fprintf(gp, "plot '%s' using 1:4 %s title 'Time Solve (float32)', \\\n", csv_filename, marker_f32);
    fprintf(gp, "     '' u 1:8 %s title 'Time Solve (float64)'\n\n", marker_f64);

    // Condition Number Calculation Time Plot
    fprintf(gp, "set output '%s_time_cond.png'\n", plot_base_filepath);
    fprintf(gp, "set title 'Condition Number Calc Time vs Size (Matrix %s)'\n", matrix_name);
    // nologscale y, format y, ylabel are already set
    fprintf(gp, "plot '%s' using 1:5 %s title 'Time Cond (float32)', \\\n", csv_filename, marker_f32);
    fprintf(gp, "     '' u 1:9 %s title 'Time Cond (float64)'\n\n", marker_f64);

    fclose(gp);
    printf("Generated Gnuplot script: %s\n", script_filepath);
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
        fprintf(stderr, "Error creating Gnuplot script %s.\n", script_filepath);
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
    fprintf(gp, "set xrange [1.8:%d.2]\n", max_n_for_comparison); // A bit of padding

    // Adjust 'every' to correctly select rows for max_n_for_comparison.
    // If sizes are 2,3,...,max_n_for_comparison, there are (max_n_for_comparison - 2 + 1) data rows.
    // Gnuplot's 'every' is 0-indexed for data blocks (after header).
    // ::1::N means skip 0 rows, plot N rows. We want up to the row corresponding to max_n_for_comparison.
    // Since sizes start from 2, index in data = size - 2.
    // So, for max_n_for_comparison, we want up to data row (max_n_for_comparison - 2).
    // 'every ::0::(max_n_for_comparison - 2)' or simply specifying xrange and not using every might be simpler.
    // Given xrange is set, 'every' might not be strictly necessary if CSV contains only up to MAX_N_I for A_I.
    // However, to be safe and handle larger CSVs if they existed:
    int last_row_index_for_A_I = max_n_for_comparison - 2; // Size 2 is row 0, size 20 is row 18

    fprintf(gp, "plot '%s' using 1:7 every ::0::%d with linespoints pt 7 lc rgb 'orange' title 'Cond Num A_I (float64)', \\\n",
            csv_A_I_filename, last_row_index_for_A_I );
    fprintf(gp, "     '%s' using 1:7 every ::0::%d with linespoints pt 6 lc rgb 'green' title 'Cond Num A_II (float64)'\n",
            csv_A_II_filename, last_row_index_for_A_I ); // A_II also plotted up to same max_n

    fclose(gp);
    printf("Generated Gnuplot comparison script: %s\n", script_filepath);
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
    const char* base_filename_tex, // e.g., "table_A_I"
    const ExperimentResult* results_float,
    const ExperimentResult* results_double,
    const int* sizes,
    int num_sizes,
    const char* matrix_caption_name, // e.g., "$A_I$"
    bool use_longtable) {

    char filepath[256];
    ensure_directory_exists("latex_out");
    sprintf(filepath, "latex_out/%s.tex", base_filename_tex);

    FILE* f = fopen(filepath, "w");
    if (!f) {
        fprintf(stderr, "Error opening LaTeX file %s for writing.\n", filepath);
        return;
    }

    fprintf(f, "%% Generated LaTeX table\n");
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
    printf("Generated LaTeX table: %s\n", filepath);
}


void generate_latex_table_comparison(
    const char* base_filename_tex, // e.g., "table_cond_compare"
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
        fprintf(stderr, "Error opening LaTeX file %s for writing.\n", filepath);
        return;
    }

    fprintf(f, "%% Generated LaTeX comparison table\n");
    fprintf(f, "\\begin{table}[htbp]\n");
    fprintf(f, "\\centering\n");
    fprintf(f, "\\caption{Porównanie współczynników uwarunkowania $\\kappa(A)$ (float64) dla $N \\le %d$ \\label{tab:%s}}\n", max_n_for_comparison, base_filename_tex);
    fprintf(f, "\\begin{tabular}{ccc}\n");
    fprintf(f, "\\toprule\n");
    fprintf(f, "N & $\\kappa(A_I)$ (float64) & $\\kappa(A_{II})$ (float64) \\\\\n");
    fprintf(f, "\\midrule\n");

    // Iterate through sizes relevant for A_I up to max_n_for_comparison
    for (int i = 0; i < num_sizes_A_I; i++) {
        int current_size_A_I = sizes_A_I[i];
        if (current_size_A_I > max_n_for_comparison) break;

        fprintf(f, "%d & ", current_size_A_I);
        fprint_latex_sci(f, results_A_I_double[i].condition_number);
        fprintf(f, " & ");

        // Find corresponding A_II result
        bool found_A_II = false;
        for (int j = 0; j < num_sizes_A_II; j++) {
            if (sizes_A_II[j] == current_size_A_I) {
                fprint_latex_sci(f, results_A_II_double[j].condition_number);
                found_A_II = true;
                break;
            }
        }
        if (!found_A_II) {
            fprintf(f, "---"); // Or some placeholder if A_II doesn't have this size
        }
        fprintf(f, " \\\\\n");
    }

    fprintf(f, "\\bottomrule\n");
    fprintf(f, "\\end{tabular}\n");
    fprintf(f, "\\end{table}\n");

    fclose(f);
    printf("Generated LaTeX comparison table: %s\n", filepath);
}