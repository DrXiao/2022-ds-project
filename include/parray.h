#ifndef __PARRAY_H__
#define __PARRAY_H__
#include <stdint.h>
#include "csv.h"

typedef struct parray parray;

struct parray {
	void (*readcsv)(parray *, char *);
	void (*create)(parray *, uint32_t);
	void *const (*getrow)(parray *, uint32_t);
	void (*setrow)(parray *, uint32_t, void *);
	void (*showrow)(parray *, uint32_t);
	void (*delrow)(parray *, uint32_t);
	parray (*retset)(parray *, cmp);
	uint32_t (*retrows)(parray *);
	void (*sampled)(parray *, uint32_t);
	void (*merge)(parray *, parray *);
	void (*sort)(parray *, cmp);
	void *const (*search)(parray *, void *, cmp);
	void (*destroy)(parray *);
	csv_op csvop;
	uint32_t row;
	uint32_t col;
	void **content;
};

parray parray_init(csv_op *);

#endif
