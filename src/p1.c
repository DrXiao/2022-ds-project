#include <stdio.h>
#include <stdlib.h>
#include "csv.h"

int main(int argc, char **argv) {
	
	if (argc < 2) {
		fprintf(stderr, "Usage:\n"
				"./p1 csv_file\n");
		exit(1);
	}

	char *csvfile = argv[1];

	csv csv_obj = csv_init(1);
	csv_obj.readcsv(&csv_obj, csvfile);

	return 0;
}
