# Gnuplot script for spectral radius from data/spectral_radii_data.csv
set terminal pngcairo enhanced size 1000,700 font 'Arial,10'
set output 'plots/spectral_radii_data.png'
set title 'Spectral Radius of Jacobi Iteration Matrix vs. System Size N'
set xlabel 'System Size (N)'
set ylabel 'Spectral Radius rho(T_J)'
set datafile separator ','
set key top left
set grid
set yrange [0:1.1]
plot 'data/spectral_radii_data.csv' using 1:2 with linespoints pt 7 lc rgb 'blue' title 'Spectral Radius rho(T_J)'

