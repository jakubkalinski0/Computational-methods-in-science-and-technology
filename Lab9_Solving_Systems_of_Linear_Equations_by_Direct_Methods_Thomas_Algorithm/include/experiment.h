#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "common.h"

void run_all_experiments(
    const int* n_sizes_arr,
    int num_n_values,
    ExperimentResult* results_array
);

#endif // EXPERIMENT_H