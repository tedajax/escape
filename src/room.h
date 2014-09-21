#ifndef _ROOM_H_
#define _ROOM_H_

#include "types.h"
#include "containers.h"
#include "json.h"

typedef struct room_t {
    i32 id;
    i32 exit_east;
    i32 exit_west;
    i32 exit_north;
    i32 exit_south;
    i32 exit_up;
    i32 exit_down;
    String *name;
    String *description;
    Vector *items;
} Room;

Room *room_new();
Room *room_set(Room *self, JsonToken *data);
void room_look(Room *self);

void room_free(Room *self);

#endif