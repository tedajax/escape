#ifndef _WORLD_H_
#define _WORLD_H_

#include <stdlib.h>

#include "types.h"
#include "containers.h"
#include "json.h"
#include "room.h"

typedef struct world_t {
    JsonToken *worldData;
    u32 roomCount;
    Room *rooms;
} World;

void world_free(World *self);

void world_load(World *self, const char *filename);
Room *world_get_room(World *self, i32 id);

#endif