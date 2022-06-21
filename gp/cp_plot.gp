reset
set xlabel 'term'
set xtics 0, 400
set ylabel 'price'
set title 'price - plot'
set term png enhanced font 'Verdana,10'
set output 'plot/cp_plot.png'
set key left


plot \
"output/close_price_plot.csv" using 1:2 with lines linewidth 1 title "close price", \
