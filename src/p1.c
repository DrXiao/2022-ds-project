#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "csv.h"
#define P1_FORMAT "%u, %lf, %lf, %lf, %lf"
#define P1_FIELDS_IN(p1) &p1.date, &p1.o_price, &p1.h_price, &p1.l_price, &p1.c_price
#define P1_FIELDS_OUT(p1) p1.date, p1.o_price, p1.h_price, p1.l_price, p1.c_price


typedef struct p1_csv {
	uint32_t date;
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

static int compare_p1_date(const void *r1, const void *r2) {
	const p1_csv **p1_r1 = r1, **p1_r2 = r2;
	return (**p1_r1).date - (**p1_r2).date;
}

static int compare_p1_c_price(const void *r1, const void *r2) {
	const p1_csv **p1_r1 = r1, **p1_r2 = r2;
	double diff = (**p1_r1).c_price - (**p1_r2).c_price;
	if (diff < 0)
		return -1;
	else if (diff == 0)
		return 0;
	else
		return 1;
}

int main(int argc, char *argv[]) {
	
	if (argc < 3) {
		fprintf(stderr, "Usage:\n"
				"./p1 csv_file [sampled_flag]\n");
		exit(1);
	}

	char *csvfile = argv[1];
	bool sampled_flag = atoi(argv[2]);

	csv_op csvop = csv_op_init(sscan_p1, print_p1, sizeof(p1_csv));
	csv csv_obj = csv_init(&csvop);
	csv_obj.readcsv(&csv_obj, csvfile);
	csv set_obj = csv_obj.retset(&csv_obj, compare_p1_date);
	csv_obj.destroy(&csv_obj);
	uint32_t row_size = set_obj.__row;
	p1_csv *row;

	
	// subprob 5
	double max_daily_ret = -1e19, min_daily_ret = 1e19;
	uint32_t max_dr1, max_dr2, min_dr1, min_dr2;
	
	// subprob 8
	FILE *subprob8 = fopen("output/daily_return_plot.csv", "w");

	p1_csv *pre_row = set_obj.getrow(&set_obj, 0);
	for (int i = 1; i < row_size; i++) {
		row = set_obj.getrow(&set_obj, i);
		double c_price_diff = (row->c_price - pre_row->c_price) / pre_row->c_price * 100;
		
		if (c_price_diff > max_daily_ret) {
			max_dr1 = pre_row->date;
			max_dr2 = row->date;
			max_daily_ret = c_price_diff;
		}
		if (c_price_diff < min_daily_ret) {
			min_dr1 = pre_row->date;
			min_dr2 = row->date;
			min_daily_ret = c_price_diff;
		}

		fprintf(subprob8, "%d, %lf\n", i, c_price_diff);
		pre_row = row;
	}
	fclose(subprob8);

	

	// subprob 6
	double max_intraday_ret = -1e19, min_intraday_ret = 1e19;
	uint32_t max_ir, min_ir;

	// subprob 7
	FILE *subprob7 = fopen("output/close_price_plot.csv", "w");

	// subprob 9
	FILE *subprob9 = fopen("output/intraday_return_plot.csv", "w");

	for (int i = 0; i < row_size; i++) {
		p1_csv *row = set_obj.getrow(&set_obj, i);

		double intraday_ret = (row->c_price - row->o_price) / row->o_price * 100;

		if (intraday_ret > max_intraday_ret) {
			max_ir = row->date;
			max_intraday_ret = intraday_ret;
		}
		if (intraday_ret < min_intraday_ret) {
			min_ir = row->date;
			min_intraday_ret = intraday_ret;
		}

		fprintf(subprob7, "%d, %lf\n", i + 1, row->c_price);
		fprintf(subprob9, "%d, %lf\n", i + 1, intraday_ret);
	}
	fclose(subprob7);
	fclose(subprob9);


	// subprob 1
	printf("subprob1 - unique dates: %u\n\n", row_size);

	// subprob 2 ~ 4
	set_obj.sort(&set_obj, compare_p1_c_price);
	printf("subprob2 - 10 smallest prices and dates\n");
	for (int i = 0; i < 10; i++) {
		row = set_obj.getrow(&set_obj, i);
		printf("%u, %lf\n", row->date, row->c_price);
	}
	printf("\n");

	printf("subprob3 - 10 largest prices and dates\n");
	for (int i = 0; i < 10; i++) {
		row = set_obj.getrow(&set_obj, row_size - i - 1);
		printf("%u, %lf\n", row->date, row->c_price);
	}
	printf("\n");

	printf("subprob 4 - median price and date\n");
	row = set_obj.getrow(&set_obj, row_size / 2);
	printf("%u, %lf", row->date, row->c_price);
	if (!(row_size & 1)) {	
		row = set_obj.getrow(&set_obj, row_size / 2 + 1);
		printf("%u, %lf\n", row->date, row->c_price);
	}
	printf("\n");

	printf("subprob 5 - daily returns\n");
	printf("dates (%u, %u) -> max return: %lf\n", max_dr1, max_dr2, max_daily_ret);
	printf("dates (%u, %u) -> min return: %lf\n\n", min_dr1, min_dr2, min_daily_ret);

	printf("subprob 6 - intraday return\n");
	printf("date (%u) -> max return: %lf\n", max_ir, max_intraday_ret);
	printf("date (%u) -> min return: %lf\n", min_ir, min_intraday_ret);

	set_obj.destroy(&set_obj);

	return 0;
}
