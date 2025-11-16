# Gnuplot: Iterations vs Rho for N=1000 from data/jacobi_solver_results_all_criteria.csv
set terminal pngcairo enhanced size 1000,750 font 'Arial,10'
set output 'plots/N_1000/N1000_iterations.png'
set title 'Jacobi: Iterations vs Tolerance (N=1000)'
set xlabel 'Tolerance (rho)'
set ylabel 'Number of Iterations'
set logscale x
set logscale y
set datafile separator ','
set key outside right top
set grid
plot \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==1000 && $2==0 && $3==0 ? $4 : 1/0):5 with linespoints title sprintf("X0:Zeros Crit:Increment"), \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==1000 && $2==0 && $3==1 ? $4 : 1/0):5 with linespoints title sprintf("X0:Zeros Crit:Residual"), \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==1000 && $2==3 && $3==0 ? $4 : 1/0):5 with linespoints title sprintf("X0:AltHigh Crit:Increment"), \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==1000 && $2==3 && $3==1 ? $4 : 1/0):5 with linespoints title sprintf("X0:AltHigh Crit:Residual")
