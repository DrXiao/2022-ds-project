#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "array.h"
#include "parray.h"
#include "util.h"
#define P2_FORMAT_IN "%u , %s , %s , %s , %s , %u , %lf , %u , %*u"
#define P2_FORMAT_OUT "%u, %s, %s, %s, %s, %u, %lf, %u"
#define P2_FIELDS_IN(p2)                                                       \
	&p2.date, p2.item_id, p2.item_price, p2.deadline, p2.auth,             \
		&p2.deal_time, &p2.deal_price, &p2.deal_num
#define P2_FIELDS_OUT(p2)                                                      \
	p2.date, p2.item_id, p2.item_price, p2.deadline, p2.auth,              \
		p2.deal_time, p2.deal_price, p2.deal_num

typedef struct p2 {
	uint32_t date;
	char item_id[4];
	char item_price[16];
	char deadline[16];
	char auth[4];
	uint32_t deal_time;
	double deal_price;
	uint32_t deal_num;
} p2;

static int sscan_p2(const char *buf, void *struct_obj) {
	p2 *p2 = struct_obj;
	return sscanf(buf, P2_FORMAT_IN, P2_FIELDS_IN((*p2)));
}

static void print_p2(void *struct_obj) {
	p2 *p2 = struct_obj;
	printf(P2_FORMAT_OUT "\n", P2_FIELDS_OUT((*p2)));
}

static int cmp_product_id(const void *product1, const void *product2) {
	char strproduct1[32] = "", strproduct2[32] = "";
#if DS == 0
	p2 *pp1 = (p2 *)product1, *pp2 = (p2 *)product2;
	strcat(strproduct1, pp1->item_id);
	strcat(strproduct1, pp1->item_price);
	strcat(strproduct1, pp1->deadline);
	strcat(strproduct1, pp1->auth);

	strcat(strproduct2, pp2->item_id);
	strcat(strproduct2, pp2->item_price);
	strcat(strproduct2, pp2->deadline);
	strcat(strproduct2, pp2->auth);
#elif DS == 1
	p2 **pp1 = (p2 **)product1, **pp2 = (p2 **)product2;
	strcat(strproduct1, (*pp1)->item_id);
	strcat(strproduct1, (*pp1)->item_price);
	strcat(strproduct1, (*pp1)->deadline);
	strcat(strproduct1, (*pp1)->auth);

	strcat(strproduct2, (*pp2)->item_id);
	strcat(strproduct2, (*pp2)->item_price);
	strcat(strproduct2, (*pp2)->deadline);
	strcat(strproduct2, (*pp2)->auth);
#endif
	return strcmp(strproduct1, strproduct2);
}

static int cmp_p2(const void *r1, const void *r2) {
	int id_diff, date_diff, time_diff, deal_price_diff, deal_num_diff;
#if DS == 0
	p2 *p2_r1 = (p2 *)r1, *p2_r2 = (p2 *)r2;
	id_diff = cmp_product_id(r1, r2);
	date_diff = (*p2_r1).date - (*p2_r2).date;
	time_diff = (*p2_r1).deal_time - (*p2_r2).deal_time;
	deal_price_diff = (*p2_r1).deal_price - (*p2_r2).deal_price;
	deal_num_diff = (*p2_r1).deal_num - (*p2_r2).deal_num;
#elif DS == 1
	p2 **p2_r1 = (p2 **)r1, **p2_r2 = (p2 **)r2;
	id_diff = cmp_product_id(r1, r2);
	date_diff = (**p2_r1).date - (**p2_r2).date;
	time_diff = (**p2_r1).deal_time - (**p2_r2).deal_time;
	deal_price_diff = (**p2_r1).deal_price - (**p2_r2).deal_price;
	deal_num_diff = (**p2_r1).deal_num - (**p2_r2).deal_num;
#endif
	if (id_diff)
		return id_diff;
	else if (deal_price_diff)
		return deal_price_diff;
	else if (date_diff)
		return date_diff;
	else if (time_diff)
		return time_diff;
	else if (deal_num_diff)
		return deal_num_diff;
	else
		return -1;
}

static int cmp_p2_time(const void *r1, const void *r2) {
	int id_diff, date_diff, time_diff, deal_price_diff, deal_num_diff;
#if DS == 0
	p2 *p2_r1 = (p2 *)r1, *p2_r2 = (p2 *)r2;
	id_diff = cmp_product_id(r1, r2);
	date_diff = (*p2_r1).date - (*p2_r2).date;
	time_diff = (*p2_r1).deal_time - (*p2_r2).deal_time;
	deal_price_diff = (*p2_r1).deal_price - (*p2_r2).deal_price;
	deal_num_diff = (*p2_r1).deal_num - (*p2_r2).deal_num;
#elif DS == 1
	p2 **p2_r1 = (p2 **)r1, **p2_r2 = (p2 **)r2;
	id_diff = cmp_product_id(r1, r2);
	date_diff = (**p2_r1).date - (**p2_r2).date;
	time_diff = (**p2_r1).deal_time - (**p2_r2).deal_time;
	deal_price_diff = (**p2_r1).deal_price - (**p2_r2).deal_price;
	deal_num_diff = (**p2_r1).deal_num - (**p2_r2).deal_num;
#endif
	if (id_diff)
		return id_diff;
	else if (date_diff)
		return date_diff;
	else if (time_diff)
		return time_diff;
	else
		return -1;
}

int main(int argc, char **argv) {

	if (argc < 2) {
		fprintf(stderr,
			"Usage:\n"
			"./p1 parray_file1 [parray_file2 [parray_file3...]]\n");
		exit(1);
	}

	struct timespec t[10];
	int time_idx = 0;

#if DS == 0
	array ds_obj = array_init(sizeof(p2));
#elif DS == 1
	parray ds_obj = parray_init(sizeof(p2));
#endif
	GET_TIME(t + time_idx++);
	for (int i = 0; i < 5; i++) {
		char *csvfile = argv[1 + i];
#if DS == 0
		array obj = array_init(sizeof(p2));
#elif DS == 1
		parray obj = parray_init(sizeof(p2));
#endif
		obj.readcsv(&obj, csvfile, sscan_p2);
		ds_obj.merge(&ds_obj, &obj);
		obj.destroy(&obj);
	}
#if DS == 0
	array set_obj = ds_obj.retset(&ds_obj, cmp_product_id);
#elif DS == 1
	parray set_obj = ds_obj.retset(&ds_obj, cmp_product_id);
#endif
	GET_TIME(t + time_idx++);

	printf("(1) total products: %u\n", set_obj.retrows(&set_obj));

	p2 target = (p2){.item_id = "TXO",
			 .item_price = "1000",
			 .deadline = "201706",
			 .auth = "P"};

	GET_TIME(t + time_idx++);
	void *ret = set_obj.search(&set_obj, &target, cmp_product_id);
	printf("(2) Does TXO_1000_201706_P exist : %d\n", (ret != NULL));

	target = (p2){.item_id = "TXO",
		      .item_price = "9500",
		      .deadline = "201706",
		      .auth = "C"};

	ret = set_obj.search(&set_obj, &target, cmp_product_id);
	printf("(3) Does TXO_9500_201706_C exist : %d\n", (ret != NULL));

	target = (p2){.item_id = "GIO",
		      .item_price = "5500",
		      .deadline = "201706",
		      .auth = "C"};

	ret = set_obj.search(&set_obj, &target, cmp_product_id);

	printf("(4) Does GIO_5500_201706_C exist : %d\n", (ret != NULL));

	GET_TIME(t + time_idx++);
	set_obj.destroy(&set_obj);

	GET_TIME(t + time_idx++);
	set_obj = ds_obj.retset(&ds_obj, cmp_p2);
	uint32_t row_size = set_obj.retrows(&set_obj);

	target = (p2){.item_id = "TXO",
		      .item_price = "9900",
		      .deadline = "201705",
		      .auth = "C"};
#if DS == 1
	p2 *iptr = &target;
#endif
	int start = -1, end = -1;
	p2 *row = NULL;
	for (int i = 0; i < row_size; i++) {
		row = set_obj.getrow(&set_obj, i);
#if DS == 0
		int ret = cmp_product_id(row, &target);
#elif DS == 1
		int ret = cmp_product_id(&row, &iptr);
#endif
		if (!ret) {
			if (start < 0)
				start = i;
		}
		else if (start != -1 && end == -1)
			end = i - 1;
	}

	printf("(5a) Find 10 smallest prices\n");
	for (int i = 0; i < 10; i++) {
		row = set_obj.getrow(&set_obj, start + i);
		printf("%u\t%u\t%lf\n", row->date, row->deal_time,
		       row->deal_price);
	}
	printf("\n");

	printf("(5b) Find 10 largest prices\n");
	for (int i = 0; i < 10; i++) {
		row = set_obj.getrow(&set_obj, end - i - 1);
		printf("%u\t%u\t%lf\n", row->date, row->deal_time,
		       row->deal_price);
	}
	printf("\n");

	printf("(5c) median prices\n");

	int TXO_size = start + (end - start) / 2;
	row = set_obj.getrow(&set_obj, TXO_size);
	printf("%u\t%u\t%lf\n", row->date, row->deal_time, row->deal_price);

	if ((end - start) & 1) {
		row = set_obj.getrow(&set_obj, TXO_size + 1);
		printf("%u\t%u\t%lf\n", row->date, row->deal_time,
		       row->deal_price);
	}
	printf("\n");

	GET_TIME(t + time_idx++);
	set_obj.sort(&set_obj, cmp_p2_time);
	p2 *pre_row, *max_d1, *max_d2, *min_d1, *min_d2;
	double max_ret = -1e19, min_ret = 1e19;

	pre_row = set_obj.getrow(&set_obj, start);
	for (int i = start + 1; i < end; i++) {
		row = set_obj.getrow(&set_obj, i);
		double price_diff = (row->deal_price - pre_row->deal_price) /
				    pre_row->deal_price * 100;
		if (price_diff > max_ret) {
			max_ret = price_diff;
			max_d1 = pre_row;
			max_d2 = row;
		}
		if (price_diff < min_ret) {
			min_ret = price_diff;
			min_d1 = pre_row;
			min_d2 = row;
		}
		pre_row = row;
	}

	printf("(5d) max/min returns\n");
	printf("When...\n");
	printf("\t%u\t%u\t%lf\n", max_d1->date, max_d1->deal_time,
	       max_d1->deal_price);
	printf("\t%u\t%u\t%lf\n", max_d2->date, max_d2->deal_time,
	       max_d2->deal_price);
	printf("\tget max return = %lf%%\n\n", max_ret);

	printf("When...\n");
	printf("\t%u\t%u\t%lf\n", min_d1->date, min_d1->deal_time,
	       min_d1->deal_price);
	printf("\t%u\t%u\t%lf\n", min_d2->date, min_d2->deal_time,
	       min_d2->deal_price);
	printf("\tget min return = %lf%%\n\n", min_ret);
	GET_TIME(t + time_idx++);

	fprintf(stderr, "prepare-dataset-solve-1 %ld\n", TIME_DIFF(t[0], t[1]));
	fprintf(stderr, "solve-2-3-4 %ld\n", TIME_DIFF(t[2], t[3]));
	fprintf(stderr, "solve-5a-5b-5c %ld\n", TIME_DIFF(t[4], t[5]));
	fprintf(stderr, "solve-5a-5b-5c %ld\n", TIME_DIFF(t[5], t[6]));

	ds_obj.destroy(&ds_obj);
	set_obj.destroy(&set_obj);
	return 0;
}
