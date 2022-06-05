

p1.elf: src/p1.c src/csv.c
	gcc -o $@ $^ -Iinclude

p1: p1.elf
	./$^ datasets/problem1/TWII_withRepeatedData.csv

p2.elf: src/p2.c src/csv.c
	gcc -o $@ $^ -Iinclude

p2: p2.elf
	./$^ datasets/problem2/OptionsDaily_2017_05_15.csv 

clean:
	rm p1.elf p2.elf
