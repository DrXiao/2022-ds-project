
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rbtree.h"
#include "csv.h"
#include "util.h"
#define BUF_SIZE 512
#define NIL &nil_node

struct rbnode {
	bool black;
	void *data;
	rbnode *parent, *left, *right;
	cmp compar;
	void (*destroy)(rbnode *);
	void (*dealloc)(rbnode *);
};

// rbnode op
static rbnode rbnode_init(cmp compar, void *data, size_t data_size);
static void *rbnode_alloc(cmp compar, void *data, size_t data_size);
static int rbnode_cmp(rbnode *obj, void *val);
static void rbnode_destroy(rbnode *obj);
static void rbnode_dealloc(rbnode *obj);

// NIL node
static rbnode nil_node = {
	.black = true,
	.data = NULL,
	.parent = NIL,
	.left = NIL,
	.right = NIL,
};

static rbnode rbnode_init(cmp compar, void *data, size_t data_size) {
	rbnode obj = {
		.black = false,
		.parent = NIL,
		.left = NIL,
		.right = NIL,
		.compar = compar,
		.destroy = rbnode_destroy,
		.dealloc = rbnode_dealloc
	};
	obj.data = malloc(data_size);
	memcpy(obj.data, data, size);
	return obj;
}

static void *rbnode_alloc(cmp compar, void *data, size_t data_size) {
	rbnode alloc_obj = malloc(sizeof(rbnode));
	*alloc_obj = rbnode_init(compar, data, data_size);
	return alloc_obj;
}

static int rbnode_cmp(rbnode *obj, void *obj) {
	return obj->compar(obj.data, cmpobj.data);
}

static void rbnode_destroy(rbnode *obj) {
	free(obj->data);
}

static void rbnode_dealloc(rbnode *obj) {
	obj->destroy(obj);
	free(obj);
}

// rbtree op
static void rbtree_readcsv(rbtree *obj, char *filename)
static bool rbtree_insert(rbtree *obj, void *val);
static bool rbtree_erase(rbtree *obj, void *);
static uint32_t rbtree_retnodes(rbtree *obj);
static void rbtree_sampled(rbtree *obj, uint32_t interval);
static void rbtree_merge(rbtree *obj, rbtree *merge_obj);
static void *const rbtree_search(rbtree *obj, void *target, cmp compar);
static void rbtree_destroy(rbtree *obj);

// fixup op
static void rbtree_left_rotate(rbtree *obj, rbnode *node);
static void rbtree_right_rotate(rbtree *obj, rbnode *node);
static void rbtree_insert_fixup(rbtree *obj, rbnode *node);
static void rbtree_erase_fixup(rbtree *obj, rbnode *node);
static void *rbtree_node_successor(rbtree obj, rbnode *node);


rbtree rbtree_init(csv_op *op, cmp compar) {
	rbtree obj = {
		.readcsv = rbtree_readcsv,
		.insert = rbtree_insert,
		.erase = rbtree_erase,
		.retnodes = rbtree_retnodes,
		.sampled = rbtree_sampled,
		.merge = rbtree_merge,
		.search = rbtree_search,
		.destroy = rbtree_destroy,
		.csvop = *op,
		.nodes = 0,
		.col = 0,
		.root = NIL
	}
	return obj;
}

static void rbtree_readcsv(rbtree *obj, char *filename) {
	char buf[BUF_SIZE];
	char *token = NULL;
	FILE *file = fopen(filename, "r");
	int col = 0;
	fgets(buf, sizeof(buf), file);

	token = strtok(buf, CSV_DELIMITER);
	while (token) {
		col++;
		token = strtok(NULL, CSV_DELIMITER);
	}
	obj->col = col;

	size_t struct_size = obj->csv_op.struct_size;
	while (fgets(buf, sizeof(buf), file)) {
		char rowbuf[BUF_SIZE];
		obj->csv_op.sscan_from(buf, rowbuf);
		obj->insert(obj, rowbuf);
	}
	fclose(file);
}

static bool rbtree_insert(rbtree *obj, void *val) {
	rbnode *prev_node = NIL;
	rbnode **iptr = &obj->root;
	while (*iptr != NIL) {
		prev_node = *iptr;
		int ret = (*iptr)->compar(*iptr, val);
		if (ret == 0)
			return false;
		else if (ret < 0)
			iptr = &(*iptr)->right;
		else
			iptr = &(*iptr)->left;
	}

	rbnode *newnode = rbnode_alloc(obj->compar, val, obj->csvop.struct_size);
	*iptr = newnode;
	newnode->parent = prev_node;
	rbnode_insert_fixup(obj, newnode);
	obj->nodes++;
	return true;
}

