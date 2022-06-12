
STD_C_QSORT := 1
CFLAGS := -DSTD_C_QSORT=$(STD_C_QSORT) -g

p1.elf: src/p1.c src/csv.c
	gcc $(CFLAGS) -o $@ $^ -Iinclude

p1: p1.elf
	./$^ datasets/problem1/TWII_withRepeatedData.csv 0

p1-b: p1.elf
	./$^ datasets/problem1/TWII_withRepeatedData.csv 1

plotp1:
	gnuplot cp_plot.gp
	gnuplot dr_plot.gp
	gnuplot ir_plot.gp

p2.elf: src/p2.c src/csv.c
	gcc $(CFLAGS) -o $@ $^ -Iinclude

p2: p2.elf
	./$^ datasets/problem2/OptionsDaily_2017_05_15.csv 

clean:
	rm p1.elf p2.elf
