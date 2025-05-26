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

// Generate Gnuplot script for individual matrix type results.
void generate_gnuplot_script_individual(
    const char* csv_filename,
    const char* matrix_name, // e.g., "A_I" or "A_II"
    const char* plot_dir,
    const char* script_dir,
    bool is_A_II // for adjusting plot marker styles for many points
);

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
    const char* base_filename_tex, // e.g., "table_A_I"
    const ExperimentResult* results_float,
    const ExperimentResult* results_double,
    const int* sizes,
    int num_sizes,
    const char* matrix_caption_name, // e.g., "$A_I$"
    bool use_longtable
);

// Generate LaTeX table for comparing condition numbers.
void generate_latex_table_comparison(
    const char* base_filename_tex, // e.g., "table_cond_compare"
    const ExperimentResult* results_A_I_double, // Only double precision for comparison table
    const ExperimentResult* results_A_II_double,
    const int* sizes_A_I, int num_sizes_A_I,
    const int* sizes_A_II, int num_sizes_A_II,
    int max_n_for_comparison
);


#endif // FILEIO_H