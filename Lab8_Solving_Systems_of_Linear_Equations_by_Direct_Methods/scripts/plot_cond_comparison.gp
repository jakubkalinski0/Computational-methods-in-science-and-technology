set terminal pngcairo enhanced size 1024,768 font 'Arial,10'
set output 'plots/cond_comparison.png'
set datafile separator ','
set title 'Condition Number Comparison (float64, N <= 20)'
set xlabel 'Matrix Size (N)'
set ylabel 'Condition Number (log scale)'
set logscale y
set format y '10^{%L}'
set key top left spacing 1.2
set grid
set xrange [1:21]
plot 'data/results_A_I.csv' using 1:7 every ::0::19 with linespoints pt 7 lc rgb 'orange' title 'Cond Num A_I (float64)', \
     'data/results_A_II.csv' using 1:7 every ::0::19 with linespoints pt 6 lc rgb 'green' title 'Cond Num A_II (float64)'
