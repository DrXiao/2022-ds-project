#include <stdio.h>
#include <stdlib.h>
#include "csv.h"
#define P2_FORMAT_IN  "%u , %3s , %lf , %6s , %c , %6s , %lf , %u , %*u"
#define P2_FORMAT_OUT "%s, %s, %lf, %s, %c, %s, %lf, %u"
#define P2_FIELDS_IN(p2) p2.date, p2.item_id, &p2.item_price, p2.deadline, &p2.auth, p2.deal_time, &p2.deal_price, &p2.dead_num
#define P2_FIELDS_OUT(p2) p2.date, p2.item_id, p2.item_price, p2.deadline, p2.auth, p2.deal_time, p2.deal_price, p2.dead_num

typedef struct p2_csv {
	uint32_t date;
	char item_id[4];
	double item_price;
	char deadline[7];
	char auth;
	char deal_time[8];
	double deal_price;
	uint32_t dead_num;
} p2_csv;



int main(int argc, char **argv) {
	
	if (argc < 2) {
		fprintf(stderr, "Usage:\n"
				"./p1 csv_file\n");
		exit(1);
	}

	char *csvfile = argv[1];

	csv_op csvop;

	csv csv_obj = csv_init(&csvop);
	csv_obj.readcsv(&csv_obj, csvfile);

	return 0;
}
