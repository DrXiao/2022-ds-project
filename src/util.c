#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "util.h"

void swap(void *obj1, void *obj2, size_t size) {
	char temp[size];
	memcpy(temp, obj1, size);
	memcpy(obj1, obj2, size);
	memcpy(obj2, temp, size);
}
