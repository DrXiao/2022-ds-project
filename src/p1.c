#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "parray.h"
#include "array.h"
#include "util.h"
#define P1_FORMAT "%u, %lf, %lf, %lf, %lf"
#define P1_FIELDS_IN(p1) &p1.date, &p1.o_price, &p1.h_price, &p1.l_price, &p1.c_price
#define P1_FIELDS_OUT(p1) p1.date, p1.o_price, p1.h_price, p1.l_price, p1.c_price

typedef struct p1 {
	uint32_t date;
	double o_price;
	double h_price;
	double l_price;
	double c_price;
} p1;


static int sscan_p1(const char *buf, void *struct_obj) {
	p1 *p1obj = (p1 *)struct_obj;
	return sscanf(buf, P1_FORMAT, P1_FIELDS_IN((*p1obj)));
}

static void print_p1(void *struct_obj) {
	p1 *p1obj = (p1 *)struct_obj;
	printf(P1_FORMAT"\n", P1_FIELDS_OUT((*p1obj)));
}

static void print_d(void *double_obj) {
	double *p = (double *)double_obj;
	printf("%lf\n", *p);
}

static int compare_p1_date(const void *r1, const void *r2) {
#if DS == 0
	const p1 *p1_r1 = (const p1 *)r1, *p1_r2 = (const p1 *)r2;
	return (*p1_r1).date - (*p1_r2).date;
#elif DS == 1
	const p1 **p1_r1 = (const p1 **)r1, **p1_r2 = (const p1 **)r2;
	return (**p1_r1).date - (**p1_r2).date;
#endif
}

static int compare_p1_c_price(const void *r1, const void *r2) {
#if DS == 0
	const p1 *p1_r1 = (const p1 *)r1, *p1_r2 = (const p1 *)r2;
	double diff = (*p1_r1).c_price - (*p1_r2).c_price;
#elif DS == 1
	const p1 **p1_r1 = (const p1 **)r1, **p1_r2 = (const p1 **)r2;
	double diff = (**p1_r1).c_price - (**p1_r2).c_price;
#endif
	if (diff < 0)
		return -1;
	else if (diff == 0)
		return 0;
	else
		return 1;
}
static int compare_double(const void *double1, const void *double2) {
#if DS == 0
	const double *d1 = (const double *)double1, *d2 = (const double *)double2;
	double diff = *d1 - *d2;
#elif DS == 1
	const double **d1 = (const double **)double1, **d2 = (const double **)double2;
	double diff = **d1 - **d2;
#endif
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
				"./p1 parray_file [sampled_flag]\n");
		exit(1);
	}

	char *parrayfile = argv[1];
	bool sampled_flag = atoi(argv[2]);
	
	struct timespec t[10];
	int time_idx = 0;
	
	GET_TIME(t + time_idx++);
	csv_op csvop = csv_op_init(sscan_p1, print_p1, sizeof(p1));
#if DS == 0
	array ds_obj = array_init(&csvop);
#elif DS == 1
	parray ds_obj = parray_init(&csvop);
#endif
	ds_obj.readcsv(&ds_obj, parrayfile);
#if DS == 0
	array set_obj = ds_obj.retset(&ds_obj, compare_p1_date);
#elif DS == 1
	parray set_obj = ds_obj.retset(&ds_obj, compare_p1_date);
#endif
	ds_obj.destroy(&ds_obj);
	if (sampled_flag) {
		set_obj.sampled(&set_obj, 5);
	}
	GET_TIME(t + time_idx++);
	uint32_t row_size = set_obj.retrows(&set_obj);
	p1 *row;
	
	// subprob 5
	double max_daily_ret = -1e19, min_daily_ret = 1e19;
	uint32_t max_dr1, max_dr2, min_dr1, min_dr2;
	
	// subprob 8
	FILE *subprob8 = fopen("output/daily_return_plot.csv", "w");

	p1 *pre_row = set_obj.getrow(&set_obj, 0);
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
	GET_TIME(t + time_idx++);

	// subprob 6
	double max_intraday_ret = -1e19, min_intraday_ret = 1e19;
	uint32_t max_ir, min_ir;

	// subprob 7
	FILE *subprob7 = fopen("output/close_price_plot.csv", "w");

	// subprob 9
	FILE *subprob9 = fopen("output/intraday_return_plot.csv", "w");

	for (int i = 0; i < row_size; i++) {
		p1 *row = set_obj.getrow(&set_obj, i);

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
	GET_TIME(t + time_idx++);

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
	row = set_obj.getrow(&set_obj, row_size >> 1);
	printf("%u, %lf", row->date, row->c_price);
	if (!(row_size & 1)) {	
		row = set_obj.getrow(&set_obj, (row_size >> 1) + 1);
		printf("%u, %lf\n", row->date, row->c_price);
	}
	printf("\n\n");
	GET_TIME(t + time_idx++);

	printf("subprob 5 - daily returns\n");
	printf("dates (%u, %u) -> max return: %lf\n", max_dr1, max_dr2, max_daily_ret);
	printf("dates (%u, %u) -> min return: %lf\n\n", min_dr1, min_dr2, min_daily_ret);

	printf("subprob 6 - intraday return\n");
	printf("date (%u) -> max return: %lf\n", max_ir, max_intraday_ret);
	printf("date (%u) -> min return: %lf\n\n", min_ir, min_intraday_ret);

	printf("subprob 10 - max, min, median prices\n");
	
	GET_TIME(t + time_idx++);
	csvop = csv_op_init(NULL, print_d, sizeof(double));
	parray double_parray = parray_init(&csvop);
	double_parray.create(&double_parray, row_size << 2);

	uint32_t double_parray_size = double_parray.retrows(&double_parray);
	for (int i = 0; i < double_parray_size; i += 4) {
		row = set_obj.getrow(&set_obj, i >> 2);
		double_parray.setrow(&double_parray, i + 0, &row->o_price);
		double_parray.setrow(&double_parray, i + 1, &row->h_price);
		double_parray.setrow(&double_parray, i + 2, &row->l_price);
		double_parray.setrow(&double_parray, i + 3, &row->c_price);
	}

	double_parray.sort(&double_parray, compare_double);

	double d_min, d_max, d_median1, d_median2, *d;
	d = double_parray.getrow(&double_parray, 0);
	d_min = *d;
	d = double_parray.getrow(&double_parray, double_parray_size - 1);
	d_max = *d;
	d = double_parray.getrow(&double_parray, double_parray_size >> 1);
	d_median1 = *d;
	d = double_parray.getrow(&double_parray, (double_parray_size >> 1) + 1);
	d_median2 = *d;

	printf("max price: %lf\n", d_max);
	printf("min price: %lf\n", d_min);
	printf("median prices: %lf, %lf\n\n", d_median1, d_median2);

	GET_TIME(t + time_idx++);

	fprintf(stderr, "prepare-dataset %ld\n", TIME_DIFF(t[0], t[1]));
	fprintf(stderr, "solve-5-8 %ld\n", TIME_DIFF(t[1], t[2]));
	fprintf(stderr, "solve-6-7-9 %ld\n", TIME_DIFF(t[2], t[3]));
	fprintf(stderr, "solve-1-2-3-4 %ld\n", TIME_DIFF(t[3], t[4]));
	fprintf(stderr, "solve-10 %ld\n", TIME_DIFF(t[5], t[6]));
	set_obj.destroy(&set_obj);
	double_parray.destroy(&double_parray);

	return 0;
}
