#ifndef __CSV_H__
#define __CSV_H__
#include <stdint.h>

typedef struct csv csv;
typedef struct csv_op csv_op;
typedef int (*sscan)(const char *, void *);
typedef void (*print)(void *);
typedef int (*cmp)(const void *, const void *);

struct csv_op {
	sscan sscan_from;
	print print_to;
	cmp compare;
	size_t struct_size;
};

struct csv {
	void (*readcsv)(csv *, char *);
	void * const (*getrow)(csv *, uint32_t);
	void (*showrow)(csv *, uint32_t);
	void (*sort)(csv *);
	void (*destroy)(csv *);
	csv_op __csv_op;
	uint32_t __row;
	uint32_t __col;
	void **__content;
};

csv_op csv_op_init(sscan, print, size_t);
csv csv_init(csv_op *);


#endif
