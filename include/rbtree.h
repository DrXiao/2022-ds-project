#ifndef __RBTREE_H__
#define __RBTREE_H__
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "util.h"

typedef struct rbnode rbnode;
typedef struct rbtree rbtree;

struct rbtree {
	void (*readcsv)(rbtree *, const char *, sscan);
	bool (*insert)(rbtree *, void *);
	bool (*erase)(rbtree *, void *);
	uint32_t (*retnodes)(rbtree *);
	void (*sampled)(rbtree *, uint32_t);
	void (*merge)(rbtree *, rbtree *);
	void *const (*search)(rbtree *, void *);
	void (*preorder)(rbtree *, void (*)(void *));
	void (*inorder)(rbtree *, void (*)(void *));
	void (*postorder)(rbtree *, void (*)(void *));
	void (*destroy)(rbtree *);
	size_t struct_size;
	cmp compar;
	uint32_t nodes;
	uint32_t col;
	rbnode *root;
};

rbtree rbtree_init(size_t, cmp);

#endif
