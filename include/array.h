#ifndef __ARRAY_H__
#define __ARRAY_H__
#include <stdint.h>
#include "csv.h"

typedef struct array array;

struct array {
	void (*readcsv)(array *, char *);
	void (*create)(array *, uint32_t);
	void * const (*getrow)(array *, uint32_t);
	void (*setrow)(array *, uint32_t, void *);
	void (*showrow)(array *, uint32_t);
	void (*delrow)(array *, uint32_t);
	array (*retset)(array *, cmp);
	uint32_t (*retrows)(array *);
	void (*sampled)(array *, uint32_t);
	void (*merge)(array *, array*);
	void (*sort)(array *, cmp);
	void * const (*search)(array *, void *, cmp);
	void (*destroy)(array *);
	csv_op __csv_op;
	uint32_t __row;
	uint32_t __col;
	void *__content;
};

array array_init(csv_op *);

#endif
