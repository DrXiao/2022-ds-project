#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "rbtree.h"
#include "util.h"
#define P1_FORMAT "%u, %lf, %lf, %lf, %lf"
#define P1_FIELDS_IN(p1)                                                       \
	&p1.date, &p1.o_price, &p1.h_price, &p1.l_price, &p1.c_price
#define P1_FIELDS_OUT(p1)                                                      \
	p1.date, p1.o_price, p1.h_price, p1.l_price, p1.c_price

typedef struct p1 {
	uint32_t date;
	double o_price;
	double h_price;
	double l_price;
	double c_price;
} p1;

// subprob 1
uint32_t row_size;

// subprob 5
double max_daily_ret = -1e19, min_daily_ret = 1e19;
uint32_t max_dr1, max_dr2, min_dr1, min_dr2;

// subprob 8
FILE *subprob8;

// subprob 6
double max_intraday_ret = -1e19, min_intraday_ret = 1e19;
uint32_t max_ir, min_ir;

// subprob 7
FILE *subprob7;

// subprob 9
FILE *subprob9;

// subprob 10
double d_min = 1e19, d_max = 1e-19, d_median1, d_median2, *d;

static int sscan_p1(const char *buf, void *struct_obj) {
	p1 *p1obj = (p1 *)struct_obj;
	return sscanf(buf, P1_FORMAT, P1_FIELDS_IN((*p1obj)));
}

static void print_p1(void *struct_obj) {
	p1 *p1obj = (p1 *)struct_obj;
	printf(P1_FORMAT "\n", P1_FIELDS_OUT((*p1obj)));
}

static void print_d(void *double_obj) {
	double *p = (double *)double_obj;
	printf("%lf\n", *p);
}

static int compare_p1_date(const void *r1, const void *r2) {
	const p1 *p1_r1 = (const p1 *)r1, *p1_r2 = (const p1 *)r2;
	return (*p1_r1).date - (*p1_r2).date;
}

static int compare_p1_c_price_date(const void *r1, const void *r2) {
	const p1 *p1_r1 = (const p1 *)r1, *p1_r2 = (const p1 *)r2;
	double diff = (*p1_r1).c_price - (*p1_r2).c_price;
	if (diff < 0)
		return -1;
	else if (diff == 0)
		return (*p1_r1).date - (*p1_r2).date;
	else
		return 1;
}

typedef struct double_date {
	double val;
	uint32_t date;
} double_date;

static int compare_double_date(const void *double1, const void *double2) {
	const double_date *d1 = (const double_date *)double1,
			  *d2 = (const double_date *)double2;
	double diff = (*d1).val - (*d2).val;
	if (diff < 0)
		return -1;
	else if (diff == 0)
		return (*d1).date - (*d2).date;
	else
		return 1;
}

static void solve5_8(void *data) {
	static double pre_price = -1;
	static uint32_t pre_date = 0;
	static int i = 0;
	p1 *val = (p1 *)data;
	double c_price = val->c_price;
	uint32_t date = val->date;
	if (pre_date) {
		double c_price_diff = (c_price - pre_price) / pre_price * 100;
		if (c_price_diff > max_daily_ret) {
			max_dr1 = pre_date;
			max_dr2 = date;
			max_daily_ret = c_price_diff;
		}
		if (c_price_diff < min_daily_ret) {
			min_dr1 = pre_date;
			min_dr2 = date;
			min_daily_ret = c_price_diff;
		}
		fprintf(subprob8, "%d, %lf\n", i, c_price_diff);
	}
	pre_price = c_price;
	pre_date = date;
	i++;
}

static void solve6_7_9(void *data) {
	static int i = 0;
	p1 *val = (p1 *)data;
	double c_price = val->c_price, o_price = val->o_price;
	double intraday_ret = (c_price - o_price) / o_price * 100;
	uint32_t date = val->date;

	if (intraday_ret > max_intraday_ret) {
		max_ir = date;
		max_intraday_ret = intraday_ret;
	}
	if (intraday_ret < min_intraday_ret) {
		min_ir = date;
		min_intraday_ret = intraday_ret;
	}
	fprintf(subprob7, "%d, %lf\n", i + 1, c_price);
	fprintf(subprob9, "%d, %lf\n", i + 1, intraday_ret);
	i++;
}

static double_date subp2[10];
static double_date subp3[10];
static double_date subp4[2];

static void solve2_3_4(void *data) {
	static int i = 0;
	p1 *val = (p1 *)data;

	// subprob 2 ~ 4
	if (i < 10) {
		subp2[i] =
			(double_date){.date = val->date, .val = val->c_price};
	}
	if (i == (row_size >> 1) ||
	    (!(row_size & 1) && ((row_size >> 1) + 1) == i)) {
		subp4[i - (row_size >> 1)] =
			(double_date){.date = val->date, .val = val->c_price};
	}
	if (row_size - i - 1 < 10) {
		subp3[row_size - i - 1] =
			(double_date){.date = val->date, .val = val->c_price};
	}
	i++;
}

rbtree set_obj;
rbtree double_data;
uint32_t double_nodes;
static void prepare_10_data(void *data) {
	p1 *val = (p1 *)data;
	double_data.insert(&double_data, &val->o_price);
	double_data.insert(&double_data, &val->h_price);
	double_data.insert(&double_data, &val->l_price);
	double_data.insert(&double_data, &val->c_price);
}

static void solve_10(void *data) {
	static int i = 0;
	double *d = (double *)data;
	if (i == (double_nodes >> 1))
		d_median1 = *d;
	if (i == ((double_nodes >> 1) + 1))
		d_median2 = *d;
	if (*d > d_max)
		d_max = *d;
	if (*d < d_min)
		d_min = *d;
	i++;
}

int main(int argc, char *argv[]) {

	if (argc < 3) {
		fprintf(stderr, "Usage:\n"
				"./p1 csv_file [sampled_flag]\n");
		exit(1);
	}

	char *csvfile = argv[1];
	bool sampled_flag = atoi(argv[2]);

	struct timespec t[10];
	int time_idx = 0;

	GET_TIME(t + time_idx++);

	set_obj = rbtree_init(sizeof(p1), compare_p1_date);

	set_obj.readcsv(&set_obj, csvfile, sscan_p1);

	if (sampled_flag) {
		set_obj.sampled(&set_obj, 5);
	}

	GET_TIME(t + time_idx++);
	row_size = set_obj.retnodes(&set_obj);
	p1 *row;
	// subprob 1
	printf("subprob1 - unique dates: %u\n\n", row_size);

	// subprob 5, 8
	subprob8 = fopen("output/daily_return_plot.csv", "w");
	set_obj.inorder(&set_obj, solve5_8);
	fclose(subprob8);
	GET_TIME(t + time_idx++);

	// subprob 6, 7, 9
	subprob7 = fopen("output/close_price_plot.csv", "w");
	subprob9 = fopen("output/intraday_return_plot.csv", "w");
	set_obj.inorder(&set_obj, solve6_7_9);
	fclose(subprob7);
	fclose(subprob9);
	GET_TIME(t + time_idx++);

	rbtree newtree = set_obj.rebuild(&set_obj, compare_p1_c_price_date);
	newtree.inorder(&newtree, solve2_3_4);
	printf("subprob2 - 10 smallest prices and dates\n");
	for (int i = 0; i < 10; i++)
		printf("%u, %lf\n", subp2[i].date, subp2[i].val);
	printf("\n");
	printf("subprob3 - 10 largest prices and dates\n");
	for (int i = 0; i < 10; i++)
		printf("%u, %lf\n", subp3[i].date, subp3[i].val);
	printf("\n");

	printf("subprob 4 - median price and date\n");
	printf("%u, %lf\n", subp4[0].date, subp4[0].val);
	if (!(row_size & 1))
		printf("%u, %lf\n", subp4[1].date, subp4[1].val);
	printf("\n\n");
	GET_TIME(t + time_idx++);

	printf("subprob 5 - daily returns\n");
	printf("dates (%u, %u) -> max return: %lf\n", max_dr1, max_dr2,
	       max_daily_ret);
	printf("dates (%u, %u) -> min return: %lf\n\n", min_dr1, min_dr2,
	       min_daily_ret);

	printf("subprob 6 - intraday return\n");
	printf("date (%u) -> max return: %lf\n", max_ir, max_intraday_ret);
	printf("date (%u) -> min return: %lf\n\n", min_ir, min_intraday_ret);

	printf("subprob 10 - max, min, median prices\n");

	GET_TIME(t + time_idx++);

	double_data = rbtree_init(sizeof(double_date), compare_double_date);
	set_obj.inorder(&set_obj, prepare_10_data);
	double_nodes = double_data.retnodes(&double_data);
	double_data.inorder(&double_data, solve_10);
	printf("max price: %lf\n", d_max);
	printf("min price: %lf\n", d_min);
	printf("median prices: %lf, %lf\n\n", d_median1, d_median2);

	GET_TIME(t + time_idx++);

	set_obj.destroy(&set_obj);
	newtree.destroy(&newtree);
	double_data.destroy(&double_data);

	fprintf(stderr, "solve-dataset-1 %ld\n", TIME_DIFF(t[0], t[1]));
	fprintf(stderr, "solve-5-8 %ld\n", TIME_DIFF(t[1], t[2]));
	fprintf(stderr, "solve-6-7-9 %ld\n", TIME_DIFF(t[2], t[3]));
	fprintf(stderr, "solve-2-3-4 %ld\n", TIME_DIFF(t[3], t[4]));
	fprintf(stderr, "solve-10 %ld\n", TIME_DIFF(t[5], t[6]));

	return 0;
}
