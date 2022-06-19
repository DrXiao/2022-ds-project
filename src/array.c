#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "array.h"
#include "csv.h"
#define BUF_SIZE 512
#define INIT_MAX_ALLOC_SIZE 1024

static void array_readcsv(array *obj, char *filename);
static void array_create(array *obj, uint32_t num);
static void * const array_getrow(array *obj, uint32_t idx);
static void array_setrow(array *obj, uint32_t idx, void *val);
static void array_showrow(array *obj, uint32_t idx);
static void array_delrow(array *obj, uint32_t idx);
static array array_retset(array *obj, cmp compare);
static uint32_t array_retrows(array *obj);
static void array_sampled(array *obj, uint32_t interval);
static void array_merge(array *obj, array *merge_obj);
static void array_sort(array *obj, cmp compare);
static void * const array_search(array *obj, void *target, cmp compare);
static void array_destroy(array *obj);

array array_init(csv_op *op) {
	array obj = {
		.readcsv = array_readcsv,
		.create = array_create,
		.getrow = array_getrow,
		.setrow = array_setrow,
		.showrow = array_showrow,
		.delrow = array_delrow,
		.retset = array_retset,
		.retrows = array_retrows,
		.sampled = array_sampled,
		.merge = array_merge,
		.sort = array_sort,
		.search = array_search, 
		.destroy = array_destroy,
		.__csv_op = *op,
		.__row = 0,
		.__col = 0,
		.__content = NULL
	};
	return obj;
}

static void array_readcsv(array *obj, char *filename) {
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
	obj->__content = malloc(obj->__csv_op.struct_size * INIT_MAX_ALLOC_SIZE);
	int capacity = INIT_MAX_ALLOC_SIZE;
	
	size_t struct_size = obj->__csv_op.struct_size;
	while(fgets(buf, sizeof(buf), file)) {
		if (obj->__row == capacity) {
			capacity <<= 1;
			obj->__content = realloc(obj->__content, struct_size * capacity);
		}
		obj->__csv_op.sscan_from(buf, (char *)obj->__content + obj->__row * struct_size);
		obj->__row++;
	}
	obj->__content = realloc(obj->__content, struct_size * obj->__row);
	fclose(file);
}

static void array_create(array *obj, uint32_t num) {
	obj->__content = malloc(obj->__csv_op.struct_size * num);
	obj->__row = num;
}

static void * const array_getrow(array *obj, uint32_t idx) {
	return (char *)obj->__content + idx * obj->__csv_op.struct_size;
}

static void array_setrow(array *obj, uint32_t idx, void *val) {
	memcpy((char *)obj->__content + idx * obj->__csv_op.struct_size, val, obj->__csv_op.struct_size);
}

static void array_showrow(array *obj, uint32_t idx) {
	obj->__csv_op.print_to((char *)obj->__content + idx * obj->__csv_op.struct_size);
}

static void array_delrow(array *obj, uint32_t idx) {
	memcpy((char *)obj->__content + idx * obj->__csv_op.struct_size, 
	       (char *)obj->__content + (idx + 1) * obj->__csv_op.struct_size, 
	       obj->__csv_op.struct_size * (obj->__row - idx - 1));
	obj->__row--;
}

static array array_retset(array *obj, cmp compare) {
	obj->sort(obj, compare);
	array retobj = array_init(&obj->__csv_op);
	retobj.__content = malloc(obj->__csv_op.struct_size * obj->__row);
	retobj.__col = obj->__col;
	size_t struct_size = obj->__csv_op.struct_size;
	uint32_t newrow = 0;
	int cur_idx = 0;
	if (obj->__row != 0) {
		for (int i = 1; i < obj->__row; i++) {
			if (compare((char *)obj->__content + cur_idx, (char *)obj->__content + i) != 0) {
				memcpy((char *)retobj.__content + newrow * struct_size, (char *)obj->__content + cur_idx * struct_size, struct_size);
				newrow++;
				cur_idx = i;
			}
		}
		memcpy((char *)retobj.__content + newrow * struct_size, (char *)obj->__content + cur_idx * struct_size, struct_size);
		newrow++;
	}
	retobj.__content = realloc(retobj.__content, struct_size * newrow);
	retobj.__row = newrow;

	return retobj;
}

static uint32_t array_retrows(array *obj) {
	return obj->__row;
}

static void array_sampled(array *obj, uint32_t interval) {
	uint32_t newrow = 0;
	size_t struct_size = obj->__csv_op.struct_size;
	char temp[struct_size];
	for (int i = 0; i < obj->__row; i += interval) {
		memcpy(temp, (char *)obj->__content + newrow * struct_size, struct_size);
		memcpy((char *)obj->__content + newrow * struct_size, (char *)obj->__content + i * struct_size, struct_size);
		memcpy((char *)obj->__content + i * struct_size, temp, struct_size);
		newrow++;
	}
	obj->__content = realloc(obj->__content, struct_size * newrow);
	obj->__row = newrow;
}

static void array_merge(array *obj, array *merge_obj) {
	uint32_t newrow = obj->__row + merge_obj->__row;
	size_t struct_size = obj->__csv_op.struct_size;
	obj->__content = realloc(obj->__content, struct_size * newrow);
	memcpy((char *)obj->__content + obj->__row * struct_size, merge_obj->__content, struct_size * merge_obj->__row);
	obj->__row = newrow;
	free(merge_obj->__content);
	merge_obj->__row = 0;
	merge_obj->__content = NULL;
}

static void array_sort(array *obj, cmp compare) {
#if STD_C_QSORT == 1
	qsort((void *)obj->__content, obj->__row, 
		obj->__csv_op.struct_size, compare);
#else

#endif
}

static void * const array_search(array *obj, void *target, cmp compare) {
	return bsearch(target, obj->__content, obj->__row, obj->__csv_op.struct_size, compare);
}

static void array_destroy(array *obj) {
	free(obj->__content);
}

