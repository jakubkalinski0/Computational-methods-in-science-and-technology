#ifndef FILEIO_H
#define FILEIO_H

#include "common.h"
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#endif

static inline void ensure_dir_exists(const char* path) {
    int ret;
#ifdef _WIN32
    ret = _mkdir(path);
#else
    ret = mkdir(path, 0775);
#endif
    if (ret != 0 && errno != EEXIST) {
        fprintf(stderr, "ERROR: Could not create directory %s: %s\n", path, strerror(errno));
    }
}

void save_results_to_csv(
    const char* csv_filename_base,
    const ExperimentResult* results_array,
    int num_n_values
);

// Updated function declaration
void generate_gnuplot_scripts(
    const char* csv_filepath,
    const char* gnuplot_script_filename_base,
    const char* plot_filename_base,
    int max_n_for_plots
);

void generate_latex_table(
    const char* latex_filename_base,
    const ExperimentResult* results_array,
    int num_n_values,
    bool use_longtable
);

#endif // FILEIO_H