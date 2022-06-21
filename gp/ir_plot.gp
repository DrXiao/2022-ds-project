reset
set xlabel 'term'
set xtics 0, 400
set ylabel '%'
set title 'price - plot'
set term png enhanced font 'Verdana,10'
set output 'plot/ir_plot.png'
set key left


plot \
"output/intraday_return_plot.csv" using 1:2 with lines linewidth 1 title "intraday return", \
