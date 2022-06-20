sudo sh ./performance.sh
make p1 DS=0 -B 2> output/p1-array.csv
make p1 DS=1 -B 2> output/p1-parray.csv
make plotp1cmp
sudo sh ./recovery.sh
