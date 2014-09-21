#include "room.h"

Room *room_new() {
    Room *self = calloc(1, sizeof(Room));
    return self;
}

Room *room_set(Room *self, JsonToken *data) {
    self->id = json_obj_get_int(data, "id");
    self->name = json_obj_get_string(data, "name");
    self->description = json_obj_get_string(data, "look_desc");

    JsonToken *connections = json_obj_get_object(data, "connections");
    if (connections) {
        self->exit_east = json_obj_get_int(connections, "east");
        self->exit_west = json_obj_get_int(connections, "west");
        self->exit_north = json_obj_get_int(connections, "north");
        self->exit_south = json_obj_get_int(connections, "south");
        self->exit_up = json_obj_get_int(connections, "up");
        self->exit_down = json_obj_get_int(connections, "down");
    }
    json_token_free(connections);

    return self;
}

void room_look(Room *self) {
    assert(self);

    printf("%s:\n", self->name->characters);
    printf("%s", self->description->characters);
    printf("\n");
}

void room_free(Room *self) {
    assert(self);
    string_free(self->description);
    string_free(self->name);
}