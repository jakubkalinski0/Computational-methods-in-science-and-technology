#ifndef FILEIO_H
#define FILEIO_H

#include "common.h"

// Ensure directory exists, create if not.
void ensure_directory_exists(const char* path);

// Save experiment results to a CSV file.
void save_results_to_csv(
    const char* base_filename,
    const ExperimentResult* results_float,
    const ExperimentResult* results_double,
    const int* sizes,
    int num_sizes
);

// --- Zmienione deklaracje funkcji generujących skrypty Gnuplota ---
// Generuje skrypt Gnuplot dla wyników macierzy A_I.
void generate_gnuplot_script_A_I(
    const char* csv_filename, // np. "data/results_A_I.csv"
    const char* plot_dir,     // np. "plots"
    const char* script_dir    // np. "scripts"
);

// Generuje skrypt Gnuplot dla wyników macierzy A_II.
void generate_gnuplot_script_A_II(
    const char* csv_filename, // np. "data/results_A_II.csv"
    const char* plot_dir,     // np. "plots"
    const char* script_dir    // np. "scripts"
);
// --- Koniec zmian w deklaracjach ---

// Generate Gnuplot script for comparing condition numbers of A_I and A_II.
void generate_gnuplot_script_comparison(
    const char* csv_A_I_filename,
    const char* csv_A_II_filename,
    const char* plot_dir,
    const char* script_dir,
    int max_n_for_comparison
);

// Generate LaTeX table for individual matrix type results.
void generate_latex_table_individual(
    const char* base_filename_tex,
    const ExperimentResult* results_float,
    const ExperimentResult* results_double,
    const int* sizes,
    int num_sizes,
    const char* matrix_caption_name,
    bool use_longtable
);

// Generate LaTeX table for comparing condition numbers.
void generate_latex_table_comparison(
    const char* base_filename_tex,
    const ExperimentResult* results_A_I_double,
    const ExperimentResult* results_A_II_double,
    const int* sizes_A_I, int num_sizes_A_I,
    const int* sizes_A_II, int num_sizes_A_II,
    int max_n_for_comparison
);


#endif // FILEIO_H