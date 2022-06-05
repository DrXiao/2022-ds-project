#ifndef __CSV_H__
#define __CSV_H__
#include <stdint.h>

typedef struct csv csv;

struct csv {
	void (*readcsv)(csv *, char *);
	void (*getrow)(csv *, uint32_t);

	void (*sort)(csv *);
	void (*destroy)(csv *);
	void (*__readcsv)(csv *, FILE *);
	void (*__cmp)(const void *, const void *);
	uint32_t __row;
	uint32_t __col;
	void **__content;
};

csv csv_init(uint32_t);

#endif
