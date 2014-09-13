#include "vector.h"

Vector *vector_new(u32 capacity) {
	Vector *self = (Vector *)malloc(sizeof(Vector));

	self->size = 0;
	self->freeFunc = NULL;
	self->data = NULL;
	_vector_resize(self, capacity);

	return self;
}

void vector_setFreeFunc(Vector *self, free_func freeFunc) {
	assert(self);
	assert(freeFunc);
	self->freeFunc = freeFunc;
}

void vector_add(Vector *self, void *element) {
	assert(self);
	assert(element);

	if (self->size >= self->capacity) {
		_vector_resize(self, self->capacity * 2);
	}

	self->data[self->size] = element;
	++self->size;
}

void *vector_index(Vector *self, u32 index) {
	assert(self);

	if (index >= self->size) {
		return NULL;
	}

	return self->data[index];
}

bool vector_remove(Vector *self, void *element) {
	assert(self);
	assert(element);

	for (int i = 0; i < self->size; ++i) {
		if (self->data[i] == element) {
			return vector_removeAt(self, i);;
		}
	}

	return false;
}

bool vector_removeAt(Vector *self, u32 index) {
	assert(self);

	if (index >= self->size) {
		return false;
	}

	if (self->freeFunc != NULL) {
		self->freeFunc(self->data[index]);
	} else {
		free(self->data[index]);
	}

	u32 i = index;
	while (i + 1 < self->size) {
		self->data[i] = self->data[i + 1];
		++i;
	}

	--self->size;

	return true;
}

void _vector_resize(Vector *self, u32 capacity) {
	assert(self);

	if (capacity <= self->size) {
		capacity = self->size + 1;
	}

	self->capacity = capacity;
	self->data = realloc(self->data, sizeof(void *) * capacity);
}