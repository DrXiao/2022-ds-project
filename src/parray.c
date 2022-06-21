#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "parray.h"
#include "util.h"
#define BUF_SIZE 512
#define INIT_MAX_ALLOC_SIZE 1024

static void parray_readcsv(parray *obj, const char *filename, sscan sscan_from);
static void parray_create(parray *obj, uint32_t num);
static void *const parray_getrow(parray *obj, uint32_t idx);
static void parray_setrow(parray *obj, uint32_t idx, void *val);
static void parray_showrow(parray *obj, uint32_t idx, print print_to);
static void parray_delrow(parray *obj, uint32_t idx);
static parray parray_retset(parray *obj, cmp compar);
static uint32_t parray_retrows(parray *obj);
static void parray_sampled(parray *obj, uint32_t interval);
static void parray_merge(parray *obj, parray *merge_obj);
static void parray_sort(parray *obj, cmp compar);
static void *const parray_search(parray *obj, void *target, cmp compar);
static void parray_destroy(parray *obj);

parray parray_init(size_t struct_size) {
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
		      .struct_size = struct_size,
		      .row = 0,
		      .col = 0,
		      .content = NULL,
		      .ptr = NULL};
	return obj;
}

static void parray_readcsv(parray *obj, const char *filename, sscan sscan_from) {
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
	obj->content = malloc(obj->struct_size * INIT_MAX_ALLOC_SIZE);
	obj->ptr = malloc(sizeof(void *) * INIT_MAX_ALLOC_SIZE);
	int capacity = INIT_MAX_ALLOC_SIZE;

	size_t struct_size = obj->struct_size;
	while (fgets(buf, sizeof(buf), file)) {
		if (obj->row == capacity) {
			capacity <<= 1;
			obj->content = realloc(obj->content, struct_size * capacity);
			obj->ptr = realloc(obj->ptr, sizeof(void *) * capacity);
		}
		sscan_from(buf, (char *)obj->content + obj->row * struct_size);
		obj->row++;
	}
	obj->content = realloc(obj->content, struct_size * obj->row);
	obj->ptr = realloc(obj->ptr, sizeof(void *) * obj->row);
	for (int i = 0; i < obj->row; i++)
		obj->ptr[i] = (void *)((char *)obj->content + i * struct_size);
	fclose(file);
}

static void parray_create(parray *obj, uint32_t num) {
	obj->content = malloc(obj->struct_size * num);
	obj->ptr = malloc(sizeof(void *) * num);
	size_t struct_size = obj->struct_size;
	for (int i = 0; i < num; i++)
		obj->ptr[i] = (char *)obj->content + i * struct_size;
	obj->row = num;
}

static void *const parray_getrow(parray *obj, uint32_t idx) {
	return obj->ptr[idx];
}

static void parray_setrow(parray *obj, uint32_t idx, void *val) {
	memcpy(obj->ptr[idx], val, obj->struct_size);
}

static void parray_showrow(parray *obj, uint32_t idx, print print_to) {
	print_to(obj->ptr[idx]);
}

static void parray_delrow(parray *obj, uint32_t idx) {
	memcpy((char *)obj->content + idx * obj->struct_size,
	       (char *)obj->content + (idx + 1) * obj->struct_size,
	       obj->struct_size * (obj->row - idx - 1));
	memcpy(obj->ptr + idx, obj->ptr + idx + 1,
	       sizeof(void *) * (obj->row - idx - 1));
	obj->row--;
}

static parray parray_retset(parray *obj, cmp compar) {
	obj->sort(obj, compar);
	parray retobj = parray_init(obj->struct_size);
	retobj.content = malloc(obj->struct_size * obj->row);
	retobj.ptr = malloc(sizeof(void *) * obj->row);
	retobj.col = obj->col;
	size_t struct_size = obj->struct_size;
	uint32_t newrow = 0;
	int cur_idx = 0;
	if (obj->row != 0) {
		for (int i = 1; i < obj->row; i++) {
			if (compar(obj->ptr + cur_idx,
				    obj->ptr + i) != 0) {
				retobj.ptr[newrow] = (char *)retobj.content + newrow * struct_size;
				memcpy(retobj.ptr[newrow],
				       obj->ptr[cur_idx], struct_size);
				newrow++;
				cur_idx = i;
			}
		}
		retobj.ptr[newrow] = (char *)retobj.content + newrow * struct_size;
		memcpy(retobj.ptr[newrow], obj->ptr[cur_idx],
		       struct_size);
		newrow++;
	}
	retobj.content = realloc(retobj.content, struct_size * newrow);
	retobj.ptr = realloc(retobj.ptr, sizeof(void *) * newrow);
	retobj.row = newrow;

	return retobj;
}

static uint32_t parray_retrows(parray *obj) {
	return obj->row;
}

static void parray_sampled(parray *obj, uint32_t interval) {
	uint32_t newrow = 0;
	size_t struct_size = obj->struct_size;
	for (int i = 0; i < obj->row; i += interval) {
		swap(obj->ptr + newrow, obj->ptr + i,
		     sizeof(void *));
		swap((char *)obj->content + newrow * struct_size,
		     (char *)obj->content + i * struct_size, struct_size); 
		newrow++;
	}
	obj->content = realloc(obj->content, struct_size * newrow);
	obj->ptr = realloc(obj->ptr, sizeof(void *) * newrow);
	obj->row = newrow;
}

static void parray_merge(parray *obj, parray *merge_obj) {
	uint32_t newrow = obj->row + merge_obj->row;
	size_t struct_size = obj->struct_size;
	obj->content = realloc(obj->content, struct_size * newrow);
	obj->ptr = realloc(obj->ptr, sizeof(void *) * newrow);
	memcpy((char *)obj->content + obj->row * struct_size,
	       (char *)merge_obj->content, struct_size * merge_obj->row);
	for (int i = 0; i < newrow; i++) {
		obj->ptr[i] = (char *)obj->content + i * struct_size;
	}
	obj->row = newrow;
	free(merge_obj->content);
	free(merge_obj->ptr);
	merge_obj->row = 0;
	merge_obj->content = NULL;
	merge_obj->ptr = NULL;
}

static void max_heapify(parray *obj, int idx, cmp compar) {
	uint32_t row = obj->row;
	while (true) {
		int lnode = idx << 1;
		int rnode = (idx << 1) + 1;
		int max_idx = idx;
		if (lnode < row) {
			max_idx = compar(obj->ptr + max_idx,
					  obj->ptr + lnode) < 0
					  ? lnode
					  : max_idx;
		}
		if (rnode < row) {
			max_idx = compar(obj->ptr + max_idx,
					  obj->ptr + rnode) < 0
					  ? rnode
					  : max_idx;
		}

		if (max_idx != idx) {
			swap(obj->ptr + max_idx, obj->ptr + idx,
			     sizeof(void *));
			idx = max_idx;
		}
		else
			break;
	}
}

static void parray_sort(parray *obj, cmp compar) {
#if STD_C_FUNC == 1
	qsort((void *)obj->ptr, obj->row, sizeof(void *), compar);
#else
	uint32_t row = obj->row;
	for (int i = row >> 1; i >= 0; i--)
		max_heapify(obj, i, compar);

	for (int i = row - 1; i >= 0; i--) {
		swap(obj->ptr, obj->ptr + i, sizeof(void *));
		obj->row--;
		max_heapify(obj, 0, compar);
	}
	obj->row = row;
#endif
}

static void *const parray_search(parray *obj, void *target, cmp compar) {
	void **ptarget = &target;
	return bsearch(ptarget, obj->ptr, obj->row, sizeof(void *),
		       compar);
}

static void parray_destroy(parray *obj) {
	free(obj->content);
	free(obj->ptr);
}
