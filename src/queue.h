#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "types.h"

typedef struct queue_t {
	u32 head;
	u32 tail;
	u32 capacity;
	void **data;
} Queue;

Queue *queue_new(u32 capacity);
void queue_push(Queue *self, void *element);
void *queue_peek(Queue *self);
void *queue_pop(Queue *self);
i32 queue_size(Queue *self);

#endif