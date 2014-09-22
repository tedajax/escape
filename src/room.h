#ifndef _ROOM_H_
#define _ROOM_H_

#include "types.h"
#include "containers.h"
#include "json.h"
#include "term.h"

typedef enum room_exit_e {
    EXIT_EAST = 0,
    EXIT_WEST,
    EXIT_NORTH,
    EXIT_SOUTH,
    EXIT_UP,
    EXIT_DOWN,

    EXIT_COUNT
} RoomExit;

extern const char *EXIT_NAMES[EXIT_COUNT];

typedef struct room_t {
    i32 id;
    i32 exits[EXIT_COUNT];
    String *name;
    String *description;
    Vector *items;
} Room;

Room *room_new();
void room_free(Room *self);

Room *room_set(Room *self, JsonToken *data);
void room_look(Room *self);

i32 room_exit_count(Room *self);
const char *room_get_exit_name(Room *self);
void room_get_exit_names(Room *self, char *dest);

#endif