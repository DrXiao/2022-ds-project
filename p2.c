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
		&p2.deal_time, &p2.deal_price, &p2.dead_num
#define P2_FIELDS_OUT(p2)                                                      \
	p2.date, p2.item_id, p2.item_price, p2.deadline, p2.auth,              \
		p2.deal_time, p2.deal_price, p2.dead_num

typedef struct p2 {
	uint32_t date;
	char item_id[4];
	char item_price[16];
	char deadline[16];
	char auth[4];
	uint32_t deal_time;
	double deal_price;
	uint32_t dead_num;
} p2;

static int sscan_p2(const char *buf, void *struct_obj) {
	p2 *p2 = struct_obj;
	return sscanf(buf, P2_FORMAT_IN, P2_FIELDS_IN((*p2)));
}

static void print_p2(void *struct_obj) {
	p2 *p2 = struct_obj;
	printf(P2_FORMAT_OUT "\n", P2_FIELDS_OUT((*p2)));
}

static int cmp_product(const void *product1, const void *product2) {
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
	array set_obj = ds_obj.retset(&ds_obj, cmp_product);
#elif DS == 1
	parray set_obj = ds_obj.retset(&ds_obj, cmp_product);
#endif
	GET_TIME(t + time_idx++);
	ds_obj.destroy(&ds_obj);

	printf("(1) total products: %u\n", set_obj.retrows(&set_obj));

	p2 target = (p2){.item_id = "TXO",
			 .item_price = "1000",
			 .deadline = "201706",
			 .auth = "P"};

	GET_TIME(t + time_idx++);
	void *ret = set_obj.search(&set_obj, &target, cmp_product);
	printf("(2) Does TXO_1000_201706_P exist : %d\n", (ret != NULL));

	target = (p2){.item_id = "TXO",
		      .item_price = "9500",
		      .deadline = "201706",
		      .auth = "C"};

	ret = set_obj.search(&set_obj, &target, cmp_product);
	printf("(3) Does TXO_9500_201706_C exist : %d\n", (ret != NULL));

	target = (p2){.item_id = "GIO",
		      .item_price = "5500",
		      .deadline = "201706",
		      .auth = "C"};

	ret = set_obj.search(&set_obj, &target, cmp_product);

	printf("(4) Does GIO_5500_201706_C exist : %d\n", (ret != NULL));

	GET_TIME(t + time_idx++);

	fprintf(stderr, "prepare-dataset-solve-1 %ld\n", TIME_DIFF(t[0], t[1]));
	fprintf(stderr, "solve-2-3-4 %ld\n", TIME_DIFF(t[2], t[3]));

	set_obj.destroy(&set_obj);
	return 0;
}
