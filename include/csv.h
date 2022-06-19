#ifndef __CSV_H__
#define __CSV_H__
#include <stddef.h>
#define CSV_DELIMITER ","

typedef struct csv_op csv_op;
typedef int (*sscan)(const char *, void *);
typedef void (*print)(void *);
typedef int (*cmp)(const void *, const void *);

struct csv_op {
	sscan sscan_from;
	print print_to;
	size_t struct_size;
};

csv_op csv_op_init(sscan, print, size_t);

#endif
