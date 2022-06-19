#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "parray.h"
#define P2_FORMAT_IN  "%u , %s , %s , %s , %s , %u , %lf , %u , %*u"
#define P2_FORMAT_OUT "%u, %s, %s, %s, %s, %u, %lf, %u"
#define P2_FIELDS_IN(p2) &p2.date, p2.item_id, p2.item_price, p2.deadline, p2.auth, &p2.deal_time, &p2.deal_price, &p2.dead_num
#define P2_FIELDS_OUT(p2) p2.date, p2.item_id, p2.item_price, p2.deadline, p2.auth, p2.deal_time, p2.deal_price, p2.dead_num

typedef struct p2_parray {
	uint32_t date;
	char item_id[4];
	char item_price[16];
	char deadline[16];
	char auth[4];
	uint32_t deal_time;
	double deal_price;
	uint32_t dead_num;
} p2_parray;


static int sscan_p2(const char *buf, void *struct_obj) {
	p2_parray *p2 = struct_obj;
	return sscanf(buf, P2_FORMAT_IN, P2_FIELDS_IN((*p2)));
}

static void print_p2(void *struct_obj) {
	p2_parray *p2 = struct_obj;
	printf(P2_FORMAT_OUT"\n", P2_FIELDS_OUT((*p2)));
}

static int cmp_product(const void *p1, const void *p2) {
	p2_parray **pp1 = p1, **pp2 = p2;
	char product1[32] = "", product2[32] = "";
	strcat(product1, (*pp1)->item_id);
	strcat(product1, (*pp1)->item_price);
	strcat(product1, (*pp1)->deadline);
	strcat(product1, (*pp1)->auth);

	strcat(product2, (*pp2)->item_id);
	strcat(product2, (*pp2)->item_price);
	strcat(product2, (*pp2)->deadline);
	strcat(product2, (*pp2)->auth);
	return strcmp(product1, product2);
}


int main(int argc, char **argv) {
	
	if (argc < 2) {
		fprintf(stderr, "Usage:\n"
				"./p1 parray_file1 [parray_file2 [parray_file3...]]\n");
		exit(1);
	}

	csv_op csvop = csv_op_init(sscan_p2, print_p2, sizeof(p2_parray));
	parray parray_obj = parray_init(&csvop);

	for (int i = 0; i < 5; i++) {
		char *parrayfile = argv[1 + i];

		parray obj = parray_init(&csvop);
		obj.readcsv(&obj, parrayfile);
		parray_obj.merge(&parray_obj, &obj);
		obj.destroy(&obj);
	}

	parray set_obj = parray_obj.retset(&parray_obj, cmp_product);
	parray_obj.destroy(&parray_obj);

	printf("(1) total products: %u\n", set_obj.retrows(&set_obj));
	
	p2_parray target = (p2_parray){.item_id = "TXO",
				 .item_price = "1000",
				 .deadline = "201706",
				 .auth = "P"};

	void *ret = set_obj.search(&set_obj, &target, cmp_product);
	printf("(2) Does TXO_1000_201706_P exist : %d\n", (ret != NULL));

	target = (p2_parray){.item_id = "TXO",
			  .item_price = "9500",
			  .deadline = "201706",
			  .auth = "C"};

	ret = set_obj.search(&set_obj, &target, cmp_product);
	printf("(3) Does TXO_9500_201706_C exist : %d\n", (ret != NULL));
	
	target = (p2_parray){.item_id = "GIO",
			  .item_price = "5500",
			  .deadline = "201706",
			  .auth = "C"};

	ret = set_obj.search(&set_obj, &target, cmp_product);

	printf("(4) Does GIO_5500_201706_C exist : %d\n", (ret != NULL));

	set_obj.destroy(&set_obj);
	return 0;
}
