sudo sh ./performance.sh
make p1 DS=0 -B 2> output/p1-array.csv
make p1 DS=1 -B 2> output/p1-parray.csv
make p1-tree DS=2 -B 2> output/p1-rbtree.csv
make plotp1cmp
cmp output/p1-array-ans.txt output/p1-parray-ans.txt
cmp output/p1-array-ans.txt output/p1-rbtree-ans.txt
sudo sh ./recovery.sh
