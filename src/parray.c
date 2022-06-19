#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "parray.h"
#include "csv.h"

#define BUF_SIZE 512
#define INIT_MAX_ALLOC_SIZE 1024

static void parray_readcsv(parray *obj, char *filename);
static void parray_create(parray *obj, uint32_t num);
static void * const parray_getrow(parray *obj, uint32_t idx);
static void parray_setrow(parray *obj, uint32_t idx, void *val);
static void parray_showrow(parray *obj, uint32_t idx);
static void parray_delrow(parray *obj, uint32_t idx);
static parray parray_retset(parray *obj, cmp compare);
static uint32_t parray_retrows(parray *obj);
static void parray_sampled(parray *obj, uint32_t interval);
static void parray_merge(parray *obj, parray *merge_obj);
static void parray_sort(parray *obj, cmp compare);
static void * const parray_search(parray *obj, void *target, cmp compare);
static void parray_destroy(parray *obj);

parray parray_init(csv_op *op) {
	parray obj = {
		.readcsv = parray_readcsv,
		.create = parray_create,
		.getrow = parray_getrow,
		.setrow = parray_setrow,
		.showrow = parray_showrow,
		.delrow = parray_delrow,
		.retset = parray_retset,
		.retrows = parray_retrows,
		.sampled = parray_sampled,
		.merge = parray_merge,
		.sort = parray_sort,
		.search = parray_search, 
		.destroy = parray_destroy,
		.__csv_op = *op,
		.__row = 0,
		.__col = 0,
		.__content = NULL
	};
	return obj;
}

static void parray_readcsv(parray *obj, char *filename) {
	char buf[BUF_SIZE];
	char *token = NULL;
	FILE *file = fopen(filename, "r");
	int col = 0;
	fgets(buf, sizeof(buf), file);
	
	token = strtok(buf, CSV_DELIMITER);
	while(token) { 
		col++;
		token = strtok(NULL, CSV_DELIMITER);
	}
	obj->__col = col;
	obj->__content = malloc(sizeof(void *) * INIT_MAX_ALLOC_SIZE);
	int capacity = INIT_MAX_ALLOC_SIZE;
	
	size_t struct_size = obj->__csv_op.struct_size;
	while(fgets(buf, sizeof(buf), file)) {
		if (obj->__row == capacity) {
			capacity <<= 1;
			obj->__content = realloc(obj->__content, sizeof(void *) * capacity);
		}
		obj->__content[obj->__row] = malloc(struct_size);
		obj->__csv_op.sscan_from(buf, obj->__content[obj->__row]);
		obj->__row++;
	}
	obj->__content = realloc(obj->__content,sizeof(void *) * obj->__row);
	fclose(file);
}

static void parray_create(parray *obj, uint32_t num) {
	obj->__content = malloc(sizeof(void *) * num);
	for (int i = 0; i < num; i++)
		obj->__content[i] = malloc(obj->__csv_op.struct_size);
	obj->__row = num;
}

static void * const parray_getrow(parray *obj, uint32_t idx) {
	return obj->__content[idx];
}

static void parray_setrow(parray *obj, uint32_t idx, void *val) {
	memcpy(obj->__content[idx], val, obj->__csv_op.struct_size);
}

static void parray_showrow(parray *obj, uint32_t idx) {
	obj->__csv_op.print_to(obj->__content[idx]);
}

static void parray_delrow(parray *obj, uint32_t idx) {
	free(obj->__content[idx]);
	memcpy(obj->__content + idx, obj->__content + idx + 1, 
		sizeof(void *) * (obj->__row - idx - 1));
	obj->__row--;
}

static parray parray_retset(parray *obj, cmp compare) {
	obj->sort(obj, compare);
	parray retobj = parray_init(&obj->__csv_op);
	retobj.__content = malloc(sizeof(void *) * obj->__row);
	retobj.__col = obj->__col;
	size_t struct_size = obj->__csv_op.struct_size;
	uint32_t newrow = 0;
	int cur_idx = 0;
	if (obj->__row != 0) {
		for (int i = 1; i < obj->__row; i++) {
			if (compare(obj->__content + cur_idx, obj->__content + i) != 0) {
				retobj.__content[newrow] = malloc(struct_size);
				memcpy(retobj.__content[newrow], obj->__content[cur_idx], struct_size);
				newrow++;
				cur_idx = i;
			}
		}
		retobj.__content[newrow] = malloc(struct_size);
		memcpy(retobj.__content[newrow], obj->__content[cur_idx], struct_size);
		newrow++;
	}
	retobj.__content = realloc(retobj.__content, sizeof(void *) * newrow);
	retobj.__row = newrow;

	return retobj;
}

static uint32_t parray_retrows(parray *obj) {
	return obj->__row;
}

static void parray_sampled(parray *obj, uint32_t interval) {
	uint32_t newrow = 0;
	for (int i = 0; i < obj->__row; i += interval) {
		void *temp = obj->__content[newrow];
		obj->__content[newrow] = obj->__content[i];
		obj->__content[i] = temp;
		newrow++;
	}
	for (int i = newrow; i < obj->__row; i++) {
		free(obj->__content[i]);
	}
	obj->__content = realloc(obj->__content, sizeof(void *) * newrow);
	obj->__row = newrow;
}

static void parray_merge(parray *obj, parray *merge_obj) {
	uint32_t newrow = obj->__row + merge_obj->__row;
	obj->__content = realloc(obj->__content, sizeof(void *) * newrow);
	memcpy(obj->__content + obj->__row, merge_obj->__content, sizeof(void *) * merge_obj->__row);
	obj->__row = newrow;
	free(merge_obj->__content);
	merge_obj->__row = 0;
	merge_obj->__content = NULL;
}

static void parray_sort(parray *obj, cmp compare) {
#if STD_C_QSORT == 1
	qsort((void *)obj->__content, obj->__row, 
		sizeof(void *), compare);
#else

#endif
}

static void * const parray_search(parray *obj, void *target, cmp compare) {
	void **ptarget = &target;
	return bsearch(ptarget, obj->__content, obj->__row, sizeof(void *), compare);
}

static void parray_destroy(parray *obj) {
	for (int i = 0; i < obj->__row; i++) {
		free(obj->__content[i]);
	}
	free(obj->__content);
}

