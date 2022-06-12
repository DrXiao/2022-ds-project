reset
set xlabel 'term'
set xtics 1
set ylabel 'price'
set title 'price - plot'
set term png enhanced font 'Verdana,10'
set output 'result.png'
set key left


plot \
"output/close_price_plot.csv" using 1:3 with lines linewidth 1 title "price", \
