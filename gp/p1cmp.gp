reset
set ylabel 'time (nsec)'
set style fill solid
set title 'DS performance'
set term png enhanced font 'Verdana,10'
set output 'plot/p1cmp.png'
set key right
set xtics rotate by 15 right

plot \
"output/p1-array.csv" using 2:xtic(1) with histogram title "array", \
"output/p1-parray.csv" using 2:xtic(1) with histogram title "parray", \
"output/p1-rbtree.csv" using 2:xtic(1) with histogram title "? ? ?", \
