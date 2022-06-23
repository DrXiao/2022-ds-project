CC := gcc
STD_C_FUNC := 0
DS := 0
CFLAGS := -DSTD_C_FUNC=$(STD_C_FUNC) -DDS=$(DS) -std=gnu99 -pedantic-errors -g

p1.elf: p1.c src/*.c
	$(CC) $(CFLAGS) -o $@ $^ -Iinclude

p1-tree.elf: p1-tree.c src/*.c
	$(CC) $(CFLAGS) -o $@ $^ -Iinclude

p1: p1.elf
	taskset 0x2 ./$^ datasets/problem1/TWII_withRepeatedData.csv 0

p1-tree: p1-tree.elf
	taskset 0x2 ./$^ datasets/problem1/TWII_withRepeatedData.csv 0

plotp1-ans:
	gnuplot gp/cp_plot.gp
	gnuplot gp/dr_plot.gp
	gnuplot gp/ir_plot.gp

plotp1cmp:
	gnuplot gp/p1cmp.gp
	eog plot/p1cmp.png

p2.elf: p2.c src/*.c
	$(CC) $(CFLAGS) -o $@ $^ -Iinclude

p2: p2.elf
	taskset 0x2 ./$^ datasets/problem2/OptionsDaily_2017_05_1*.csv 

plotp2cmp:
	gnuplot gp/p2cmp.gp
	eog plot/p2cmp.png

validate: validate.c src/util.c src/rbtree.c src/queue.c
	$(CC) $(CFLAGS) -o rbtree.elf $^ -Iinclude


clean:
	rm p1.elf p2.elf
