#ifndef FILE_IO_H
#define FILE_IO_H

#include "common.h"

// For directory creation
void ensure_directory_exists(const char* path);

// CSV for detailed experiment results
void open_csv_file(FILE** fp, const char* filename_base, const char* sub_dir);
void append_to_csv_file(FILE* fp, const ExperimentData* data);
void close_csv_file(FILE* fp);

// CSV for spectral radius results
void save_spectral_radius_to_csv(const char* filename_base, const int* n_values, const double* sr_values, int num_n_values);

// Gnuplot script generation
void generate_gnuplot_scripts_for_n(
    const char* csv_filename_base,
    int n_val,
    const InitialVectorType* x0_types, int num_x0_types,
    const StopCriterionType* criterion_types_arr, int num_criterion_types // Zmieniona nazwa, aby było jasne
);
void generate_gnuplot_script_spectral_radius(const char* csv_filename_base);

// LaTeX table generation
void generate_latex_table_selected_results(
    const char* base_filename_tex,
    const ExperimentData* all_results,
    int total_results_count,
    const char* table_caption,
    int target_n,
    const double* target_rhos, int num_target_rhos
);

void generate_latex_table_spectral_radius(const char* base_filename_tex,
                                          const int* n_values,
                                          const double* sr_values,
                                          int num_n_values);

// Helpers (can be static in .c file if not needed elsewhere)
const char* get_x0_type_string(InitialVectorType type);
const char* get_criterion_type_string(StopCriterionType type);


#endif // FILE_IO_H