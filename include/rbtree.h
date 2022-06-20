#ifndef __RBTREE_H__
#define __RBTREE_H__
#include <stdint.h>
#include <stdbool.h>
#include "csv.h"

typedef struct rbnode rbnode;
typedef struct rbtree rbtree;

struct rbtree {
	void (*readcsv)(rbtree *, char *);
	bool (*insert)(rbtree *, void *);
	bool (*erase)(rbtree *, void *);
	uint32_t (*retnodes)(rbtree *);
	void (*sampled)(rbtree *, uint32_t);
	void (*merge)(rbtree *, rbtree *);
	void *const (*search)(rbtree *, void *);
	void (*destroy)(rbtree *);
	csv_op csvop;
	cmp compar;
	uint32_t nodes;
	uint32_t col;
	rbnode *root;
};

rbtree rbtree_init(csv_op *, cmp);

#endif
