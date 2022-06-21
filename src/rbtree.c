#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rbtree.h"
#include "util.h"
#define BUF_SIZE 512
#define NIL &nil_node

struct rbnode {
	bool black;
	void *data;
	rbnode *parent, *left, *right;
	cmp compar;
	int (*compar_data)(rbnode *, void *);
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
		.compar_data = rbnode_cmp,
		.destroy = rbnode_destroy,
		.dealloc = rbnode_dealloc
	};
	obj.data = malloc(data_size);
	memcpy(obj.data, data, data_size);
	return obj;
}

static void *rbnode_alloc(cmp compar, void *data, size_t data_size) {
	rbnode *alloc_obj = malloc(sizeof(rbnode));
	*alloc_obj = rbnode_init(compar, data, data_size);
	return alloc_obj;
}

static int rbnode_cmp(rbnode *obj, void *data) {
	return obj->compar(obj->data, data);
}

static void rbnode_destroy(rbnode *obj) {
	free(obj->data);
}

static void rbnode_dealloc(rbnode *obj) {
	obj->destroy(obj);
	free(obj);
}

// rbtree op
static void rbtree_readcsv(rbtree *obj, const char *filename, sscan sscan_from);
static bool rbtree_insert(rbtree *obj, void *val);
static bool rbtree_erase(rbtree *obj, void *val);
static uint32_t rbtree_retnodes(rbtree *obj);
static void rbtree_sampled(rbtree *obj, uint32_t interval);
static void rbtree_merge(rbtree *obj, rbtree *merge_obj);
static void *const rbtree_search(rbtree *obj, void *target);
static void rbtree_preorder(rbtree *obj, void (*behavior)(void *));
static void rbtree_inorder(rbtree *obj, void (*behavior)(void *));
static void rbtree_postorder(rbtree *obj, void (*behavior)(void *));
static void rbtree_preorder_node(rbnode *obj, void (*behavior)(void *));
static void rbtree_inorder_node(rbnode *obj, void (*behavior)(void *));
static void rbtree_postorder_node(rbnode *obj, void (*behavior)(void *));
static void rbtree_destroy(rbtree *obj);
static void rbtree_node_destroy(rbnode *obj);

// fixup op
static void rbtree_left_rotate(rbtree *obj, rbnode *node);
static void rbtree_right_rotate(rbtree *obj, rbnode *node);
static void rbtree_insert_fixup(rbtree *obj, rbnode *node);
static void rbtree_erase_fixup(rbtree *obj, rbnode *node);
static void *rbtree_node_successor(rbtree *obj, rbnode *node);


rbtree rbtree_init(size_t struct_size, cmp compar) {
	rbtree obj = {
		.readcsv = rbtree_readcsv,
		.insert = rbtree_insert,
		.erase = rbtree_erase,
		.retnodes = rbtree_retnodes,
		.sampled = rbtree_sampled,
		.merge = rbtree_merge,
		.search = rbtree_search,
		.preorder = rbtree_preorder,
		.inorder = rbtree_inorder,
		.postorder = rbtree_postorder,
		.destroy = rbtree_destroy,
		.struct_size = struct_size,
		.compar = compar,
		.nodes = 0,
		.col = 0,
		.root = NIL
	};
	return obj;
}

static void rbtree_readcsv(rbtree *obj, const char *filename, sscan sscan_from) {
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

	size_t struct_size = obj->struct_size;
	while (fgets(buf, sizeof(buf), file)) {
		char rowbuf[BUF_SIZE];
		sscan_from(buf, rowbuf);
		obj->insert(obj, rowbuf);
	}
	fclose(file);
}

static bool rbtree_insert(rbtree *obj, void *val) {
	rbnode *prev_node = NIL;
	rbnode **iptr = &obj->root;
	while (*iptr != NIL) {
		prev_node = *iptr;
		int ret = (*iptr)->compar_data(*iptr, val);
		if (ret == 0)
			return false;
		else if (ret < 0)
			iptr = &(*iptr)->right;
		else
			iptr = &(*iptr)->left;
	}

	rbnode *newnode = rbnode_alloc(obj->compar, val, obj->struct_size);
	*iptr = newnode;
	newnode->parent = prev_node;
	rbtree_insert_fixup(obj, newnode);
	obj->nodes++;
	return true;
}

static bool rbtree_erase(rbtree *obj, void *val) {
	rbnode **iptr = &obj->root;
	while(*iptr != NIL) {
		int ret = (*iptr)->compar_data((*iptr), val);
		if (ret == 0) {
			bool erase_black = false;
			rbnode *successor = rbtree_node_successor(obj, (*iptr));
			rbnode *erase_node = NIL;
			if (successor != NIL) {
				void *tmp = (*iptr)->data;
				(*iptr)->data = successor->data;
				successor->data = tmp;
				iptr = (successor->parent->left == successor) ? 
					&successor->parent->left : &successor->parent->right;
			}
			erase_node = *iptr;
			erase_black = erase_node->black;
			*iptr = (erase_node->left != NIL) ? erase_node->left : erase_node->right;
			(*iptr)->parent = erase_node->parent;
			erase_node->dealloc(erase_node);
			if (erase_black) {
				rbtree_erase_fixup(obj, *iptr);
			}
			nil_node.black = true;
			nil_node.parent = NIL;
			return true;

		}
		else if (ret < 0)
			iptr = &(*iptr)->right;
		else
			iptr = &(*iptr)->left;
	}
	return false;
}

static uint32_t rbtree_retnodes(rbtree *obj) {
	return obj->nodes;
}

static void rbtree_sampled(rbtree *obj, uint32_t interval) {

}

static void rbtree_merge(rbtree *obj, rbtree *merge_obj) {

}

static void *const rbtree_search(rbtree *obj, void *target) {
	rbnode *ptr = obj->root;
	while (ptr != NIL) {
		int ret = ptr->compar_data(ptr, target);
		if (ret == 0)
			return ptr->data;
		else if (ret < 0)
			ptr = ptr->right;
		else
			ptr = ptr->left;
	}
	return NULL;
}

static void rbtree_preorder(rbtree *obj, void (*behavior)(void *)) {
	rbtree_preorder_node(obj->root, behavior);
}

static void rbtree_inorder(rbtree *obj, void (*behavior)(void *)) {
	rbtree_inorder_node(obj->root, behavior);
}

static void rbtree_postorder(rbtree *obj, void (*behavior)(void *)) {
	rbtree_postorder_node(obj->root, behavior);
}

static void rbtree_preorder_node(rbnode *obj, void (*behavior)(void *)) {
	if (obj != NIL) {
		behavior(obj->data);
		rbtree_preorder_node(obj->left, behavior);
		rbtree_preorder_node(obj->right, behavior);
	}
}

static void rbtree_inorder_node(rbnode *obj, void (*behavior)(void *)) {
	if (obj != NIL) {
		rbtree_inorder_node(obj->left, behavior);
		behavior(obj->data);
		rbtree_inorder_node(obj->right, behavior);
	}
}

static void rbtree_postorder_node(rbnode *obj, void (*behavior)(void *)) {
	if (obj != NIL) {
		rbtree_postorder_node(obj->left, behavior);
		rbtree_postorder_node(obj->right, behavior);
		behavior(obj->data);
	}
}

static void rbtree_destroy(rbtree *obj) {
	rbtree_node_destroy(obj->root);
	obj->root = NIL;
}

static void rbtree_node_destroy(rbnode *obj) {
	if (obj != NIL) {
		rbtree_node_destroy(obj->left);
		rbtree_node_destroy(obj->right);
		obj->dealloc(obj);
	}
}

static void rbtree_left_rotate(rbtree *obj, rbnode *node) {
	rbnode *parent = node->parent;
	rbnode *right = node->right;
	rbnode **parent_child = NULL;
	
	if (node == parent->left)
		parent_child = &parent->left;
	else
		parent_child = &parent->right;

	node->right = right->left;

	if (right->left != NIL)
		right->left->parent = node;
	
	right->parent = parent;

	if (parent != NIL)
		*parent_child = right;
	else
		obj->root = right;
	
	right->left = node;
	node->parent = right;
}

static void rbtree_right_rotate(rbtree *obj, rbnode *node) {
	rbnode *parent = node->parent;
	rbnode *left = node->left;
	rbnode **parent_child = NULL;
	if (node == parent->left)
		parent_child = &parent->left;
	else
		parent_child = &parent->right;
	
	node->left = left->right;

	if (left->right != NIL)
		left->right->parent = node;
	
	left->parent = parent;

	if (parent != NIL)
		*parent_child = left;
	else
		obj->root = left;

	left->right = node;
	node->parent = left;
}

static void rbtree_insert_fixup(rbtree *obj, rbnode *node) {
	while(node->parent->black == false) {
		rbnode *parent = node->parent;
		rbnode *grandparent = parent->parent;
		rbnode *uncle = NIL;

		if (parent == grandparent->left)
			uncle = grandparent->right;
		else
			uncle = grandparent->left;

		if (uncle->black == false) {
			parent->black = true;
			uncle->black = true;
			grandparent->black = false;
			node = grandparent;
		}
		else if (parent == grandparent->left) {
			if (node == parent->right) {
				node = parent;
				rbtree_left_rotate(obj, node);
				parent = node->parent;
				grandparent = parent->parent;
			}
			parent->black = true;
			grandparent->black = false;
			rbtree_right_rotate(obj, grandparent);
		}
		else {
			if (node == parent->left) {
				node = parent;
				rbtree_right_rotate(obj, node);
				parent = node->parent;
				grandparent = parent->parent;
			}
			parent->black = true;
			grandparent->black = false;
			rbtree_left_rotate(obj, grandparent);
		}
	}
	obj->root->black = true;
}

static void rbtree_erase_fixup(rbtree *obj, rbnode *node) {
	while (node != obj->root && node->black) {
		rbnode *parent = node->parent;
		bool node_is_left = (node == parent->left);
		rbnode *sibling = node_is_left ? parent->right : parent->left;
		if (sibling->black == false) {
			sibling->black = true;
			node->parent->black = false;
			if (node_is_left) {
				rbtree_left_rotate(obj, parent);
				sibling = parent->right;
			}
			else {
				rbtree_right_rotate(obj, parent);
				sibling = parent->left;
			}
		}
		if (sibling->left->black && sibling->right->black) {
			sibling->black = false;
			node = parent;
		}
		else if (node_is_left) {
			if (sibling->right->black) {
				sibling->black = false;
				sibling->left->black = true;
				rbtree_right_rotate(obj, sibling);
				sibling = parent->right;
			}
			sibling->black = parent->black;
			parent->black = true;
			sibling->right->black = true;
			rbtree_left_rotate(obj, parent);
			node = obj->root;
		}
		else {
			if (sibling->left->black) {
				sibling->black = false;
				sibling->right->black = true;
				rbtree_left_rotate(obj, sibling);
				sibling = parent->left;
			}
			sibling->black = parent->black;
			parent->black = true;
			sibling->left->black = true;
			rbtree_right_rotate(obj, parent);
			node = obj->root;
		}
	}
	node->black = true;
}

static void *rbtree_node_successor(rbtree *obj, rbnode *node) {
	rbnode *successor = node->right;
	while(successor->left != NIL)
		successor = successor->left;
	return successor;
}
