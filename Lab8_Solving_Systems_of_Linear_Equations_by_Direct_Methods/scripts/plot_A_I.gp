# Plik: scripts/plot_A_I.gp
# Skrypt Gnuplot dla wyników macierzy A_I

set terminal pngcairo enhanced size 1024,768 font 'Arial,10'
set datafile separator ','
set key top left spacing 1.2
set grid

# Wartość zastępująca zero na wykresie logarytmicznym błędów
error_zero_replacement = 1e-18
replace_error_zero(col_val) = (col_val == 0.0 ? error_zero_replacement : col_val)

# --- Wykres maksymalnego błędu absolutnego ---
set output 'plots/A_I_error.png'
set title 'Max Absolute Error vs Size (Matrix A_I)'
set xlabel 'Matrix Size (N)'
set xrange [1:21]
set ylabel 'Max Absolute Error (log scale)'
set logscale y
set format y '10^{%L}'
set yrange [error_zero_replacement/10 : *]

plot 'data/results_A_I.csv' using 1:(replace_error_zero(column(2))) with linespoints pt 7 lc rgb 'blue' title 'Error (float32)', \
     '' using 1:(replace_error_zero(column(6))) with linespoints pt 6 lc rgb 'red' title 'Error (float64)'


# --- Wykres współczynnika uwarunkowania ---
set output 'plots/A_I_cond.png'
set title 'Condition Number vs Size (Matrix A_I)'
set xlabel 'Matrix Size (N)'
set xrange [1:21]
set ylabel 'Condition Number (log scale)'
set logscale y
set format y '10^{%L}'
set yrange [*:*]

plot 'data/results_A_I.csv' using 1:3 with linespoints pt 7 lc rgb 'blue' title 'Cond Num (float32)', \
     '' u 1:7 with linespoints pt 6 lc rgb 'red' title 'Cond Num (float64)'


# --- Wykres czasu rozwiązywania ---
set output 'plots/A_I_time_solve.png'
set title 'Solve Time vs Size (Matrix A_I)'
set xlabel 'Matrix Size (N)'
set xrange [1:21]
set nologscale y
set format y '%.6f'
set ylabel 'Time (seconds)'
set yrange [0:*]
set ytics auto

plot 'data/results_A_I.csv' using 1:4 with linespoints pt 7 lc rgb 'blue' title 'Time Solve (float32)', \
     '' u 1:8 with linespoints pt 6 lc rgb 'red' title 'Time Solve (float64)'


# --- Wykres czasu obliczania współczynnika uwarunkowania ---
set output 'plots/A_I_time_cond.png'
set title 'Condition Number Calc Time vs Size (Matrix A_I)'
set xlabel 'Matrix Size (N)'
set xrange [1:21]
set nologscale y
set format y '%.6f'
set ylabel 'Time (seconds)'
set yrange [0:*]
set ytics auto

plot 'data/results_A_I.csv' using 1:5 with linespoints pt 7 lc rgb 'blue' title 'Time Cond (float32)', \
     '' u 1:9 with linespoints pt 6 lc rgb 'red' title 'Time Cond (float64)'


print "Generated plots for A_I."
