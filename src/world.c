#include "world.h"

void world_free(World *self) {
    for (u32 i = 0; i < self->roomCount; ++i) {
        room_free(&self->rooms[i]);
    }

    free(self);
}

void world_load(World *self, const char *filename) {
    assert(self);
    assert(filename);

    JsonToken *root = json_parse(filename);
    Vector *roomVec = JSON_VECTOR(json_obj_get_array(root, "rooms"));

    assert(roomVec);

    self->roomCount = roomVec->size;
    self->rooms = calloc(self->roomCount, sizeof(Room));

    for (u32 i = 0; i < roomVec->size; ++i) {
        room_set(&self->rooms[i], vector_index(roomVec, i));
    }
}

Room *world_get_room(World *self, i32 id) {
    assert(self);

    for (u32 i = 0; i < self->roomCount; ++i) {
        if (self->rooms[i].id == id) {
            return &self->rooms[i];
        }
    }

    return NULL;
}