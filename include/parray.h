#ifndef __PARRAY_H__
#define __PARRAY_H__
#include <stdint.h>
#include <stddef.h>
#include "util.h"

typedef struct parray parray;

struct parray {
	void (*readcsv)(parray *, const char *, sscan);
	void (*create)(parray *, uint32_t);
	void *const (*getrow)(parray *, uint32_t);
	void (*setrow)(parray *, uint32_t, void *);
	void (*showrow)(parray *, uint32_t, print);
	void (*delrow)(parray *, uint32_t);
	parray (*retset)(parray *, cmp);
	uint32_t (*retrows)(parray *);
	void (*sampled)(parray *, uint32_t);
	void (*merge)(parray *, parray *);
	void (*sort)(parray *, cmp);
	void *const (*search)(parray *, void *, cmp);
	void (*destroy)(parray *);
	size_t struct_size;
	uint32_t row;
	uint32_t col;
	void *content;
	void **ptr;
};

parray parray_init(size_t);

#endif
