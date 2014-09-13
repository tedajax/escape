#include "queue.h"

Queue *queue_new(u32 capacity) {
	Queue *self = (Queue *)malloc(sizeof(Queue));

	self->capacity = capacity;
	self->head = 0;
	self->tail = 0;

	self->data = malloc(sizeof(void *) * capacity);

	return self;
}

void queue_push(Queue *self, void *element) {
	assert(self);
	assert(element);

	self->data[self->tail] = element;
	++self->tail;
}

void *queue_peek(Queue *self) {
	if (self->head == self->tail) {
		return NULL;
	}

	return self->data[self->head];
}

void *queue_pop(Queue *self) {
	if (self->head == self->tail) {
		return NULL;
	}
	
	void *presult = self->data[self->head];

	++self->head;
	if (self->head == self->tail) {
		self->head = 0;
		self->tail = 0;
	}

	return presult;
}

i32 queue_size(Queue *self) {
	return self->tail - self->head;
}