#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "queue.h"

struct queue_node {
	void *data;
	queue_node *next;
	void (*destroy)(queue_node *);
	void (*dealloc)(queue_node *);
};

// queue node op
static queue_node queue_node_init(void *data, size_t data_size);
static queue_node *queue_node_alloc(void *data, size_t data_size);
static void queue_node_destroy(queue_node *obj);
static void queue_node_dealloc(queue_node *obj);

static queue_node queue_node_init(void *data, size_t data_size) {
	queue_node obj = {
		.next = NULL,
		.destroy = queue_node_destroy,
		.dealloc = queue_node_dealloc,
	};
	obj.data = malloc(data_size);
	memcpy(obj.data, data, data_size);
	return obj;
}

static queue_node *queue_node_alloc(void *data, size_t data_size) {
	queue_node *alloc_obj = malloc(sizeof(queue_node));
	*alloc_obj = queue_node_init(data, data_size);
	return alloc_obj;
}

static void queue_node_destroy(queue_node *obj) {
	free(obj->data);
}

static void queue_node_dealloc(queue_node *obj) {
	obj->destroy(obj);
	free(obj);
}

// queue op
static bool queue_insert(queue *obj, void *data);
static void *const queue_first(queue *obj);
static uint32_t queue_retnodes(queue *obj);
static bool queue_erase(queue *obj);
static void queue_destroy(queue *obj);

queue queue_init(size_t struct_size) {
	queue obj = {.insert = queue_insert,
		     .first = queue_first,
		     .retnodes = queue_retnodes,
		     .erase = queue_erase,
		     .destroy = queue_destroy,
		     .struct_size = struct_size,
		     .nodes = 0,
		     .head = NULL,
		     .tail = NULL};
	return obj;
}

static bool queue_insert(queue *obj, void *data) {
	queue_node *newnode = queue_node_alloc(data, obj->struct_size);
	if (obj->tail) {
		obj->tail->next = newnode;
		obj->tail = newnode;
	}
	else {
		obj->head = obj->tail = newnode;
	}
	obj->nodes++;
	return true;
}

static void *const queue_first(queue *obj) {
	if (obj->head)
		return obj->head->data;
	return NULL;
}

static uint32_t queue_retnodes(queue *obj) {
	return obj->nodes;
}

static bool queue_erase(queue *obj) {
	queue_node *node = obj->head;
	if (!node)
		return false;
	if (obj->head == obj->tail)
		obj->tail = NULL;
	obj->head = obj->head->next;
	node->dealloc(node);
	obj->nodes--;
}

static void queue_destroy(queue *obj) {
	while (obj->erase(obj))
		;
}
