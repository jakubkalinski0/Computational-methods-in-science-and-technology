# Gnuplot: Final Absolute Error vs Rho for N=50 from data/jacobi_solver_results_all_criteria.csv
set terminal pngcairo enhanced size 1000,750 font 'Arial,10'
set output 'plots/N_50/N50_abs_error.png'
set title 'Jacobi: Max Absolute Error vs Tolerance (N=50)'
set xlabel 'Tolerance (rho)'
set ylabel 'Max Absolute Error ||x_{comp} - x_{true}||_{max}'
set logscale x
set logscale y
set datafile separator ','
set key outside right top
set grid
plot \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==50 && $2==0 && $3==0 ? $4 : 1/0):8 with linespoints title sprintf("X0:Zeros Crit:Increment"), \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==50 && $2==0 && $3==1 ? $4 : 1/0):8 with linespoints title sprintf("X0:Zeros Crit:Residual"), \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==50 && $2==3 && $3==0 ? $4 : 1/0):8 with linespoints title sprintf("X0:AltHigh Crit:Increment"), \
     'data/jacobi_solver_results_all_criteria.csv' using ($1==50 && $2==3 && $3==1 ? $4 : 1/0):8 with linespoints title sprintf("X0:AltHigh Crit:Residual")
