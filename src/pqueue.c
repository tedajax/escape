#include "pqueue.h"
#include <stdio.h>

PQueue *pqueue_new(u32 capacity, priority_f priorityF) {
	PQueue *self = (PQueue *)malloc(sizeof(PQueue));

	self->size = 0;
	self->capacity = 0;
	self->priorityFunc = priorityF;
	self->data = NULL;
	_pqueue_resize(self, capacity);

	return self;
}

void _pqueue_resize(PQueue *self, u32 capacity) {
	assert(self);

	if (capacity <= self->capacity) {
		return;
	}
	self->capacity = capacity;

	self->data = realloc(self->data, self->capacity * sizeof(void *)); 
}

void pqueue_push(PQueue *self, void *element) {
	assert(self);

	if (self->size >= self->capacity) {
		_pqueue_resize(self, self->capacity * 2);
	}

	self->data[self->size] = element;
	++self->size;

	if (self->size <= 1) {
		return;
	}

	u32 i = self->size - 2;
	i32 elemP = self->priorityFunc(element);
	while (elemP < self->priorityFunc(self->data[i])) {
		void *temp = self->data[i];
		self->data[i] = self->data[i + 1];
		self->data[i + 1] = temp;

		if (i == 0) {
			break;
		}

		--i;
	}
}

void *pqueue_peek(PQueue *self) {
	assert(self);

	return self->data[0];
}

void *pqueue_pop(PQueue *self) {
	assert(self);

	void *result = self->data[0];

	for (int i = 0; i < self->size - 1; ++i) {
		self->data[i] = self->data[i + 1];
	}
	self->data[self->size - 1] = NULL;
	--self->size;

	return result;
}

void pqueue_free(PQueue *self) {
	assert(self);

	free(self->data);
	free(self);
}