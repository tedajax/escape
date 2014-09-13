#ifndef _PQUEUE_H_
#define _PQUEUE_H_

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "types.h"

typedef i32(*priority_f)(void *);

typedef struct pqueue_t {
	u32 size;
	u32 capacity;
	priority_f priorityFunc;
	void **data;
} PQueue;

PQueue *pqueue_new(u32 capacity, priority_f priorityF);
void pqueue_push(PQueue *self, void *element);
void *pqueue_peek(PQueue *self);
void *pqueue_pop(PQueue *self);
void pqueue_free(PQueue *self);

void _pqueue_resize(PQueue *self, u32 capacity);

#endif