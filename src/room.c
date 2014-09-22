#include "room.h"

Room *room_new() {
    Room *self = calloc(1, sizeof(Room));
    return self;
}

void room_free(Room *self) {
    assert(self);

    if (self->description) {
        string_free(self->description);
    }

    if (self->name) {
        string_free(self->name);
    }
}

Room *room_set(Room *self, JsonToken *data) {
    self->id = json_obj_get_int(data, "id");
    self->name = json_obj_get_string(data, "name");
    self->description = json_obj_get_string(data, "look_desc");

    JsonToken *connections = json_obj_get_object(data, "connections");
    if (connections) {
        for (u32 i = 0; i < EXIT_COUNT; ++i) {
            self->exits[i] = json_obj_get_int(connections, EXIT_NAMES[i]);
        }
    }
    json_token_free(connections);

    return self;
}

void room_look(Room *self) {
    assert(self);

    printf("%s:\n", self->name->characters);
    printf("%s", self->description->characters);

    i32 exitCount = room_exit_count(self);
    if (exitCount == 0) {
        printf("  There are no exits... what have you done?");
    } else if (exitCount == 1) {
        printf("  There is one exit to the %s.", room_get_exit_name(self));
    } else {
        char *directions = calloc(64, sizeof(char));
        room_get_exit_names(self, directions);
        printf("  Exits are %s", directions);
        free(directions);
    }

    printf("\n");
}

i32 room_exit_count(Room *self) {
    assert(self);

    i32 result = 0;
    for (u32 i = 0; i < EXIT_COUNT; ++i) {
        if (self->exits[i] > 0) {
            ++result;
        }
    }

    return result;
}

const char *room_get_exit_name(Room *self) {
    assert(self);

    for (u32 i = 0; i < EXIT_COUNT; ++i) {
        if (self->exits[i] > 0) {
            return EXIT_NAMES[i];
        }
    }

    return "none";
}

void room_get_exit_names(Room *self, char *dest) {
    assert(self);

    i32 exitCount = room_exit_count(self);
    ASSERT(exitCount >= 2,
        "Only use this function for names if there's more than 1 exit.");

    const char *commaStr = ", ";
    const char *andStr = " and ";
    const char *periodStr = ".";

    char *exitEnd = (exitCount == 2) ? andStr : commaStr;

    i32 exitsNamed = 0;
    u32 currentIndex = 0;

    for (u32 i = 0; i < EXIT_COUNT; ++i) {
        if (self->exits[i] > 0) {
            ++exitsNamed;
            strcpy(&dest[currentIndex], EXIT_NAMES[i]);
            currentIndex += strlen(EXIT_NAMES[i]);
            strcpy(&dest[currentIndex], exitEnd);
            currentIndex += strlen(exitEnd);
        }

        if (exitsNamed == exitCount - 2) {
            exitEnd = andStr;
        } else if (exitsNamed == exitCount - 1) {
            exitEnd = periodStr;
        }
    }
}