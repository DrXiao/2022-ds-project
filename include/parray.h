#ifndef __PARRAY_H__
#define __PARRAY_H__
#include <stdint.h>
#include "csv.h"

typedef struct parray parray;

struct parray {
	void (*readcsv)(parray *, char *);
	void (*create)(parray *, uint32_t);
	void * const (*getrow)(parray *, uint32_t);
	void (*setrow)(parray *, uint32_t, void *);
	void (*showrow)(parray *, uint32_t);
	void (*delrow)(parray *, uint32_t);
	parray (*retset)(parray *, cmp);
	uint32_t (*retrows)(parray *);
	void (*sampled)(parray *, uint32_t);
	void (*merge)(parray *, parray*);
	void (*sort)(parray *, cmp);
	void * const (*search)(parray *, void *, cmp);
	void (*destroy)(parray *);
	csv_op __csv_op;
	uint32_t __row;
	uint32_t __col;
	void **__content;
};

parray parray_init(csv_op *);

#endif
