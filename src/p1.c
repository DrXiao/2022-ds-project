#include <stdio.h>
#include <stdlib.h>
#include "csv.h"
#define P1_FORMAT "%8s, %lf, %lf, %lf, %lf"
#define P1_FIELDS_IN(p1) p1.date, &p1.o_price, &p1.h_price, &p1.l_price, &p1.c_price
#define P1_FIELDS_OUT(p1) p1.date, p1.o_price, p1.h_price, p1.l_price, p1.c_price


typedef struct p1_csv {
	char date[16];
	double o_price;
	double h_price;
	double l_price;
	double c_price;
} p1_csv;


static int sscan_p1(const char * buf, void *struct_obj) {
	p1_csv *p1 = struct_obj;
	return sscanf(buf, P1_FORMAT, P1_FIELDS_IN((*p1)));
}

static void print_p1(void *struct_obj) {
	p1_csv *p1 = struct_obj;
	printf(P1_FORMAT"\n", P1_FIELDS_OUT((*p1)));
}


int main(int argc, char **argv) {
	
	if (argc < 2) {
		fprintf(stderr, "Usage:\n"
				"./p1 csv_file\n");
		exit(1);
	}

	char *csvfile = argv[1];

	csv_op csvop = csv_op_init(sscan_p1, print_p1, sizeof(p1_csv));

	csv csv_obj = csv_init(&csvop);
	
	csv_obj.readcsv(&csv_obj, csvfile);

	csv_obj.showrow(&csv_obj, 1);

	csv_obj.destroy(&csv_obj);

	return 0;
}
