#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "csv.h"
#define CSV_DELIMITER ","
#define BUF_SIZE 512
#define MAX_ALLOC_SIZE 320000

static void csv_read(csv *obj, char *filename);
static void * const csv_getrow(csv *obj, uint32_t idx);
static void csv_showrow(csv *obj, uint32_t idx);
static void csv_delrow(csv *obj, uint32_t idx);
static csv csv_retset(csv *obj, cmp compare);
static void csv_sort(csv *obj, cmp compare);
static void csv_destroy(csv *obj);

csv_op csv_op_init(sscan sscan_op, print print_op, size_t struct_size) {
	csv_op csvop = {
		.sscan_from = sscan_op,
		.print_to = print_op,
		.struct_size = struct_size,
	};
	return csvop;
}

csv csv_init(csv_op *op) {
	csv obj = {
		.readcsv = csv_read,
		.getrow = csv_getrow,
		.showrow = csv_showrow,
		.delrow = csv_delrow,
		.retset = csv_retset,
		.sort = csv_sort,
		.destroy = csv_destroy,
		.__csv_op = *op,
		.__row = 0,
		.__col = 0,
		.__content = NULL
	};
	return obj;
}

static void csv_read(csv *obj, char *filename) {
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
	obj->__content = malloc(sizeof(void *) * MAX_ALLOC_SIZE);
	
	size_t struct_size = obj->__csv_op.struct_size;
	while(fgets(buf, sizeof(buf), file)) {
		obj->__content[obj->__row] = malloc(struct_size);
		obj->__csv_op.sscan_from(buf, obj->__content[obj->__row]);
		obj->__row++;
	}
	obj->__content = realloc(obj->__content,sizeof(void *) * obj->__row);
	fclose(file);
}

static void * const csv_getrow(csv *obj, uint32_t idx) {
	return obj->__content[idx];
}

static void csv_showrow(csv *obj, uint32_t idx) {
	obj->__csv_op.print_to(obj->__content[idx]);
}

static void csv_delrow(csv *obj, uint32_t idx) {
	free(obj->__content[idx]);
	memcpy(obj->__content + idx, obj->__content + idx + 1, 
		sizeof(void *) * (obj->__row - idx - 1));
	obj->__row--;
}

static csv csv_retset(csv *obj, cmp compare) {
	obj->sort(obj, compare);
	csv retobj = csv_init(&obj->__csv_op);
	retobj.__content = malloc(sizeof(void *) * MAX_ALLOC_SIZE);
	retobj.__col = obj->__col;
	size_t struct_size = obj->__csv_op.struct_size;
	int newrow = 0;
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

static void csv_sort(csv *obj, cmp compare) {
#if STD_C_QSORT == 1
	qsort((void *)obj->__content, obj->__row, 
		sizeof(void *), compare);
#else

#endif
}


static void csv_destroy(csv *obj) {
	for (int i = 0; i < obj->__row; i++) {
		free(obj->__content[i]);
	}
	free(obj->__content);
}

