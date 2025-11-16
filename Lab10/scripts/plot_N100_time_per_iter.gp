# Gnuplot: Avg Time per Iteration vs Rho for N=100 from data/jacobi_solver_results_all_criteria.csv
set terminal pngcairo enhanced size 1000,750 font 'Arial,10'
set output 'plots/N_100/N100_time_per_iter.png'
set title 'Jacobi: Avg. Time per Iteration vs Tolerance (N=100)'
set xlabel 'Tolerance (rho)'
set ylabel 'Time per Iteration (s)'
set logscale x
set datafile separator ','
set key outside right top
set grid
plot \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==100 && $2==0 && $3==0 ? $4 : 1/0):7 with linespoints title sprintf("X0:Zeros Crit:Increment"), \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==100 && $2==0 && $3==1 ? $4 : 1/0):7 with linespoints title sprintf("X0:Zeros Crit:Residual"), \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==100 && $2==3 && $3==0 ? $4 : 1/0):7 with linespoints title sprintf("X0:AltHigh Crit:Increment"), \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==100 && $2==3 && $3==1 ? $4 : 1/0):7 with linespoints title sprintf("X0:AltHigh Crit:Residual")
