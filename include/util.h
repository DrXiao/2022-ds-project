#ifndef __UTIL_H__
#define __UTIL_H__
#include <stddef.h>
#include <time.h>
#define CSV_DELIMITER ","

#define GET_TIME(t) clock_gettime(CLOCK_MONOTONIC, t);
#define TIME_DIFF(t1, t2)                                                      \
	(t2.tv_sec - t1.tv_sec) * (int)1e9 - t1.tv_nsec + t2.tv_nsec

typedef struct csv_op csv_op;
typedef int (*sscan)(const char *, void *);
typedef void (*print)(void *);
typedef int (*cmp)(const void *, const void *);

void swap(void *obj1, void *obj2, size_t size);

#endif
