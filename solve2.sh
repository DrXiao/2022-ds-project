sudo sh ./performance.sh
make p2 DS=0 -B 2> output/p2-array.csv
make p2 DS=1 -B 2> output/p2-parray.csv
make plotp2cmp
sudo sh ./recovery.sh
