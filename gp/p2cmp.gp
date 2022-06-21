reset
set ylabel 'time (nsec)'
set style fill solid
set title 'DS performance'
set term png enhanced font 'Verdana,10'
set output 'plot/p2cmp.png'
set key right
set xtics rotate by 15 right

plot \
"output/p2-array.csv" using 2:xtic(1) with histogram title "array", \
"output/p2-parray.csv" using 2:xtic(1) with histogram title "parray", \
