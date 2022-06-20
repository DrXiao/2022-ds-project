STD_C_FUNC := 0
DS := 0
CFLAGS := -DSTD_C_FUNC=$(STD_C_FUNC) -DDS=$(DS) -std=gnu99 -pedantic-errors -g

p1.elf: src/p1.c src/csv.c src/util.c src/array.c src/parray.c 
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

plotp2cmp:
	gnuplot p2cmp.gp
	eog plot/p2cmp.png

p2.elf: src/p2.c src/csv.c src/util.c src/array.c src/parray.c
	gcc $(CFLAGS) -o $@ $^ -Iinclude

p2: p2.elf
	taskset 0x2 ./$^ datasets/problem2/OptionsDaily_2017_05_1*.csv 

clean:
	rm p1.elf p2.elf
