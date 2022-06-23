#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "array.h"
#include "util.h"
#define BUF_SIZE 512
#define INIT_MAX_ALLOC_SIZE 1024

static void array_readcsv(array *obj, const char *filename, sscan sscan_from);
static void array_create(array *obj, uint32_t num);
static void *const array_getrow(array *obj, uint32_t idx);
static void array_setrow(array *obj, uint32_t idx, void *val);
static void array_showrow(array *obj, uint32_t idx, print print_to);
static void array_delrow(array *obj, uint32_t idx);
static array array_retset(array *obj, cmp compar);
static uint32_t array_retrows(array *obj);
static void array_sampled(array *obj, uint32_t interval);
static void array_merge(array *obj, array *merge_obj);
static void array_sort(array *obj, cmp compar);
static void *const array_search(array *obj, void *target, cmp compar);
static void array_destroy(array *obj);

array array_init(size_t struct_size) {
	array obj = {.readcsv = array_readcsv,
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
		     .struct_size = struct_size,
		     .row = 0,
		     .col = 0,
		     .content = NULL};
	return obj;
}

static void array_readcsv(array *obj, const char *filename, sscan sscan_from) {
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
	int capacity = INIT_MAX_ALLOC_SIZE;

	size_t struct_size = obj->struct_size;
	while (fgets(buf, sizeof(buf), file)) {
		if (obj->row == capacity) {
			capacity <<= 1;
			obj->content =
				realloc(obj->content, struct_size * capacity);
		}
		sscan_from(buf, (char *)obj->content + obj->row * struct_size);
		obj->row++;
	}
	obj->content = realloc(obj->content, struct_size * obj->row);
	fclose(file);
}

static void array_create(array *obj, uint32_t num) {
	obj->content = malloc(obj->struct_size * num);
	obj->row = num;
}

static void *const array_getrow(array *obj, uint32_t idx) {
	return (char *)obj->content + idx * obj->struct_size;
}

static void array_setrow(array *obj, uint32_t idx, void *val) {
	memcpy((char *)obj->content + idx * obj->struct_size, val,
	       obj->struct_size);
}

static void array_showrow(array *obj, uint32_t idx, print print_to) {
	print_to((char *)obj->content + idx * obj->struct_size);
}

static void array_delrow(array *obj, uint32_t idx) {
	memcpy((char *)obj->content + idx * obj->struct_size,
	       (char *)obj->content + (idx + 1) * obj->struct_size,
	       obj->struct_size * (obj->row - idx - 1));
	obj->row--;
}

static array array_retset(array *obj, cmp compar) {
	obj->sort(obj, compar);
	array retobj = array_init(obj->struct_size);
	retobj.content = malloc(obj->struct_size * obj->row);
	retobj.col = obj->col;
	size_t struct_size = obj->struct_size;
	uint32_t newrow = 0;
	int cur_idx = 0;
	if (obj->row != 0) {
		for (int i = 1; i < obj->row; i++) {
			if (compar((char *)obj->content + cur_idx * struct_size,
				   (char *)obj->content + i * struct_size) !=
			    0) {
				memcpy((char *)retobj.content +
					       newrow * struct_size,
				       (char *)obj->content +
					       cur_idx * struct_size,
				       struct_size);
				newrow++;
				cur_idx = i;
			}
		}
		memcpy((char *)retobj.content + newrow * struct_size,
		       (char *)obj->content + cur_idx * struct_size,
		       struct_size);
		newrow++;
	}
	retobj.content = realloc(retobj.content, struct_size * newrow);
	retobj.row = newrow;

	return retobj;
}

static uint32_t array_retrows(array *obj) {
	return obj->row;
}

static void array_sampled(array *obj, uint32_t interval) {
	uint32_t newrow = 0;
	size_t struct_size = obj->struct_size;
	char temp[struct_size];
	for (int i = 0; i < obj->row; i += interval) {
		swap((char *)obj->content + newrow * struct_size,
		     (char *)obj->content + i * struct_size, struct_size);
		newrow++;
	}
	obj->content = realloc(obj->content, struct_size * newrow);
	obj->row = newrow;
}

static void array_merge(array *obj, array *merge_obj) {
	uint32_t newrow = obj->row + merge_obj->row;
	size_t struct_size = obj->struct_size;
	obj->content = realloc(obj->content, struct_size * newrow);
	memcpy((char *)obj->content + obj->row * struct_size,
	       merge_obj->content, struct_size * merge_obj->row);
	obj->row = newrow;
	free(merge_obj->content);
	merge_obj->row = 0;
	merge_obj->content = NULL;
}

static void max_heapify(array *obj, int idx, cmp compar) {
	uint32_t row = obj->row;
	size_t struct_size = obj->struct_size;

	while (true) {
		int lnode = idx << 1;
		int rnode = (idx << 1) + 1;
		int max_idx = idx;
		if (lnode < row) {
			max_idx = compar((char *)obj->content +
						 max_idx * struct_size,
					 (char *)obj->content +
						 lnode * struct_size) < 0
					  ? lnode
					  : max_idx;
		}
		if (rnode < row) {
			max_idx = compar((char *)obj->content +
						 max_idx * struct_size,
					 (char *)obj->content +
						 rnode * struct_size) < 0
					  ? rnode
					  : max_idx;
		}
		if (max_idx != idx) {
			swap((char *)obj->content + max_idx * struct_size,
			     (char *)obj->content + idx * struct_size,
			     struct_size);
			idx = max_idx;
		}
		else
			break;
	}
}

static void array_sort(array *obj, cmp compar) {
#if STD_C_FUNC == 1
	qsort((void *)obj->content, obj->row, obj->struct_size, compar);
#else
	uint32_t row = obj->row;
	size_t struct_size = obj->struct_size;
	for (int i = row >> 1; i >= 0; i--)
		max_heapify(obj, i, compar);

	for (int i = row - 1; i >= 0; i--) {
		swap(obj->content, (char *)obj->content + i * struct_size,
		     struct_size);
		obj->row--;
		max_heapify(obj, 0, compar);
	}
	obj->row = row;
#endif
}

static void *const array_search(array *obj, void *target, cmp compar) {
	return bsearch(target, obj->content, obj->row, obj->struct_size,
		       compar);
}

static void array_destroy(array *obj) {
	free(obj->content);
}
