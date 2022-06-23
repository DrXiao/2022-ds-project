#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct queue_node queue_node;
typedef struct queue queue;

struct queue {
	bool (*insert)(queue *, void *);
	void *const (*first)(queue *);
	uint32_t (*retnodes)(queue *);
	bool (*erase)(queue *);
	void (*destroy)(queue *);
	size_t struct_size;
	uint32_t nodes;
	queue_node *head, *tail;
};

queue queue_init(size_t);

#endif
