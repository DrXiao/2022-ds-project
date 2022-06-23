#include <stdio.h>
#include <stdlib.h>
#include "rbtree.h"

int cmp_int(const void *p1, const void *p2) {
	int *int1 = (int *)p1, *int2 = (int *)p2;
	return *int1 - *int2;
}

void print_int(void *data) {
	int *val = (int *)data;
	printf(" %d", *val);
}

int main(void) {

	rbtree ds_obj = rbtree_init(sizeof(int), cmp_int);

	int val = 0;

	while (scanf("%d\n", &val) != EOF) {
		ds_obj.insert(&ds_obj, &val);
	}
	printf("result: \n");
	ds_obj.preorder(&ds_obj, print_int);
	printf("\n");
	ds_obj.inorder(&ds_obj, print_int);

	ds_obj.destroy(&ds_obj);

	return 0;
}
