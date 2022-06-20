#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "parray.h"
#include "csv.h"
#include "util.h"
#define BUF_SIZE 512
#define INIT_MAX_ALLOC_SIZE 1024

static void parray_readcsv(parray *obj, char *filename);
static void parray_create(parray *obj, uint32_t num);
static void *const parray_getrow(parray *obj, uint32_t idx);
static void parray_setrow(parray *obj, uint32_t idx, void *val);
static void parray_showrow(parray *obj, uint32_t idx);
static void parray_delrow(parray *obj, uint32_t idx);
static parray parray_retset(parray *obj, cmp compar);
static uint32_t parray_retrows(parray *obj);
static void parray_sampled(parray *obj, uint32_t interval);
static void parray_merge(parray *obj, parray *merge_obj);
static void parray_sort(parray *obj, cmp compar);
static void *const parray_search(parray *obj, void *target, cmp compar);
static void parray_destroy(parray *obj);

parray parray_init(csv_op *op) {
	parray obj = {.readcsv = parray_readcsv,
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
		      .csvop = *op,
		      .row = 0,
		      .col = 0,
		      .content = NULL};
	return obj;
}

static void parray_readcsv(parray *obj, char *filename) {
	char buf[BUF_SIZE];
	char *token = NULL;
	FILE *file = fopen(filename, "r");
	int col = 0;
	fgets(buf, sizeof(buf), file);

	token = strtok(buf, CSV_DELIMITER);
	while (token) {
		col++;
		token = strtok(NULL, CSV_DELIMITER);
	}
	obj->col = col;
	obj->content = malloc(sizeof(void *) * INIT_MAX_ALLOC_SIZE);
	int capacity = INIT_MAX_ALLOC_SIZE;

	size_t struct_size = obj->csvop.struct_size;
	while (fgets(buf, sizeof(buf), file)) {
		if (obj->row == capacity) {
			capacity <<= 1;
			obj->content = realloc(obj->content,
						 sizeof(void *) * capacity);
		}
		obj->content[obj->row] = malloc(struct_size);
		obj->csvop.sscan_from(buf, obj->content[obj->row]);
		obj->row++;
	}
	obj->content = realloc(obj->content, sizeof(void *) * obj->row);
	fclose(file);
}

static void parray_create(parray *obj, uint32_t num) {
	obj->content = malloc(sizeof(void *) * num);
	for (int i = 0; i < num; i++)
		obj->content[i] = malloc(obj->csvop.struct_size);
	obj->row = num;
}

static void *const parray_getrow(parray *obj, uint32_t idx) {
	return obj->content[idx];
}

static void parray_setrow(parray *obj, uint32_t idx, void *val) {
	memcpy(obj->content[idx], val, obj->csvop.struct_size);
}

static void parray_showrow(parray *obj, uint32_t idx) {
	obj->csvop.print_to(obj->content[idx]);
}

static void parray_delrow(parray *obj, uint32_t idx) {
	free(obj->content[idx]);
	memcpy(obj->content + idx, obj->content + idx + 1,
	       sizeof(void *) * (obj->row - idx - 1));
	obj->row--;
}

static parray parray_retset(parray *obj, cmp compar) {
	obj->sort(obj, compar);
	parray retobj = parray_init(&obj->csvop);
	retobj.content = malloc(sizeof(void *) * obj->row);
	retobj.col = obj->col;
	size_t struct_size = obj->csvop.struct_size;
	uint32_t newrow = 0;
	int cur_idx = 0;
	if (obj->row != 0) {
		for (int i = 1; i < obj->row; i++) {
			if (compar(obj->content + cur_idx,
				    obj->content + i) != 0) {
				retobj.content[newrow] = malloc(struct_size);
				memcpy(retobj.content[newrow],
				       obj->content[cur_idx], struct_size);
				newrow++;
				cur_idx = i;
			}
		}
		retobj.content[newrow] = malloc(struct_size);
		memcpy(retobj.content[newrow], obj->content[cur_idx],
		       struct_size);
		newrow++;
	}
	retobj.content = realloc(retobj.content, sizeof(void *) * newrow);
	retobj.row = newrow;

	return retobj;
}

static uint32_t parray_retrows(parray *obj) {
	return obj->row;
}

static void parray_sampled(parray *obj, uint32_t interval) {
	uint32_t newrow = 0;
	for (int i = 0; i < obj->row; i += interval) {
		swap(obj->content + newrow, obj->content + i,
		     sizeof(void *));
		newrow++;
	}
	for (int i = newrow; i < obj->row; i++) {
		free(obj->content[i]);
	}
	obj->content = realloc(obj->content, sizeof(void *) * newrow);
	obj->row = newrow;
}

static void parray_merge(parray *obj, parray *merge_obj) {
	uint32_t newrow = obj->row + merge_obj->row;
	obj->content = realloc(obj->content, sizeof(void *) * newrow);
	memcpy(obj->content + obj->row, merge_obj->content,
	       sizeof(void *) * merge_obj->row);
	obj->row = newrow;
	free(merge_obj->content);
	merge_obj->row = 0;
	merge_obj->content = NULL;
}

static void max_heapify(parray *obj, int idx, cmp compar) {
	uint32_t row = obj->row;
	while (true) {
		int lnode = idx << 1;
		int rnode = (idx << 1) + 1;
		int max_idx = idx;
		if (lnode < row) {
			max_idx = compar(obj->content + max_idx,
					  obj->content + lnode) < 0
					  ? lnode
					  : max_idx;
		}
		if (rnode < row) {
			max_idx = compar(obj->content + max_idx,
					  obj->content + rnode) < 0
					  ? rnode
					  : max_idx;
		}

		if (max_idx != idx) {
			swap(obj->content + max_idx, obj->content + idx,
			     sizeof(void *));
			idx = max_idx;
		}
		else
			break;
	}
}

static void parray_sort(parray *obj, cmp compar) {
#if STD_C_FUNC == 1
	qsort((void *)obj->content, obj->row, sizeof(void *), compar);
#else
	uint32_t row = obj->row;
	for (int i = row >> 1; i >= 0; i--)
		max_heapify(obj, i, compar);

	for (int i = row - 1; i >= 0; i--) {
		swap(obj->content, obj->content + i, sizeof(void *));
		obj->row--;
		max_heapify(obj, 0, compar);
	}
	obj->row = row;
#endif
}

static void *const parray_search(parray *obj, void *target, cmp compar) {
	void **ptarget = &target;
	return bsearch(ptarget, obj->content, obj->row, sizeof(void *),
		       compar);
}

static void parray_destroy(parray *obj) {
	for (int i = 0; i < obj->row; i++) {
		free(obj->content[i]);
	}
	free(obj->content);
}
