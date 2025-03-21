#include <stdio.h>
#include "visualization.h"
#include "interpolation.h"

// Function to save data for gnuplot visualization
void saveDataForPlot(InterpolationData *data, double *dd) {
    FILE *file = fopen("interpolation_data.txt", "w");
    if (file == NULL) {
        printf("Cannot open file for writing.\n");
        return;
    }
    
    // Save interpolation nodes
    fprintf(file, "# Interpolation nodes\n");
    for (int i = 0; i < data->n; i++) {
        fprintf(file, "%lf %lf\n", data->x[i], data->y[i]);
    }
    fprintf(file, "\n\n");
    
    // Save points for interpolation function plot
    fprintf(file, "# Interpolation function\n");
    int points = 500;
    double step = (data->b - data->a) / points;
    
    for (int i = 0; i <= points; i++) {
        double x = data->a + i * step;
        double y;
        
        if (data->method == 0) {
            y = lagrangeInterpolation(data, x);
        } else {
            y = newtonInterpolation(data, x, dd);
        }
        
        fprintf(file, "%lf %lf\n", x, y);
    }
    
    fclose(file);
    
    // Create gnuplot script file
    FILE *gnuplot = fopen("plot_script.gp", "w");
    if (gnuplot == NULL) {
        printf("Cannot open gnuplot script file.\n");
        return;
    }
    
    fprintf(gnuplot, "set title '%s Interpolation, %s nodes'\n", 
            data->method == 0 ? "Lagrange" : "Newton",
            data->node_type == 0 ? "uniform" : "Chebyshev");
    fprintf(gnuplot, "set grid\n");
    fprintf(gnuplot, "set key outside\n");
    fprintf(gnuplot, "plot 'interpolation_data.txt' index 0 with points pt 7 ps 1.5 title 'Nodes', ");
    fprintf(gnuplot, "'interpolation_data.txt' index 1 with lines lw 2 title 'Interpolation polynomial'\n");
    fprintf(gnuplot, "pause -1 'Press Enter to exit'\n");
    
    fclose(gnuplot);
    
    printf("Data saved to file. To display the plot, run gnuplot and type: load 'plot_script.gp'\n");
}