
STD_C_QSORT := 1
DS := 0
CFLAGS := -DSTD_C_QSORT=$(STD_C_QSORT) -DDS=$(DS) -std=gnu99 -pedantic-errors -g

p1.elf: src/p1.c src/csv.c src/array.c src/parray.c 
	gcc $(CFLAGS) -o $@ $^ -Iinclude

p1: p1.elf
	taskset 0x2 ./$^ datasets/problem1/TWII_withRepeatedData.csv 0

p1-b: p1.elf
	taskset 0x2 ./$^ datasets/problem1/TWII_withRepeatedData.csv 1

plotp1-ans:
	gnuplot cp_plot.gp
	gnuplot dr_plot.gp
	gnuplot ir_plot.gp

plotp1cmp:
	gnuplot p1cmp.gp
	eog plot/p1cmp.png

p2.elf: src/p2.c src/csv.c src/array.c src/parray.c
	gcc $(CFLAGS) -o $@ $^ -Iinclude

p2: p2.elf
	taskset 0x2 ./$^ datasets/problem2/OptionsDaily_2017_05_1*.csv 

clean:
	rm p1.elf p2.elf
